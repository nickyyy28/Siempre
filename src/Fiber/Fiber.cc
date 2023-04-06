#include "Fiber/Fiber.h"
#include "Logger/Logger.h"
#include "Fiber/Scheduler.h"
#include "common/macro.h"
#include <ucontext.h>

namespace siem{

//全局静态变量，用于生成协程id
static std::atomic<uint64_t> s_fiber_id = 0;

//全局静态变量，用于统计当前的协程数
static std::atomic<uint64_t> s_fiber_numbers = 0;

//线程局部变量，当前线程正在运行的协程
static thread_local Fiber* t_fiber = nullptr;

//线程的主协程
static thread_local Fiber::ptr t_thread_fiber = nullptr;

static ConfigVar<uint32_t>::ptr s_stackSize = Config::lookup<uint32_t>("system.stackSize", 1024 * 1024, "Fiber Stack Size");

using StackAllocator = FiberStackAllocator;

void* FiberStackAllocator::Alloc(size_t size)
{
    return malloc(size);
}

void FiberStackAllocator::Dealloc(void* vp, size_t size)
{
    free(vp);
}

Fiber::Fiber(void)
{
    m_state = State::EXEC;
    setThis(this);
    m_is_runinscheduler = false;

    if (getcontext(&m_context)) {
        SIEM_ASSERT_STR(false, "get context fail");
    }

    ++s_fiber_numbers;
}

Fiber::Fiber(callBack cb, size_t stack_size, bool is_runinscheduler)
    : m_cb(cb)
    , m_stackSize(stack_size ? stack_size : s_stackSize->getValue())
    , m_id(++s_fiber_id) 
    , m_is_runinscheduler(is_runinscheduler) {

    m_stack = StackAllocator::Alloc(m_stackSize);

    if (getcontext(&m_context)) {
        SIEM_ASSERT_STR(false, "get context fail");
    }

    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stackSize;

    /*if(!is_usecaller) {
        makecontext(&m_context, &Fiber::MainFunc, 0);
    } else {
        makecontext(&m_context, &Fiber::CallerMainFunc, 0);
    }*/

    makecontext(&m_context, &Fiber::MainFunc, 0);

}

Fiber::~Fiber()
{
    --s_fiber_numbers;
    if (m_stack) {
        SIEM_ASSERT(m_state == State::TERM || m_state == State::READY);
        StackAllocator::Dealloc(m_stack, m_stackSize);
    } else {
        SIEM_ASSERT(!m_cb);
        SIEM_ASSERT(m_state == State::EXEC);

        Fiber* cur = t_fiber;
        if (cur == this) {
            setThis(nullptr);
        }
    }
}

void Fiber::setThis(Fiber* f)
{
    t_fiber = f;
}

Fiber::ptr Fiber::getThis(void)
{
    if (t_fiber) {
        return t_fiber->shared_from_this();
    }

    Fiber::ptr main_fiber(new Fiber());
    t_thread_fiber = main_fiber;
    SIEM_ASSERT(t_fiber == t_thread_fiber.get());
    t_fiber = main_fiber.get();
    return main_fiber;
}

void Fiber::yieldToReady(void)
{
    Fiber::ptr cur = getThis();
    SIEM_ASSERT(cur->m_state == State::EXEC);
    cur->m_state = State::READY;
    cur->swapOut();
}

void Fiber::yieldToHold(void)
{
    Fiber::ptr cur = getThis();
    SIEM_ASSERT(cur->m_state == State::EXEC);
    // cur->m_state = HOLD;
    cur->swapOut();
}

uint64_t Fiber::getTotalFibers(void)
{
    return s_fiber_numbers;
}

void Fiber::MainFunc(void)
{
    Fiber::ptr cur = getThis();
    SIEM_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = State::TERM;
    } catch (std::exception& e) {
        cur->m_state = State::TERM;
        ERROR() << "Fiber Exception" << e.what();
    } catch (...) {
        cur->m_state = State::TERM;
        ERROR() << "Fiber Exception";
    }

    Fiber* raw_ptr = cur.get();
    cur.reset();
    // raw_ptr->swapOut();
    raw_ptr->yield();

    SIEM_ASSERT_STR(false, "never reach fiber_id=" + std::to_string(raw_ptr->getID()));
}

void Fiber::CallerMainFunc()
{
    Fiber::ptr cur = getThis();
    SIEM_ASSERT(cur);
    try {
        cur->m_cb();
        cur->m_cb = nullptr;
        cur->m_state = State::TERM;
    } catch (std::exception& ex) {
        cur->m_state = State::TERM;
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "Fiber Except: " << ex.what()
            << " fiber_id=" << cur->getID()
            << std::endl
            << siem::BackTraceToString(100, 2);
    } catch (...) {
        cur->m_state = State::TERM;
        LOG_ERROR(GET_LOG_BY_NAME(system)) << "Fiber Except"
            << " fiber_id=" << cur->getID()
            << std::endl
            << siem::BackTraceToString(100, 2);
    }

    auto raw_ptr = cur.get();
    cur.reset();
    raw_ptr->swapOut();
    SIEM_ASSERT_STR(false, "never reach fiber_id=" + std::to_string(raw_ptr->getID()));
}

uint64_t Fiber::getCurFiberID(void)
{
    if (t_fiber) {
        return t_fiber->getID();
    }

    return 0;
}

Fiber::State Fiber::getState(void) const
{
    return m_state;
}

void Fiber::setState(State state)
{
    m_state = state;
}

void Fiber::call() {
    setThis(this);
    m_state = State::EXEC;
    if(swapcontext(&t_thread_fiber->m_context, &m_context)) {
        SIEM_ASSERT_STR(false, "swapcontext");
    }
}

void Fiber::resume()
{
    SIEM_ASSERT(m_state != State::TERM || m_state != State::EXEC);
    setThis(this);
    m_state = State::EXEC;

    if (m_is_runinscheduler) {
        if(swapcontext(&Scheduler::getMainFiber()->m_context, &m_context)) {
            SIEM_ASSERT_STR(false, "swapcontext");
        }
    } else {
        if(swapcontext(&t_thread_fiber->m_context, &m_context)) {
            SIEM_ASSERT_STR(false, "swapcontext");
        }
    }
}

void Fiber::back() {
    setThis(t_thread_fiber.get());
    if(swapcontext(&m_context, &t_thread_fiber->m_context)) {
        SIEM_ASSERT_STR(false, "swapcontext");
    }
}

void Fiber::yield()
{
    SIEM_ASSERT(m_state == State::EXEC || m_state == State::TERM);
    setThis(t_thread_fiber.get());
    if (m_state != State::READY) {
        m_state = State::READY;
    }

    if (m_is_runinscheduler) {
        if(swapcontext(&m_context, &Scheduler::getMainFiber()->m_context)) {
            SIEM_ASSERT_STR(false, "swapcontext");
        }
    } else {
        if(swapcontext(&m_context, &t_thread_fiber->m_context)) {
            SIEM_ASSERT_STR(false, "swapcontext");
        }
    }
    
}

void Fiber::reset(callBack cb)
{
    SIEM_ASSERT(m_stack);
    SIEM_ASSERT(m_state == State::TERM
        || m_state == State::READY);

    m_cb = cb;

    if (getcontext(&m_context)) {
        SIEM_ASSERT_STR(false, "get context fail");
    }

    m_context.uc_link = nullptr;
    m_context.uc_stack.ss_sp = m_stack;
    m_context.uc_stack.ss_size = m_stackSize;

    makecontext(&m_context, &Fiber::MainFunc, 0);

    m_state = State::READY;
}

void Fiber::swapIn(void)
{
    setThis(this);
    SIEM_ASSERT(m_state != State::EXEC);

    m_state = State::EXEC;

    if (swapcontext(&Scheduler::getMainFiber()->m_context, &m_context)) {
        SIEM_ASSERT_STR(false,"swap context");
    }
}

void Fiber::swapOut(void)
{
    setThis(Scheduler::getMainFiber());

    if (swapcontext(&m_context, &Scheduler::getMainFiber()->m_context)) {
        SIEM_ASSERT_STR(false,"swap context");
    }
}

uint64_t Fiber::getID(void) const
{
    return m_id;
}

}