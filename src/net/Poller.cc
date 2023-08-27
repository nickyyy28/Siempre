#include "net/Poller.h"
#include "net/EventLoop.h"

namespace siem {

namespace net {

Poller::Poller(EventLoop* loop)
    : m_loop(loop)
{

}

Poller::~Poller()
{

}

bool Poller::hasChannel(Channel *channel) const
{
    auto it = m_channels.find(channel->fd());
    return it != m_channels.end() && it->second == channel;
}

}

}