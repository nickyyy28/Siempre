#include "Thread/Thread.h"

namespace siem{

static thread_local Thread* local_thread = nullptr;
static thread_local std::string local_name = "";

Thread* Thread::getThis(void)
{
    return local_thread;
}

const std::string& Thread::getThisName(void)
{
    return local_name;
}

void Thread::setThisName(const std::string& name)
{
    if (local_thread) {
        local_name = name;
    }
}

void* Thread::run(void* arg)
{
    Thread* thread = (Thread *)(arg);
    local_thread = thread;
    local_name = thread->getName();

    thread->m_id = getThreadID();

    pthread_setname_np(pthread_self(), thread->getName().substr(0, 15).c_str());

    Thread::callBack cb;
    cb.swap(thread->m_cb);

    thread->m_semaphore.notify();

    cb();

    return 0;
}

Thread::Thread(callBack cb, const std::string& name)
    : m_cb(cb)
    , m_name(name) {

    if (m_name.empty()) {
        m_name = "UNKOWN";
    }

    int ret = pthread_create(&m_thread, nullptr, &Thread::run, this);

    if (ret) {
        ERROR() << "pthread create thread fail";
    }

    m_semaphore.wait();
};

Thread::~Thread()
{
    if (m_thread) {
        pthread_detach(m_thread);
    }
}

const std::string& Thread::getName(void) const
{
    return m_name;
}

pid_t Thread::getID(void) const
{
    return m_id;
}

void Thread::join()
{
    int ret = pthread_join(m_thread, nullptr);
    if (ret) {
        ERROR() << "pthread_join failed";
    }

    m_thread = 0;
}

void Thread::detach()
{
    int ret = pthread_detach(m_thread);
    if (ret) {
        ERROR() << "pthread_detach failed";
    }

    m_thread = 0;
}

}