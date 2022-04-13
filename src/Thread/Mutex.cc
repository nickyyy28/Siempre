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

}