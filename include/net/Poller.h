#pragma once

#include "base/TimeStamp.h"
#include "common/nocopyable.h"
#include "net/Channel.h"
#include "net/EventLoop.h"
#include <map>
#include <unordered_map>
#include <vector>
namespace siem {

namespace net {

class EventLoop;

//the io multiplex core module
class Poller : NoCopyAble{
public:
    using ChannelList = std::vector<Channel*>;

    explicit Poller(EventLoop* loop);
    virtual ~Poller();

    /**
     * @brief the unified api of io multiplex
     * 
     * @param timeout 
     * @param activateChannels 
     * @return TimeStamp 
     */
    virtual TimeStamp poll(int timeout, ChannelList* activateChannels) = 0;

    virtual void updateChannel(Channel* channel) = 0;
    virtual void removeChannel(Channel* channel) = 0;

    /**
     * @brief determine the channel is existed in the poller
     * 
     * @param channel 
     * @return true 
     * @return false 
     */
    bool hasChannel(Channel* channel) const;

    /**
     * @brief Eventloop can get default poller instance
     * 
     * @param loop 
     * @return Poller* 
     */
    static Poller* newDefaultPoller(EventLoop* loop);
protected:
    using ChannelMap = std::unordered_map<int, Channel*>;
    ChannelMap m_channels;

private:
    EventLoop* m_loop;
};

}

}