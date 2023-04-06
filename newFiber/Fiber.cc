#include "Fiber.h"
#include <atomic>

/// 全局静态变量，用于生成协程id
static std::atomic<uint64_t> s_fiber_id = 0;
/// 全局静态变量，用于统计当前的协程数
static std::atomic<uint64_t> s_fiber_count = 0;

/// 线程局部变量，当前线程正在运行的协程
static thread_local Fiber *t_fiber = nullptr;
/// 线程局部变量，当前线程的主协程，切换到这个协程，就相当于切换到了主线程中运行，智能指针形式
static thread_local Fiber::ptr t_thread_fiber = nullptr;

