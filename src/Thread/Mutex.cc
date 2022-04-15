#include "Thread/Mutex.h"
#include <cerrno>

namespace siem{

Semaphore::Semaphore(size_t count)
{
    if (sem_init(&m_semaphore, 0, count)) {
        throw std::logic_error("semaphore init error");
    }
}

Semaphore::~Semaphore()
{
    sem_destroy(&m_semaphore);
}

void Semaphore::wait(void)
{
    if (sem_wait(&m_semaphore)) {
        throw std::logic_error("sem_wait error");
    }
}

bool Semaphore::trywait(void)
{
    if (sem_trywait(&m_semaphore)) {
        return false;
    }
    return true;
}

void Semaphore::notify(void)
{
    if (sem_post(&m_semaphore)) {
        throw std::logic_error("sem_post error");
    }
}

Mutex::Mutex()
{
    pthread_mutex_init(&m_mutex, nullptr);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_mutex);
}

void Mutex::lock(void)
{
    pthread_mutex_lock(&m_mutex);
}

void Mutex::unlock(void)
{
    pthread_mutex_unlock(&m_mutex);
}

RWMutex::RWMutex()
{
    pthread_rwlock_init(&m_lock, nullptr);
}

RWMutex::~RWMutex()
{
    pthread_rwlock_destroy(&m_lock);
}

void RWMutex::rdlock(void)
{
    pthread_rwlock_rdlock(&m_lock);
}

void RWMutex::wrlock(void)
{
    pthread_rwlock_wrlock(&m_lock);
}

void RWMutex::unlock(void)
{
    pthread_rwlock_unlock(&m_lock);
}

}