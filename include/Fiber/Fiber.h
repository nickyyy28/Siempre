#pragma once

#include "../Logger/Logger.h"
#include "../Thread/Thread.h"
#include "../common/macro.h"
#include "../common/nocopyable.h"
#include "../Configurator/Configurator.h"
#include "../common/singleton.h"

#include <memory>
#include <functional>
#include <atomic>

#include <ucontext.h>

namespace siem
{

class Fiber;

class FiberStackAllocator{
public:
    static void* Alloc(size_t size);
    static void Dealloc(void* vp, size_t size);

};

class Fiber : public NoCopyAble, public std::enable_shared_from_this<Fiber>{
private:
    Fiber(void);

public:
    using callBack = std::function<void(void)>;
    using ptr = std::shared_ptr<Fiber>;

    Fiber(callBack cb, size_t stack_size = 0, bool is_runinscheduler = false);
    ~Fiber();

    /**
     * @brief 重置协程函数, 并重置状态
     * 
     * @param cb 
     */
    void reset(callBack cb);

    /**
     * @brief 切换到当前协程
     * 
     */
    void swapIn(void);

    /**
     * @brief 切换到后台执行
     * 
     */
    void swapOut(void);

    /**
     * @brief 获取协程ID
     * 
     * @return uint64_t 
     */
    uint64_t getID(void) const;

    enum class State{
        READY,          /* 准备 */
        EXEC,           /* 运行 */
        TERM,           /* 结束 */
    };

    /**
     * @brief 获取协程状态
     * 
     * @return State 
     */
    State getState(void) const;

    /**
     * @brief 设置状态
     * 
     * @param state 
     */
    void setState(State state);

    /**
     * @brief 将当前线程切换到执行状态 resume
     * @pre 执行的为当前线程的主协程
     */
    void call();

    /**
     * @brief 将当前线程切换到后台 yield
     * @pre 执行的为该协程
     * @post 返回到线程的主协程
     */
    void back();

    /**
     * @brief 将当前线程调度到后台挂起, 返回主协程
     * 
     */
    void yield();

    /**
     * @brief 主协程将该协程调度到前台运行
     * 
     */
    void resume();

private:

    /**
     * @brief 上下文
     * 
     */
    ucontext_t m_context;

    /**
     * @brief 协程栈大小
     * 
     */
    size_t m_stackSize = 0;

    /**
     * @brief 协程ID
     * 
     */
    uint64_t m_id = 0;

    /**
     * @brief 协程状态
     * 
     */
    State m_state = State::READY;

    /**
     * @brief 协程栈
     * 
     */
    void* m_stack = nullptr;

    /**
     * @brief 协程执行方法
     * 
     */
    callBack m_cb;

    /**
     * @brief 该协程是否由调度器调度
     * 
     */
    bool m_is_runinscheduler;

public:

    /**
     * @brief 设置当前协程
     * 
     * @param f 
     */
    static void setThis(Fiber* f);

    /**
     * @brief 返回当前运行的协程, 如果未创建主协程则创建主协程并返回
     * 
     * @return Fiber::ptr 
     */
    static Fiber::ptr getThis(void);

    /**
     * @brief 协程切换值至后台并切换至Ready状态
     * 
     */
    static void yieldToReady(void);

    /**
     * @brief 协程切换至后台并切换至Hold状态
     * 
     */
    static void yieldToHold(void);

    /**
     * @brief 获取当前所有协程数
     * 
     * @return uint64_t 
     */
    static uint64_t getTotalFibers(void);

    /**
     * @brief 执行主方法
     * 
     */
    static void MainFunc(void);

    /**
     * @brief 协程执行函数
     * @post 执行完成返回到线程调度协程
     */
    static void CallerMainFunc();

    /**
     * @brief 获取当前协程id
     * 
     * @return uint64_t 
     */
    static uint64_t getCurFiberID(void);
};

} // namespace siem