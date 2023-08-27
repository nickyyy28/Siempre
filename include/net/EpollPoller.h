#pragma once

#include "../common/nocopyable.h"
#include "base/TimeStamp.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include "net/Poller.h"

#include <sys/epoll.h>
#include <vector>

namespace siem {
    
namespace net {

class EpollPoller : public Poller{
public:
    explicit EpollPoller(EventLoop* loop);
    ~EpollPoller() override;

    TimeStamp poll(int timeout, ChannelList* activeChannels) override;
    void updateChannel(Channel* channel) override;
    void removeChannel(Channel* channel) override;

private:

    /**
     * @brief fill the active channels
     * 
     * @param numEvents 
     * @param activeChannels 
     */
    void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;
    void update(int operation, Channel* channel);

private:
    using EventList = std::vector<epoll_event>;

    static const int kInitEventListSize = 16;

    int m_epoll_fd;
    EventList m_events;
};

}

}