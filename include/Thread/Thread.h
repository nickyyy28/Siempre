#ifndef __THREAD_H
#define __THREAD_H

#include <memory>
#include <pthread.h>
#include <functional>
#include <list>

#include <Logger/Logger.h>
#include <common/nocopyable.h>
#include <common/singleton.h>

namespace siem{

/**
 * @brief 基于pthread的封装
 * 
 */
class Thread : public NoCopyAble , public std::enable_shared_from_this<Thread>{
public:
    typedef std::shared_ptr<Thread> ptr;
    typedef std::function<void()> callBack;

    Thread(callBack cb, const std::string& name);
    ~Thread();

    /**
     * @brief 获取线程名称
     * 
     * @return const std::string& 
     */
    const std::string& getName(void) const;

    /**
     * @brief 获取线程pid
     * 
     * @return pid_t 
     */
    pid_t getID(void) const;

    /**
     * @brief 阻塞式开始线程
     * 
     */
    void join(void);

    /**
     * @brief 非阻塞开始线程
     * 
     */
    void detach(void);

    /**
     * @brief 获取当前运行线程指针
     * 
     * @return Thread* 
     */
    static Thread* getThis(void);

    /**
     * @brief 获取当前运行线程名称
     * 
     * @return const std::string& 
     */
    static const std::string& getThisName(void);

    /**
     * @brief 设置当前运行线程名称
     * 
     * @param name 
     */
    static void setThisName(const std::string& name);

    /**
     * @brief 运行函数
     * 
     * @param arg 
     * @return void* 
     */
    static void* run(void * arg);
private:
    pid_t m_id;
    pthread_t m_thread;
    std::function<void()> m_cb;
    std::string m_name;
};

}

#endif //__THREAD_H