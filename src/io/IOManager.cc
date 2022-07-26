#include "io/IOManager.h"

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
    case READ:
        return read;
    case WRITE:
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
    , m_epfd(epoll_create(1024)) {
    
    SIEM_ASSERT(m_epfd > 0);

    int ret = pipe(m_tickle_fds);

    epoll_event event;
    memset(&event, 0, sizeof(event));

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickle_fds[READ_TICKLE_PIPE];

    ret = fcntl(m_tickle_fds[READ_TICKLE_PIPE], F_SEAL_SEAL, O_NONBLOCK);

    SIEM_ASSERT(ret);

    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickle_fds[READ_TICKLE_PIPE], &event);

    SIEM_ASSERT(ret);

    contextResize(64);

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
        contextResize(fd * 2);
        fd_ctx = m_FdContexts[fd];
    }

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);
    if (fd_ctx->m_events & event) {
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "add Event failed";
        SIEM_ASSERT(!(fd_ctx->m_events & event));
    }

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

    ++m_penddingEventCount;
    fd_ctx->m_events = (Event)(fd_ctx->m_events | event);
    FdContext::EventContext& event_ctx = fd_ctx->getContext(event);

    SIEM_ASSERT(!event_ctx.sc && !event_ctx.fiber && !event_ctx.cb);

    event_ctx.sc = Scheduler::getThis();

    if (cb) {
        event_ctx.cb.swap(cb);
    } else {
        event_ctx.fiber = Fiber::getThis();
        SIEM_ASSERT(event_ctx.fiber->getState() == Fiber::EXEC);
    }

    return 0;

}

bool IOManager::delEvent(int fd, Event event)
{
    RWMutexType::ReadLock rlock(m_mutex);

    if (m_FdContexts.size() <= fd) {
        return false;
    }

    FdContext* fd_ctx = m_FdContexts[fd];
    rlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);

    if (SIEM_UNLIKELY(!(fd_ctx->m_events & event))) {
        return false;
    }

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

    --m_penddingEventCount;
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

    FdContext* fd_ctx = m_FdContexts[fd];
    rlock.unlock();

    FdContext::MutexType::Lock lock(fd_ctx->m_mutex);

    if (SIEM_UNLIKELY(!(fd_ctx->m_events & event))) {
        return false;
    }

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

    --m_penddingEventCount;
    fd_ctx->triggerEvent(event);
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

    if(fd_ctx->m_events & READ) {
        fd_ctx->triggerEvent(READ);
        --m_penddingEventCount;
    }
    if(fd_ctx->m_events & WRITE) {
        fd_ctx->triggerEvent(WRITE);
        --m_penddingEventCount;
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
    uint64_t timeout = 0;
    return stopping(timeout);
}

bool IOManager::stopping(uint64_t timeout)
{
    return false;
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
    LOG_DEBUG(GET_LOG_BY_NAME(system)) << "io manager idle";
    const uint64_t MAX_EVNETS = 256;
    epoll_event* events = new epoll_event[MAX_EVNETS]();
    std::shared_ptr<epoll_event> shared_events(events, [](epoll_event* ptr){
        delete[] ptr;
    });

    while(true) {
        uint64_t next_timeout = 0;
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
        // listExpiredCb(cbs);
        if(!cbs.empty()) {
            //SYLAR_LOG_DEBUG(g_logger) << "on timer cbs.size=" << cbs.size();
            schedule(cbs.begin(), cbs.end());
            cbs.clear();
        }

        //if(SYLAR_UNLIKELY(rt == MAX_EVNETS)) {
        //    SYLAR_LOG_INFO(g_logger) << "epoll wait events=" << rt;
        //}

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

            //SYLAR_LOG_INFO(g_logger) << " fd=" << fd_ctx->fd << " events=" << fd_ctx->events
            //                         << " real_events=" << real_events;
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

        raw_ptr->swapOut();
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