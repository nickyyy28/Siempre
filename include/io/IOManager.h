#ifndef __SIEM_IO_MANAGER_H
#define __SIEM_IO_MANAGER_H

#include <sys/epoll.h>

#include "Logger/Logger.h"
#include "Fiber/Scheduler.h"

namespace siem{

class IOManager : public Scheduler{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event {
        None = 0x0,
        READ,
        WRITE
    };

    IOManager(int thread = 1, bool use_caller = true, const std::string& name = "");
    ~IOManager();

    /**
     * @brief 添加事件
     *
     * @param fd    事件关联句柄
     * @param event 事件
     * @param cb    回调
     * @return int  状态
     */
    int addEvent(int fd, Event event, std::function<void()> cb = nullptr);
    
    /**
     * @brief 删除事件
     * 
     * @param fd        事件关联的句柄
     * @param event     事件
     * @return true     删除成功
     * @return false    删除失败
     */
    bool delEvent(int fd, Event event);

    /**
     * @brief 取消事件
     * 
     * @param fd        事件关联句柄
     * @param event     事件
     * @return true     取消成功
     * @return false    取消失败
     */
    bool cancelEvent(int fd, Event event);

    /**
     * @brief 取消该句柄的所有事件
     * 
     * @param fd        事件句柄
     * @return true     取消成功
     * @return false    取消失败
     */
    bool cancelAll(int fd);

    /**
     * @brief 获取当前IOManager
     * 
     * @return IOManager* 
     */
    static IOManager* getThis();

protected:
    bool stopping(void) override;
    bool stopping(uint64_t timeout);
    void tickle(void) override;
    void idle(void) override;
    // void onTimerInsertedAtFront(void) override;

    void contextResize(size_t size);
private:
    struct FdContext{
        typedef Mutex MutexType;
        struct EventContext{
            Scheduler* sc = nullptr;    // 事件执行的调度器
            Fiber::ptr fiber;           // 事件执行的协程
            std::function<void()> cb;   // 事件执行的回调
        };

        EventContext& getContext(Event e);
        void resetContext(EventContext& e);
        void triggerEvent(Event e);

        int fd;                     // 事件关联的句柄
        struct EventContext read;          // 读事件
        struct EventContext write;         // 写事件
        Event m_events = None;       // 已注册的事件
        MutexType m_mutex;          // 互斥量
    };

private:
    std::string m_name;
    int m_epfd = 0;                                 //
    int m_tickle_fds[2];                            //
    std::atomic<size_t> m_penddingEventCount = {0}; // 当前等待执行的事件数量
    RWMutexType m_mutex;                            // 读写锁
    std::vector<FdContext*> m_FdContexts;           // 事件数组
};

}

#endif //__SIEM_IO_MANAGER_H