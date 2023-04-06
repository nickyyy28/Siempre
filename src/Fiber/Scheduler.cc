#include "Fiber/Scheduler.h"
#include "Fiber/Fiber.h"
#include "Logger/Logger.h"
#include "common/macro.h"
#include <string>

namespace siem{

static Logger::ptr syslog = GET_LOG_BY_NAME(system);

//当前线程调度器
static thread_local Scheduler* t_scheduler = nullptr;

//当前线程主协程
static thread_local Fiber* t_fiber = nullptr;


Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
    : m_threadCount(threads)
    , m_name(name)
    , m_isusecaller(use_caller) {

    //判断传入参数线程数是否大于0
    SIEM_ASSERT(threads > 0);

    //如果将创建调度器的线程作为工作线程的话, 那么就在该线程创建一个调度协程, 注意该线程的调度协程不是该线程的主协程
    if (use_caller) {
        Fiber::getThis();
        --threads;

        SIEM_ASSERT_STR(getThis() == nullptr, "Scheduler Exist");
        t_scheduler = this;
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        Thread::setThisName(m_name);

        t_fiber = m_rootFiber.get();
        m_rootThreadID = getThreadID();
        m_threadIds.push_back(m_rootThreadID);

    } else {
        m_rootThreadID = -1;
    }

    m_threadCount = threads;

}

void Scheduler::setThis(void)
{
    t_scheduler = this;
}

Scheduler* Scheduler::getThis(void)
{
    return t_scheduler;
}

Fiber* Scheduler::getMainFiber(void)
{
    return t_fiber;
}

void Scheduler::run(void)
{
    {
        LOG_DEBUG(GET_LOG_ROOT()) << m_name << " run";
    }
    setThis();

    //如果当前线程不是caller线程的话将主协程作为当前线程的调度协程
    if(siem::getThreadID() != m_rootThreadID) {
        t_fiber = Fiber::getThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this), 0, true));
    Fiber::ptr cb_fiber;

    SchedulerTask task;
    while(true) {
        task.reset();

        //是否制定了其他线程进行调度
        bool tickle_me = false;
        bool is_active = false;
        {
            Mutex::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            //遍历所有调度任务
            while(it != m_fibers.end()) {

                //如果获取到的调度任务指定线程调度, 但不是当前线程
                if(it->thread_id != -1 && it->thread_id != siem::getThreadID()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                //找到一个未指定线程或者指定本线程任务
                SIEM_ASSERT(it->m_fiber || it->m_cb);
                
                //如果该协程不为空且为正在执行状态的话就continue
                if(it->m_fiber && it->m_fiber->getState() == Fiber::State::EXEC) {
                    ++it;
                    continue;
                }

                task = *it;
                //从协程数组中删除
                m_fibers.erase(it++);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }

            // 当前线程拿完一个任务后，发现任务队列还有剩余，那么tickle一下其他线程
            tickle_me |= it != m_fibers.end();
        }

        if(tickle_me) {
            tickle();
        }

        if (task.m_fiber) {
            task.m_fiber->resume();
            --m_activeThreadCount;
            task.reset();
        } else if (task.m_cb) {
            if (cb_fiber) {
                cb_fiber->reset(task.m_cb);
            } else {
                cb_fiber.reset(new Fiber(task.m_cb, 0, true));
            }

            task.reset();
            cb_fiber->resume();
            --m_activeThreadCount;
            cb_fiber.reset();
        } else {
            //进入该分支说明任务队列空了
            if (idle_fiber->getState() == Fiber::State::TERM) {
                // 如果调度器没有调度任务，那么idle协程会不停地resume/yield，不会结束，如果idle协程结束了，那一定是调度器停止了
                LOG_ERROR(GET_LOG_BY_NAME(root)) << "idle fiber term";
                break;
            }
            ++m_idleThreadCount;
            idle_fiber->resume();
            --m_idleThreadCount;
        }
    }

    INFO() << "siem::Scheduler::run() exit";
}

std::string Scheduler::getName() const
{
    return m_name;
}

void Scheduler::start(void)
{
    Mutex::Lock lock(m_mutex);

    //是否第一次执行start, 否则退出start
    if (!m_stopping) {
        return;
    }
    m_stopping = false;

    SIEM_ASSERT(m_threads.empty());
    m_threads.resize(m_threadCount);
    for (size_t i = 0 ; i < m_threadCount ; i++) {
        m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
            m_name + "_" + std::to_string(i)));
        m_threadIds.push_back(m_threads[i]->getID());
    }
}

void Scheduler::stop(void)
{
    if (stopping()) {
        return;
    }
    m_stopping = true;
    
    //如果use caller，那只能由caller线程发起stop
    if (m_isusecaller) {
        SIEM_ASSERT(getThis() == this);
    } else {
        SIEM_ASSERT(getThis() != this);
    }

    for(size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    /// 在use caller情况下，调度器协程结束时，应该返回caller协程
    if (m_rootFiber) {
        m_rootFiber->resume();
        DEBUG() << "m_rootFiber end";
    }

    std::vector<Thread::ptr> thrs;
    {
        Mutex::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for(auto& i : thrs) {
        i->join();
    }
}

void Scheduler::tickle(void)
{
    // LOG_INFO(GET_LOG_BY_NAME(system)) << "tickle";
}

void Scheduler::idle(void)
{
    {
        LOG_INFO(GET_LOG_BY_NAME(system)) << "idle";
    }
    while(!stopping()) {
        Fiber::getThis()->yield();
    }
}

bool Scheduler::stopping(void)
{
    Mutex::Lock lock(m_mutex);
    return m_autoStop && m_stopping
        && m_fibers.empty() && m_activeThreadCount == 0;
}

void Scheduler::switchTo(int thread)
{
    SIEM_ASSERT(Scheduler::getThis() != nullptr);
    if(Scheduler::getThis() == this) {
        if(thread == -1 || thread == siem::getThreadID()) {
            return;
        }
    }
    schedule(Fiber::getThis(), thread);
    Fiber::yieldToHold();
}

std::ostream& Scheduler::dump(std::ostream& os) {
    os << "[Scheduler name=" << m_name
       << " size=" << m_threadCount
       << " active_count=" << m_activeThreadCount
       << " idle_count=" << m_idleThreadCount
       << " stopping=" << m_stopping
       << " ]" << std::endl << "    ";
    for(size_t i = 0; i < m_threadIds.size(); ++i) {
        if(i) {
            os << ", ";
        }
        os << m_threadIds[i];
    }
    return os;
}

}