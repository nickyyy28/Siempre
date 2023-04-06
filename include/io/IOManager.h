#pragma once

#include <sys/epoll.h>

#include "../Logger/Logger.h"
#include "../Fiber/Scheduler.h"

namespace siem{

class IOManager : public Scheduler{
public:
    typedef std::shared_ptr<IOManager> ptr;
    typedef RWMutex RWMutexType;

    enum Event {
        None = 0x0,     //无事件
        READ = 0x1,     //读事件
        WRITE = 0x4     //写事件
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

    //是否正在退出调度
    bool stopping(void) override;

    /**
    * @brief 通知调度器有任务要调度
    * @details 写pipe让idle协程从epoll_wait退出，待idle协程yield之后Scheduler::run就可以调度其他任务
    * 如果当前没有空闲调度线程，那就没必要发通知
    */
    void tickle(void) override;

    /**
    * @brief idle协程
    * @details 对于IO协程调度来说，应阻塞在等待IO事件上，idle退出的时机是epoll_wait返回，对应的操作是tickle或注册的IO事件就绪
    * 调度器无调度任务时会阻塞idle协程上，对IO调度器而言，idle状态应该关注两件事，一是有没有新的调度任务，对应Schduler::schedule()，
    * 如果有新的调度任务，那应该立即退出idle状态，并执行对应的任务；二是关注当前注册的所有IO事件有没有触发，如果有触发，那么应该执行
    * IO事件对应的回调函数
    */
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
        Event m_events = Event::None;       // 已注册的事件
        MutexType m_mutex;          // 互斥量
    };

private:
    std::string m_name;
    int m_epfd = 0;                                 //
    int m_tickle_fds[2];                            //
    std::atomic<size_t> m_pendingEventCount = {0}; // 当前等待执行的事件数量
    RWMutexType m_mutex;                            // 读写锁
    std::vector<FdContext*> m_FdContexts;           // 事件数组
};

}