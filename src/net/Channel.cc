#include "net/Channel.h"
#include "common/macro.h"

#include <memory>
#include <sys/epoll.h>

namespace siem {

namespace net {

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* eventLoop, int fd)
    : m_loop(eventLoop)
    , m_fd(fd)
    , m_events(0)
    , m_index(-1)
    , m_rtevents(0)
    , m_tied(false)
{

}

Channel::~Channel()
{

}

void Channel::setReadEventCallback(const ReadEventCallback &callback)
{
    this->m_read_cb = callback;
}

void Channel::setWriteEventCallback(const WriteEventCallback &callback)
{
    this->m_write_cb = callback;
}

void Channel::setCloseEventCallback(const EventCallback &callback)
{
    this->m_close_cb = callback;
}

void Channel::setErrorEventCallback(const EventCallback &callback)
{
    this->m_error_cb = callback;
}

void Channel::handleEvent(TimeStamp recvTime)
{
    std::shared_ptr<void> guard;
    if (m_tied) {
        guard = m_tie.lock();
        if (guard) {
            handleEventWithGuard(recvTime);
        }
    } else {
        handleEventWithGuard(recvTime);
    }

}

void Channel::handleEventWithGuard(TimeStamp recvTime)
{
    if ((m_rtevents & EPOLLHUP) && !(m_rtevents & EPOLLIN)) {
        if (m_close_cb) {
            m_close_cb();
        }
    }

    if (m_rtevents & EPOLLERR) {
        if (m_error_cb) {
            m_error_cb();
        }
    }

    if (m_rtevents & (EPOLLIN | EPOLLPRI)) {
        if (m_read_cb) {
            m_read_cb(recvTime);
        }
    }

    if (m_rtevents & EPOLLOUT) {
        if (m_write_cb) {
            m_write_cb();
        }
    }
}

void Channel::tie(const std::shared_ptr<void> &tie_)
{
    m_tie = tie_;
    m_tied = true;
}

void Channel::enableReading()
{
    m_events |= kReadEvent;
    update();
}

void Channel::disableReading()
{
    m_events &= ~kReadEvent;
    update();
}

void Channel::enableWriting()
{
    m_events |= kWriteEvent;
    update();
}

void Channel::disableWriting()
{
    m_events &= ~kWriteEvent;
    update();
}

void Channel::disableAll()
{
    m_events &= kNoneEvent;
    update();
}

void Channel::update()
{
    //TODO add code
    //m_loop->updateChannel(this);
}

void Channel::remove()
{
    //TODO add code
    //m_loop->removeChannel(this);
}

}

}