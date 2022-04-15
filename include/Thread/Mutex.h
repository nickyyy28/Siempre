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

/**
 * @brief 局部锁, 通过局部变量的构造和析构自动加锁自动解锁
 * 
 * @tparam T 
 */
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

    /**
     * @brief 加锁
     * 
     */
    void lock()
    {
        if (!isLocked) {
            m_mutex.lock();
            isLocked = true;
        }
    }

    /**
     * @brief 解锁
     * 
     */
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

/**
 * @brief 局部读锁
 * 
 * @tparam T 
 */
template<class T>
struct ReadScopeLockImpl
{
public:
    ReadScopeLockImpl(T& mutex)
    : m_mutex(mutex) {
        lock();
        isLocked = true;
    }
    ~ReadScopeLockImpl() {
        unlock();
    }

    void lock()
    {
        if (!isLocked) {
            m_mutex.rdlock();
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

/**
 * @brief 局部写锁
 * 
 * @tparam T 
 */
template<class T>
struct WriteScopeLockImpl
{
public:
    WriteScopeLockImpl(T& mutex)
    : m_mutex(mutex) {
        lock();
        isLocked = true;
    }
    ~WriteScopeLockImpl() {
        unlock();
    }

    void lock()
    {
        if (!isLocked) {
            m_mutex.wrlock();
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

/**
 * @brief 互斥量
 * 
 */
class Mutex : public NoCopyAble{
public:
    typedef ScopeLockImpl<Mutex> Lock;

    Mutex();
    ~Mutex();

    void lock(void);
    void unlock(void);
private:
    pthread_mutex_t m_mutex;
};

class RWMutex : public NoCopyAble{
public:
    typedef ReadScopeLockImpl<RWMutex> ReadLock;
    typedef WriteScopeLockImpl<RWMutex> WriteLock;

    RWMutex();
    ~RWMutex();

    void rdlock(void);
    void wrlock(void);
    void unlock(void);
private:
    pthread_rwlock_t m_lock;
};

}

#endif //__MUTEX_H