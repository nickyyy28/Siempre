#include "Thread/ThreadPool.h"

namespace siem{

ThreadPool::ThreadPool(int min_thread_count, int max_thread_count) 
        : m_min_thread_count(min_thread_count)
        , m_max_thread_count(max_thread_count) {
    //this->ths.resize(max_thread_count);
    for(int i = 0 ; i < max_thread_count ; ++i) {
        // ThreadWrap wrap(Thread::ptr(new Thread([&](){
            
        // }, "thread_pool_" + std::to_string(i))));
        // ths.push_back(wrap);
    }

    this->manager.reset(new Thread([&](){
       //管理者线程 
    }));
}

ThreadPool::~ThreadPool()
{

}

ThreadPool::ThreadWrap::ThreadWrap()
{
    this->th = nullptr;
    this->m_state = ThreadState::DEAD;
}

ThreadPool::ThreadWrap::ThreadWrap(Thread::ptr th)
{
    if (th) {
        this->th = th;
        this->m_state = ThreadState::IDLE;
    } else {
        this->th = nullptr;
        this->m_state = ThreadState::DEAD;
    }

}

ThreadPool::ThreadWrap::~ThreadWrap()
{
    if (m_state == ThreadState::IDLE || m_state == ThreadState::ACTIVE) {
        destroy();
    }
}

void ThreadPool::ThreadWrap::destroy()
{
    if (m_state != ThreadState::DEAD) {
        m_state = ThreadState::DEAD;
       SIEM_ASSERT_STR(!this->th, ERROR ) 
    }
}

void ThreadPool::ThreadWrap::activate()
{

}

}

