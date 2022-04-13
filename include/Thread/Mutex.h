#ifndef __MUTEX_H
#define __MUTEX_H

#include <semaphore.h>

#include "common/nocopyable.h"
#include "common/singleton.h"

namespace siem{

/**
 * @brief 信号量
 * 
 */
class Semaphore : public NoCopyAble{
public:
    Semaphore(size_t count = 0);
    ~Semaphore();

    /**
     * @brief 等待
     * 
     */
    void wait(void);
    bool trywait(void);

    /**
     * @brief 通知
     * 
     */
    void notify(void);

private:
    sem_t m_semaphore;
};

template<class T>
struct ScopeLockImpl
{
public:
    ScopeLockImpl(T& mutex)
    : m_mutex(mutex) {
        lock();
        isLocked = true;
    }
    ~ScopeLockImpl() {
        unlock();
    }

    void lock()
    {
        if (!isLocked) {
            m_mutex.lock();
            isLocked = true;
        }
    }

    void unlock()
    {
        if (isLocked) {
            m_mutex.unlock();
            isLocked = false;
        }
    }

private:
    T& m_mutex;
    bool isLocked;
};


}

#endif //__MUTEX_H