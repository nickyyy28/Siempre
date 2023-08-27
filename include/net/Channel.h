#pragma once

#include <functional>
#include <memory>

#include "../common/nocopyable.h"
#include "../base/TimeStamp.h"

namespace siem {

namespace net {

class EventLoop;

/**
 * @brief the Channel class encapsulates the socket fd and its interested event, like EPOLLIN,EPOLLOUT
          and bind the specific event by poller returned
 * 
 */
class Channel : public NoCopyAble{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void(TimeStamp recvTime)>;
    using WriteEventCallback = std::function<void()>;

    explicit Channel(EventLoop* eventLoop, int fd);
    ~Channel();

    /**
     * @brief when the fd get poller's events, to handle events
     * 
     * @param recvTime 
     */
    void handleEvent(TimeStamp recvTime);

    void setReadEventCallback(const ReadEventCallback& callback);
    void setWriteEventCallback(const WriteEventCallback& callback);
    void setCloseEventCallback(const EventCallback& callback);
    void setErrorEventCallback(const EventCallback& callback);

    /**
     * @brief prevent the channel be manually removed , the channel still run the event callbacks
     * 
     * @param tie_ 
     */
    void tie(const std::shared_ptr<void>& tie_);

    int fd() const { return m_fd; }
    int events() const { return m_events; }
    void set_rtevents(int rtevents) { m_rtevents = rtevents; }
    bool isNoneEvent() { return m_events == kNoneEvent; }
    bool isReadingEvent() { return m_events &= kReadEvent; }
    bool isWriteEvent() { return m_events &= kWriteEvent; }

    void enableReading();
    void disableReading();
    void enableWriting();
    void disableWriting();
    void disableAll();

    /**
     * @brief remove this channel in owner eventloop
     * 
     */
    void remove();

    int index() const { return m_index; }
    void setIndex(int index) { m_index = index; }

    EventLoop* ownerLoop() const { return m_loop; }

private:
    /**
     * @brief when change events ,update the fd's events in poller
     * 
     */
    void update();
    void handleEventWithGuard(TimeStamp recvTime);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    int m_fd;               //the fd of poller listened
    EventLoop* m_loop;      //event loop

    int m_events;           //register the fd interested event
    int m_rtevents;         //the poller returned event
    int m_index;

    std::weak_ptr<void> m_tie;
    bool m_tied;

    ReadEventCallback m_read_cb;
    WriteEventCallback m_write_cb;
    EventCallback m_close_cb;
    EventCallback m_error_cb;

};


}

}