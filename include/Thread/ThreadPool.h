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
//     using Func = std::function<void(void*)>;
//     Func task;
//     void* args;
// };

#undef USE_STD_CONDITION

#ifdef USE_STD_CONDITION
    using _condition = std::condition_variable;
    using _scopeLock = std::unique_lock<std::mutex>;
    using _mutex = std::mutex;
    using _thread = std::thread;
    using _threadPtr = std::shared_ptr<std::thread>;
#else
    using _condition = siem::Condition;
    using _scopeLock = siem::Mutex::Lock;
    using _mutex = siem::Mutex;
    using _thread = siem::Thread;
    using _threadPtr = siem::Thread::ptr;
#endif


class ThreadPool : public NoCopyAble{
public:
    using Task = std::function<void()>;

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
