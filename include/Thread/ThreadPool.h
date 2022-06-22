#ifndef __SIEM_THREAD_POOL_H
#define __SIEM_THREAD_POOL_H

#include "Logger/Logger.h"
#include "Thread.h"
#include "Thread/Mutex.h"
#include "common/macro.h"

#include <vector>
#include <queue>
#include <functional>

namespace siem{

struct Task{
    typedef std::function<void(void*)> Func;
    Func task;
    void* args;
};

class ThreadPool : public NoCopyAble{
public:
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
     * @brief Construct a new Thread Pool object
     * 
     * @param min_thread_count 最大工作线程
     * @param max_thread_count 最小工作线程
     */
    ThreadPool(int min_thread_count = 1, int max_thread_count = 10);
    ~ThreadPool();

    /**
     * @brief 向任务队列添加任务
     * 
     * @param task  任务
     * @param args  任务参数
     */
    void addTask(Task::Func task, void* args);
private:
    std::queue<Task> tasks;         //任务队列
    Mutex m_mutex;                  //保护任务队列的互斥锁

    Thread::ptr manager;            //管理者线程
    std::vector<ThreadWrap> ths;    //工作线程
    int m_max_thread_count;         //最大线程数量
    int m_min_thread_count;         //最小线程数量
    uint32_t out_times;             //线程空闲超时时间, 超时销毁线程
};

}

#endif
