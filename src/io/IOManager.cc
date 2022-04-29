#include "io/IOManager.h"

#include <unistd.h>
#include <fcntl.h>

#define READ_TICKLE_PIPE   0   //读管道句柄编号
#define WRITE_TICKLE_PIPE  1   //写管道句柄编号

namespace siem{

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
        SIEM_ASSERT_STR(false, get context);
    }

    throw std::invalid_argument("getContext");
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

    m_FdContexts.resize(64);

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
        SIEM_ASSERT(false);
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



}

bool IOManager::delEvent(int fd, Event event)
{

}

bool IOManager::cancelEvent(int fd, Event event)
{

}

bool IOManager::cancelAll(int fd)
{

}

IOManager* IOManager::getThis()
{

}

bool IOManager::stopping(void)
{

}
void IOManager::tickle(void)
{

}

void IOManager::idle(void)
{

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