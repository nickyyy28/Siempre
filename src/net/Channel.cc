#include "net/Channel.h"

namespace siem {

namespace net {

Channel::Channel(EventLoop* eventLoop, int fd)
    : m_loop(eventLoop)
    , m_fd(fd)
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

void Channel::update()
{
    
}

}

}