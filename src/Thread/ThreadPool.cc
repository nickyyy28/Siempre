#include "Thread/ThreadPool.h"
#include "Logger/Logger.h"
#include "Thread/Mutex.h"
#include <cstddef>
#include <string>
#include <unistd.h>

namespace siem{

ThreadPool::ThreadPool(int min_thread_count, int max_thread_count) 
        : m_min_thread_count(min_thread_count)
        , m_max_thread_count(max_thread_count) {
    
    setWorkMode(ThreadPool_Mode::CACHED);

    for (size_t i = 0 ; i < m_max_thread_count ; ++i) {
        ths.emplace_back(new _thread([&]{
            while(true) {
                _scopeLock lock(this->m_mutex);
                while (this->tasks.size() == 0) {
                    this->m_cond.wait(lock);
                }

                auto task = this->tasks.front();
                task();

                this->m_cond.notify_all();

                usleep(1000);
            }
        }));
    }

}

ThreadPool::ThreadPool(int thread_count)
{
    m_max_thread_count = thread_count;
    setWorkMode(ThreadPool_Mode::FIXED);

    for (size_t i = 0 ; i < m_max_thread_count ; ++i) {
        ths.push_back(siem::_threadPtr(new siem::_thread([&]{
            while(true) {
                Task task;
                {
                    _scopeLock lock(this->m_mutex);
#ifdef USE_STD_CONDITION
                    while(this->tasks.size() == 0) m_notEmpty.wait(lock);
#else
                    m_notEmpty.wait(lock, [this]()->bool { return this->tasks.size() == 0; });
#endif
                    
                    task = this->tasks.front();
                    this->tasks.pop();
                    m_taskSize--;
                    this->m_notFull.notify_all();
                }
                if (task) {
                    task();
                }
            }
        })));
    }
}

ThreadPool::~ThreadPool()
{
    for (size_t i = 0 ; i < ths.size() ; ++i) {
#ifdef USE_STD_CONDITION
#else
        ths[i]->destroy();
#endif
    }
}

void ThreadPool::setTaskThreshold(uint32_t threshold)
{
    m_taskThreshold = threshold;
}

void ThreadPool::addTask(Task task)
{
    _scopeLock lock(m_mutex);

    //提交任务时不能一直阻塞在这里
#ifdef USE_STD_CONDITION
    while (this->tasks.size() >= this->m_taskThreshold) m_notFull.wait(lock);
#else
    m_notFull.wait(lock, [this](void)->bool { return (this->tasks.size() >= this->m_taskThreshold); });
#endif
    

    tasks.push(task);
    this->m_taskSize++;

    m_notEmpty.notify_all();
}

void ThreadPool::start()
{
    for (size_t i = 0 ; i < ths.size() ; ++i) {
        ths[i]->detach();
    }
}

void ThreadPool::schedule()
{

}

void ThreadPool::setWorkMode(ThreadPool_Mode mode)
{
    m_workMode = mode;
}

}

