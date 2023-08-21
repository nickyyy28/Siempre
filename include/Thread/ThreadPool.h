#pragma once

#include "../Logger/Logger.h"
#include "./Thread.h"
#include "./Mutex.h"
#include "../common/macro.h"
#include "../build_config.h"

#include <cstdint>
#include <memory>
#include <mutex>
#include <sys/types.h>
#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <condition_variable>
#include <atomic>

namespace siem{

enum class ThreadPool_Mode{
    FIXED = 0,
    CACHED = 1
};

// struct Task{
//     typedef std::function<void(void*)> Func;
//     Func task;
//     void* args;
// };

#undef USE_STD_CONDITION

#ifdef USE_STD_CONDITION
    typedef std::condition_variable _condition;
    typedef std::unique_lock<std::mutex> _scopeLock;
    typedef std::mutex _mutex;
    typedef std::thread _thread;
    typedef std::shared_ptr<std::thread> _threadPtr;
#else
    typedef siem::Condition _condition;
    typedef siem::Mutex::Lock _scopeLock;
    typedef siem::Mutex _mutex;
    typedef siem::Thread _thread;
    typedef siem::Thread::ptr _threadPtr;
#endif


class ThreadPool : public NoCopyAble{
public:
    typedef std::function<void()> Task;

    enum ThreadState {
        DEAD = 0x00,    //未启动线程
        ACTIVE = 0x01,  //工作线程
        IDLE = 0x02     //空闲线程
    };
    

    class ThreadWrap{
    public:
        ThreadWrap();
        ThreadWrap(Thread::ptr th);
        ~ThreadWrap();

        /**
         * @brief 销毁线程
         * 
         */
        void destroy();

        /**
         * @brief 激活线程
         * 
         */
        void activate();
    private:
        Thread::ptr th;
        ThreadState m_state;
        Semaphore m_sem;        //信号量
    };

    /**
     * @brief Cached模式构造
     * 
     * @param min_thread_count 最大工作线程
     * @param max_thread_count 最小工作线程
     */
    ThreadPool(int min_thread_count, int max_thread_count);

    /**
     * @brief Fixed模式工作线程
     * 
     * @param thread_count 
     */
    ThreadPool(int thread_count);
    ~ThreadPool();

    /**
     * @brief 向任务队列添加任务
     * 
     * @param task  任务
     * @param args  任务参数
     */
    void addTask(Task task);

    /**
     * @brief 设置线程池工作模式
     * 
     * @param mode 
     */
    void setWorkMode(ThreadPool_Mode mode);

    /**
     * @brief 设置最大任务队列数量
     * 
     * @param threshold 
     */
    void setTaskThreshold(uint32_t threshold);

    /**
     * @brief 开启线程池
     * 
     */
    void start();
private:

    /**
     * @brief 调度
     * 
     */
    void schedule();


private:
    ThreadPool() = delete;
    ThreadPool(const ThreadPool&) = delete;
    void operator=(const ThreadPool&) = delete;

    std::queue<Task> tasks;                 //任务队列
    _mutex m_mutex;                          //保护任务队列的互斥锁
    uint32_t m_taskThreshold;               //任务队列最大数量
    std::atomic_uint m_taskSize;            //当前队列大小

    _threadPtr manager;                    //管理者线程
    std::vector<_threadPtr> ths;     //工作线程列表
    int m_max_thread_count;                 //最大线程数量
    int m_min_thread_count;                 //最小线程数量
    uint32_t out_times;                     //线程空闲超时时间, 超时销毁线程

    ThreadPool_Mode m_workMode = ThreadPool_Mode::FIXED;    //工作模式

    _condition m_notFull;                    //表示任务队列不满
    _condition m_notEmpty;                   //表示任务队列不空

    _condition m_cond;
};

}
