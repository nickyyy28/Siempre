#pragma once

#include <functional>
#include <memory>

#include "../common/nocopyable.h"
#include "../base/TimeStamp.h"

namespace siem {

namespace net {

class EventLoop;

class Channel : public NoCopyAble{
public:
    using EventCallback = std::function<void()>;
    using ReadEventCallback = std::function<void()>;
    using WriteEventCallback = std::function<void()>;

    explicit Channel(EventLoop* eventLoop, int fd);
    ~Channel();

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
    void tie(const std::shared_ptr<void> tie_);

    int fd() const { return m_fd; }
    int events() const { return m_events; }
    int set_rtevents(int rtevents) { m_rtevents = rtevents; }
    bool isNoneEvent(int event) { return event == kNoneEvent; }

    void enableReading();
    void disableReading();
    void update();

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