#include "io/IOManager.h"
#include "Logger/Logger.h"
#include "common/macro.h"

#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>

#define READ_TICKLE_PIPE   0   //读管道句柄编号
#define WRITE_TICKLE_PIPE  1   //写管道句柄编号

namespace siem{

enum EpollCtlOp {
};

static Logger::ptr syslog = GET_LOG_BY_NAME(system);

static thread_local IOManager* t_io_manager = nullptr;

IOManager::FdContext::EventContext& IOManager::FdContext::getContext(IOManager::Event e)
{
    switch (e)
    {
    case Event::READ:
        return read;
    case Event::WRITE:
        return write;
    default:
        SIEM_ASSERT_STR(false, "get context");
    }

    throw std::invalid_argument("getContext");
}

void IOManager::FdContext::resetContext(EventContext& e)
{
    e.cb = nullptr;
    e.fiber.reset();
    e.sc = nullptr;
}

void IOManager::FdContext::triggerEvent(IOManager::Event e)
{
    SIEM_ASSERT(m_events & e);

    m_events = (Event) (m_events & ~e);
    EventContext& ctx = getContext(e);
    if (ctx.cb) {
        ctx.sc->schedule(&ctx.cb);
    } else {
        ctx.sc->schedule(&ctx.fiber);
    }

    ctx.sc = nullptr;
    return;
}

IOManager::IOManager(int thread, bool use_caller, const std::string& name)
    : m_name(name)
    , Scheduler(thread, use_caller, name)
    , m_epfd(epoll_create(5000)) {
    
    SIEM_ASSERT(m_epfd > 0);

    //创建pipe，获取m_tickleFds[2]，其中m_tickleFds[0]是管道的读端，m_tickleFds[1]是管道的写端
    int ret = pipe(m_tickle_fds);
    SIEM_ASSERT(!ret);

    epoll_event event;
    memset(&event, 0, sizeof(event));

    //设置Edge Trigger, 边缘触发, 此时读取数据就需要配合非阻塞使用
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickle_fds[READ_TICKLE_PIPE];

    //设置非阻塞方式，配合边缘触发
    auto flag = fcntl(m_tickle_fds[READ_TICKLE_PIPE], F_GETFL);
    flag |= O_NONBLOCK;
    ret = fcntl(m_tickle_fds[READ_TICKLE_PIPE], F_SETFL, flag);
    SIEM_ASSERT(ret);

    //将管道的读描述符加入epoll多路复用，如果管道可读，idle中的epoll_wait会返回
    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickle_fds[READ_TICKLE_PIPE], &event);
    SIEM_ASSERT(ret);

    contextResize(64);

    //开启调度
    start();

}

IOManager::~IOManager()
{
    stop();
    close(m_epfd);
    close(m_tickle_fds[READ_TICKLE_PIPE]);
    close(m_tickle_fds[WRITE_TICKLE_PIPE]);

    for (size_t i = 0 ; i < m_FdContexts.size() ; ++i) {
        if (m_FdContexts[i]) {
            delete m_FdContexts[i];
            m_FdContexts[i] = nullptr;
        } else {
            m_FdContexts[i] = nullptr;
        }
    }
}

int IOManager::addEvent(int fd, Event event, std::function<void()> cb)
{
    FdContext *fd_ctx = nullptr;
    RWMutexType::ReadLock rlock(m_mutex);
    if (m_FdContexts.size() > fd) {
        fd_ctx = m_FdContexts[fd];
        rlock.unlock();
    } else {
        rlock.unlock();
        RWMutexType::WriteLock wlock(m_mutex);
        contextResize(fd * 1.5);
        fd_ctx = m_FdContexts[fd];
    }

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);
    //同一个fd不允许添加相同类型的事件
    if (fd_ctx->m_events & event) {
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "add Event failed";
        SIEM_ASSERT(!(fd_ctx->m_events & event));
    }

    //将新的事件加入epoll_wait，使用epoll_event的私有指针存储FdContext的位置
    int op = fd_ctx->m_events ? EPOLL_CTL_MOD : EPOLL_CTL_ADD;
    epoll_event epevent;
    epevent.events = EPOLLET | fd_ctx->m_events | event;
    epevent.data.ptr = fd_ctx;
    int ret = epoll_ctl(m_epfd, op, fd_ctx->fd, &epevent);
    if (ret) {
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "epoll_ctl<" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << ");"
            << ret << " (" << errno << ") (" << strerror(errno) << ")";

        return -1;
    }

    // 待执行IO事件数加1
    ++m_pendingEventCount;

    // 找到这个fd的event事件对应的EventContext，对其中的scheduler, cb, fiber进行赋值
    fd_ctx->m_events = (Event)(fd_ctx->m_events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    SIEM_ASSERT(!event_ctx.sc && !event_ctx.fiber && !event_ctx.cb);

    // 赋值scheduler和回调函数，如果回调函数为空，则把当前协程当成回调执行体
    event_ctx.sc = Scheduler::getThis();

    if (cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::getThis();
        SIEM_ASSERT(event_ctx.fiber->getState() == Fiber::State::EXEC);
    }

    return 0;

}

bool IOManager::delEvent(int fd, Event event)
{
    RWMutexType::ReadLock rlock(m_mutex);

    // 找到fd对应的FdContext
    if (m_FdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_FdContexts[fd];
    rlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);
    if (SIEM_UNLIKELY(!(fd_ctx->m_events & event))) {
        return false;
    }

    // 清除指定的事件，表示不关心这个事件了，如果清除之后结果为0，则从epoll_wait中删除该文件描述符
    Event new_event = (Event) (fd_ctx->m_events & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_event;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if (ret) {
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "epoll_ctl<" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << ");"
            << ret << " (" << errno << ") (" << strerror(errno) << ")";

        return false;
    }

    // 待执行事件数减1
    --m_pendingEventCount;

    // 重置该fd对应的event事件上下文
    fd_ctx->m_events = new_event;
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);
    fd_ctx->resetContext(event_ctx);

    return true;
}

bool IOManager::cancelEvent(int fd, Event event)
{
    RWMutexType::ReadLock rlock(m_mutex);

    if (m_FdContexts.size() <= fd) {
        return false;
    }
    // 找到fd对应的FdContext
    FdContext* fd_ctx = m_FdContexts[fd];
    rlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);

    if (SIEM_UNLIKELY(!(fd_ctx->m_events & event))) {
        return false;
    }
    // 删除事件
    Event new_event = (Event) (fd_ctx->m_events & ~event);
    int op = new_event ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = EPOLLET | new_event;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if (ret) {
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "epoll_ctl<" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << ");"
            << ret << " (" << errno << ") (" << strerror(errno) << ")";

        return false;
    }

    // 删除之前触发一次事件
    fd_ctx->triggerEvent(event);
    
    //活跃事件数减1
    --m_pendingEventCount;

    return true;
}

bool IOManager::cancelAll(int fd)
{
    RWMutexType::ReadLock rlock(m_mutex);
    if (m_FdContexts.size() <= fd) {
        return false;
    }
    FdContext* fd_ctx = m_FdContexts[fd];
    rlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);

    if (!(fd_ctx->m_events)) {
        return false;
    }

    // 删除全部事件
    int op = EPOLL_CTL_DEL;
    epoll_event epevent;
    epevent.events = 0;
    epevent.data.ptr = fd_ctx;

    int ret = epoll_ctl(m_epfd, op, fd, &epevent);
    if (ret) {
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "epoll_ctl<" << m_epfd << ", " 
            << op << ", " << fd << ", " << epevent.events << ");"
            << ret << " (" << errno << ") (" << strerror(errno) << ")";

        return false;
    }

    // 触发全部已注册的事件
    if(fd_ctx->m_events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_pendingEventCount;
    }
    if(fd_ctx->m_events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_pendingEventCount;
    }

    SIEM_ASSERT(fd_ctx->m_events == 0);
    return true;
}

IOManager* IOManager::getThis()
{
    return dynamic_cast<IOManager*>(Scheduler::getThis());
}

bool IOManager::stopping(void)
{
    return m_pendingEventCount == 0 && Scheduler::stopping();
}

void IOManager::tickle(void)
{
    if(!hasIdleThreads()) {
        return;
    }
    int rt = write(m_tickle_fds[1], "T", 1);
    SIEM_ASSERT(rt == 1);
}

void IOManager::idle(void)
{
    LOG_DEBUG(GET_LOG_BY_NAME( )) << "io manager idle";
    const uint64_t MAX_EVENTS = 256;
    epoll_event* events = new epoll_event[MAX_EVENTS]();

    //使用智能指针管理epoll事件, 自定义删除器
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete[] ptr;
    });

    while(true) {
        /*uint64_t next_timeout = 0;
        if(SIEM_UNLIKELY(stopping(next_timeout))) {
            LOG_DEBUG(GET_LOG_BY_NAME(system)) << "name=" << ""
                                     << " idle stopping exit";
            break;
        }

        int rt = 0;
        do {
            static const int MAX_TIMEOUT = 3000;
            if(next_timeout != ~0ull) {
                next_timeout = (int)next_timeout > MAX_TIMEOUT
                                ? MAX_TIMEOUT : next_timeout;
            } else {
                next_timeout = MAX_TIMEOUT;
            }
            rt = epoll_wait(m_epfd, events, MAX_EVNETS, (int)next_timeout);
            if(rt < 0 && errno == EINTR) {
            } else {
                break;
            }
        } while(true);

        std::vector<std::function<void()> > cbs;

        //暂未实现
        if(!cbs.empty()) {
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        for(int i = 0; i < rt; ++i) {
            epoll_event& event = events[i];
            if(event.data.fd == m_tickle_fds[0]) {
                uint8_t dummy[256];
                while(read(m_tickle_fds[0], dummy, sizeof(dummy)) > 0);
                continue;
            }

            FdContext* fd_ctx = (FdContext*)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->m_mutex);
            if(event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->m_events;
            }
            int real_events = None;
            if(event.events & EPOLLIN) {
                real_events |= READ;
            }
            if(event.events & EPOLLOUT) {
                real_events |= WRITE;
            }

            if((fd_ctx->m_events & real_events) == None) {
                continue;
            }

            int left_events = (fd_ctx->m_events & ~real_events);
            int op = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events = EPOLLET | left_events;

            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if(rt2) {
                LOG_DEBUG(GET_LOG_BY_NAME(system)) << "epoll_ctl(" << m_epfd << ", "
                    << (EpollCtlOp)op << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):"
                    << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }

            if(real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_penddingEventCount;
            }
            if(real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_penddingEventCount;
            }
        }

        Fiber::ptr cur = Fiber::getThis();
        auto raw_ptr = cur.get();
        cur.reset();

        raw_ptr->swapOut();*/

        if (SIEM_UNLIKELY(stopping())) {
            DEBUG() << "name=" << getName() << "idle stopping exit";
            break;
        }

        // 阻塞在epoll_wait上，等待事件发生
        static const int MAX_TIMEOUT = 5000;
        int rt = epoll_wait(m_epfd, events, MAX_EVENTS, MAX_TIMEOUT);
        if(rt < 0) {
            if(errno == EINTR) {
                continue;
            }
            ERROR() << "epoll_wait(" << m_epfd << ") (rt="
                                      << rt << ") (errno=" << errno << ") (errstr:" << strerror(errno) << ")";
            break;
        }

        // 遍历所有发生的事件，根据epoll_event的私有指针找到对应的FdContext，进行事件处理
        for (int i = 0; i < rt; ++i) {
            epoll_event &event = events[i];
            if (event.data.fd == m_tickle_fds[0]) {
                // m_tickle_fds[0]用于通知协程调度，这时只需要把管道里的内容读完即可，本轮idle结束Scheduler::run会重新执行协程调度
                uint8_t dummy[256];
                while (read(m_tickle_fds[0], dummy, sizeof(dummy)) > 0);
                continue;
            }
             
            // 通过epoll_event的私有指针获取FdContext
            FdContext *fd_ctx = (FdContext *)event.data.ptr;
            FdContext::MutexType::Lock lock(fd_ctx->m_mutex);
            /**
             * EPOLLERR: 出错，比如写读端已经关闭的pipe
             * EPOLLHUP: 套接字对端关闭
             * 出现这两种事件，应该同时触发fd的读和写事件，否则有可能出现注册的事件永远执行不到的情况
             */
            if (event.events & (EPOLLERR | EPOLLHUP)) {
                event.events |= (EPOLLIN | EPOLLOUT) & fd_ctx->m_events;
            }
            int real_events = Event::None;
            if (event.events & EPOLLIN) {
                real_events |= Event::READ;
            }
            if (event.events & EPOLLOUT) {
                real_events |= Event::WRITE;
            }
 
            if ((fd_ctx->m_events & real_events) == Event::None) {
                continue;
            }
 
            // 剔除已经发生的事件，将剩下的事件重新加入epoll_wait，
            // 如果剩下的事件为0，表示这个fd已经不需要关注了，直接从epoll中删除
            int left_events = (fd_ctx->m_events & ~real_events);
            int op          = left_events ? EPOLL_CTL_MOD : EPOLL_CTL_DEL;
            event.events    = EPOLLET | left_events;
 
            int rt2 = epoll_ctl(m_epfd, op, fd_ctx->fd, &event);
            if (rt2) {
                ERROR() << "epoll_ctl(" << m_epfd << ", "
                                          << (EpollCtlOp)op << ", " << fd_ctx->fd << ", " << (EPOLL_EVENTS)event.events << "):"
                                          << rt2 << " (" << errno << ") (" << strerror(errno) << ")";
                continue;
            }
 
            // 处理已经发生的事件，也就是让调度器调度指定的函数或协程
            if (real_events & READ) {
                fd_ctx->triggerEvent(READ);
                --m_pendingEventCount;
            }
            if (real_events & WRITE) {
                fd_ctx->triggerEvent(WRITE);
                --m_pendingEventCount;
            }
        } // end for

        /**
         * 一旦处理完所有的事件，idle协程yield，这样可以让调度协程(Scheduler::run)重新检查是否有新任务要调度
         * 上面triggerEvent实际也只是把对应的fiber重新加入调度，要执行的话还要等idle协程退出
         */
        Fiber::ptr cur = Fiber::getThis();
        auto raw_ptr   = cur.get();
        cur.reset();
        raw_ptr->yield();
    }
}

void IOManager::contextResize(size_t size)
{
    m_FdContexts.resize(size);

    for (size_t i = 0 ; i < size ; ++i) {
        if (!m_FdContexts[i]) {
            m_FdContexts[i] = new FdContext();
            m_FdContexts[i]->fd = i;
        }
    }
}

}