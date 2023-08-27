#include "net/EpollPoller.h"
#include "Logger/Logger.h"
#include "base/TimeStamp.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Poller.h"
#include <cstdlib>
#include <sys/epoll.h>
#include <cerrno>

namespace siem {
    
namespace net {

static const int kNew = -1;     //channel not add in channel list
static const int kAdded = 1;    //channel added in channel list
static const int kDeleted = 2;  //channel deleted in channel list

EpollPoller::EpollPoller(EventLoop* loop)
    : Poller(loop)
{
    m_epoll_fd = epoll_create(1);
    if (m_epoll_fd < 0) {
        ERROR() << __FILE__ << ":" << __LINE__ << "epoll create failed:" << errno;
        std::exit(-1);
    }
    m_events = EventList(kInitEventListSize);
}

EpollPoller::~EpollPoller()
{
    close(m_epoll_fd);
}

TimeStamp EpollPoller::poll(int timeout, ChannelList *activeChannels)
{
    return TimeStamp::now();
}

void EpollPoller::updateChannel(Channel *channel)
{
    auto index = channel->index();
    if (index == kNew || index == kDeleted) {
        auto fd = channel->fd();
        if (index == kNew) {
            m_channels[fd] = channel;
        }
        if (index == kDeleted) {
            auto it = m_channels.find(fd);
            if (it != m_channels.end()) {
                ERROR() << "event deleted but still exits in channel map";
            }
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        (void)fd;

        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EpollPoller::removeChannel(Channel *channel)
{
    auto idnex = channel->index();
    // if (index == kNew)
}

void EpollPoller::update(int operation, Channel* channel)
{

}

void EpollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{

}

}

}