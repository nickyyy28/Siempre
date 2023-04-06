#include <ucontext.h>
#include <functional>
#include <memory>

class Fiber : public std::enable_shared_from_this<Fiber>{
private:
    Fiber();

public:
    typedef std::function<void(void)> callBack;
    typedef std::shared_ptr<Fiber> ptr;

    enum class FiberState{
        READY,
        RUNNING,
        TERM
    };

    Fiber(callBack cb, size_t stack_size = 0);
    ~Fiber();

    /**
     * @brief 设置当前协程
     * 
     * @param f 
     */
    static void setThis(Fiber* f);

    /**
    * @brief 返回当前线程正在执行的协程
    * @details 如果当前线程还未创建协程，则创建线程的第一个协程，
    * 且该协程为当前线程的主协程，其他协程都通过这个协程来调度，也就是说，其他协程
    * 结束时,都要切回到主协程，由主协程重新选择新的协程进行resume
    * @attention 线程如果要创建协程，那么应该首先执行一下Fiber::GetThis()操作，以初始化主函数协程
    * @return 当前正在执行的协程
    */
    static Fiber::ptr getThis(void);

private:
    /// 协程id
    uint64_t m_id        = 0;
    /// 协程栈大小
    uint32_t m_stacksize = 0;
    /// 协程状态
    FiberState m_state   = FiberState::READY;
    /// 协程上下文
    ucontext_t m_ctx;
    /// 协程栈地址
    void *m_stack = nullptr;
    /// 协程入口函数
    callBack m_cb;

};