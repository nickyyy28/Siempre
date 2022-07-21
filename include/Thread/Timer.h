#pragma once

#include "Thread/Thread.h"
#include <cstdint>
#include <functional>
#include <memory>
#include <signal.h>
#include <sys/socket.h>
#include <sys/epoll.h>

// 默认Socket定时器监听端口
#define DEFAULT_TIMER_LISTEN_PORT   9547

namespace siem {

typedef std::function<void()> TimerCallBack;

class BaseTimer{
public:

    BaseTimer(int timeout, TimerCallBack cb);
    ~BaseTimer() = default;

    /**
     * @brief 设置回调
     * 
     * @param cb 
     */
    void setCallBack(TimerCallBack cb);

    /**
     * @brief 设置定时时间
     * 
     * @param timeout 
     */
    void setTimeout(int timeout);

    /**
     * @brief 开启定时器
     * 
     */
    virtual void start() = 0;

    /**
     * @brief 开启单次定时
     * 
     */
    virtual void startOnce() = 0;

protected:
    int m_timeout;
    TimerCallBack m_callback;
};

class SocketTimerListener{
public:
    typedef std::shared_ptr<SocketTimerListener> ptr;

    static SocketTimerListener::ptr getInstance();

    ~SocketTimerListener();
private:
    Thread::ptr m_listener;
    struct sockaddr_in m_addr;
    int m_epfd;
    int m_fd;
    struct epoll_event m_ev;

    SocketTimerListener();

    SocketTimerListener(const SocketTimerListener&) = delete;
    void operator= (const SocketTimerListener&) = delete;
};

class SocketTimer : public BaseTimer{
public:
    SocketTimer(int timeout, TimerCallBack cb);
    SocketTimer();
    ~SocketTimer();

    /**
     * @brief 开启定时器
     * 
     */
    void start() override;

    /**
     * @brief 定时一次
     * 
     */
    void startOnce() override;

private:
    
    bool isSingle;
    Thread::ptr m_thread;
    int m_fd;

    static int timer_cnt;
};

struct _timer{

};

/**
 * @brief 时间轮
 * 
 */
class TimeWheel{
public:
private:
    uint32_t m_mills;
    uint32_t m_seconds;
    uint32_t m_minutes;
};

}