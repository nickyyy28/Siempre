#pragma once

#include "./Fiber.h"
#include "../Thread/Thread.h"
#include "../common/nocopyable.h"

#include <vector>
#include <memory>

namespace siem{

/**
 * @brief 调度器, 在N线程:M协程之间进行任务调度
 * 
 */
class Scheduler : public NoCopyAble{
public:
    using ptr = std::shared_ptr<Scheduler>;

    /**
     * @brief 创建一个调度器
     * 
     * @details 通过isCaller来确定是否启用创建该调度器的线程作为工作线程之一
     * @param thread 
     * @param use_caller 
     * @param name 
     */
    Scheduler(size_t thread = 1, bool use_caller = true, const std::string& name = "");
    virtual ~Scheduler() = default;

    /**
     * @brief 获取当前调度器
     * 
     * @return Scheduler* 
     */
    static Scheduler* getThis(void);

    /**
     * @brief 获取主协程
     * 
     * @return Fiber* 
     */
    static Fiber* getMainFiber(void);

    /**
     * @brief 设置当前调度器
     * 
     */
    void setThis(void);

    /**
     * @brief 取出任务运行的入口函数
     * 
     */
    void run(void);

    /**
     * @brief 开启调度
     * 
     */
    void start(void);

    /**
     * @brief 停止调度
     * 
     */
    void stop(void);

    /**
     * @brief 是否有空闲线程
     */
    bool hasIdleThreads() { return m_idleThreadCount > 0; }

    /**
     * @brief 获取调度器名字
     * 
     * @return std::string 
     */
    std::string getName() const;

    /**
     * @brief 批量添加调度
     * 
     * @tparam InputIterator 迭代器
     * @param start 
     * @param end 
     */
    template<class InputIterator>
    void schedule(InputIterator begin, InputIterator end)
    {
        bool need_tickle = false;
        {
            Mutex::Lock lock(m_mutex);
            while(begin != end) {
                need_tickle = scheduleNoLock(&*begin) || need_tickle;
            }
        }

        if (need_tickle) {
            tickle();
        }
    }

    /**
     * @brief 添加调度
     * 
     * @tparam Task    协程或函数指针类
     * @param fb  协程或函数
     * @param thr 线程ID
     */
    template<class Task>
    void schedule(Task fb, uint64_t thr = -1)
    {
        bool need_tickle = false;
        {
            Mutex::Lock lock(m_mutex);
            need_tickle = scheduleNoLock(fb, thr);
        }

        if (need_tickle) {
            tickle();
        }

    }

    void switchTo(int thread = -1);
    std::ostream& dump(std::ostream& os);

private:

    /**
     * @brief 无锁添加调度
     * 
     * @tparam Task 协程或函数指针类
     * @param fb    协程或函数指针
     * @param thr   线程ID
     * @return true 
     * @return false 
     */
    template<class Task>
    bool scheduleNoLock(Task fb, uint64_t thr = -1)
    {
        bool need_tickle = m_fibers.empty();
        SchedulerTask task(fb, thr);
        if (task.m_cb || task.m_fiber) {
            m_fibers.push_back(task);
        }

        return need_tickle;
    }

private:

    /**
     * @brief 协程或函数指针包装类
     * 
     */
    struct SchedulerTask{
        Fiber::ptr m_fiber = nullptr;
        typename Fiber::callBack m_cb = nullptr;
        uint64_t thread_id = 0;

        SchedulerTask(Fiber::ptr fiber, uint64_t thr)
            : m_fiber(fiber)
            , thread_id(thr) {
        }

        SchedulerTask(Fiber::ptr* fiber, uint64_t thr)
            : thread_id(thr) {
            m_fiber.swap(*fiber);
        }

        SchedulerTask(Fiber::callBack cb, uint64_t thr)
            : thread_id(thr)
            , m_cb(cb) {
        }

        SchedulerTask(Fiber::callBack* cb, uint64_t thr)
            : thread_id(thr) {
            m_cb.swap(*cb);
        }

        SchedulerTask()
            : thread_id(-1) {
        }

        void reset()
        {
            thread_id = -1;
            m_cb = nullptr;
            m_fiber = nullptr;
        }
    };

protected:
    virtual void tickle(void);
    virtual bool stopping(void);

    /**
     * @brief 协程无任务可调度时执行idle协程
     */
    virtual void idle();
private:
    //// 互斥锁
    siem::Mutex m_mutex;
    //// 调度器名称
    std::string m_name;
    //// 线程池
    std::vector<siem::Thread::ptr> m_threads;
    //// 协程池
    std::list<SchedulerTask> m_fibers;
    //// use_caller为true时有效, 调度协程
    Fiber::ptr m_rootFiber;

    //是否使用当前线程作为调度线程
    bool m_isusecaller;

protected:
    /// 协程下的线程id数组
    std::vector<int> m_threadIds;
    /// 线程数量
    size_t m_threadCount = 0;
    /// 工作线程数量
    std::atomic<size_t> m_activeThreadCount = {0};
    /// 空闲线程数量
    std::atomic<size_t> m_idleThreadCount = {0};
    /// 是否正在停止
    bool m_stopping = true;
    /// 是否自动停止
    bool m_autoStop = false;
    /// 主线程id(use_caller)
    int m_rootThreadID = 0;

};

}