#pragma once

#include <cstdint>
#include <functional>
#include <semaphore.h>
#include <pthread.h>

#include "../common/nocopyable.h"
#include "../common/singleton.h"

namespace siem{

class Condition;

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
    friend class Condition;
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
    friend class Condition;
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

class Condition{
public:
    typedef std::function<bool()> CondIf;
    Condition();
    ~Condition() = default;

    /**
     * @brief 等待条件满足
     * 
     * @param lock 
     */
    void wait(Mutex::Lock& lock);

    /**
     * @brief 当条件成立时才等待条件满足
     * 
     * @param lock 
     * @param condIf 
     */
    void wait(Mutex::Lock& lock, CondIf condIf);

    /**
     * @brief 当条件成立时开始等待, 超过一定时间退出等待
     * 
     * @param lock 
     * @param condIf 
     * @param timeout 
     * @return true 
     * @return false 
     */
    bool wait_for(Mutex::Lock& lock, CondIf condIf, uint32_t timeout);

    /**
     * @brief 唤醒所有陷入等待的线程
     * 
     */
    void notify_all();

    /**
     * @brief 随机唤醒一个等待的线程
     * 
     */
    void notify_one();

private:
    pthread_cond_t m_cond;
};

}