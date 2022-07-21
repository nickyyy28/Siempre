#include "Fiber/Scheduler.h"
#include "Logger/Logger.h"

namespace siem{

static Logger::ptr syslog = GET_LOG_BY_NAME(system);
static thread_local Scheduler* t_scheduler = nullptr;
static thread_local Fiber* t_fiber = nullptr;

Scheduler::Scheduler(size_t threads, bool use_caller, const std::string& name)
    : m_threadCount(threads)
    , m_name(name) {
    SIEM_ASSERT(threads > 0);

    if (use_caller) {
        Fiber::getThis();
        --threads;

        SIEM_ASSERT_STR(getThis() == nullptr, "Scheduler Exist");
        t_scheduler = this;
        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        Thread::setThisName(m_name);

        t_fiber = m_rootFiber.get();
        m_rootThread = getThreadID();
        m_threadIds.push_back(m_rootThread);

    } else {
        m_rootThread = -1;
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
    if(siem::getThreadID() != m_rootThread) {
        t_fiber = Fiber::getThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;

    ThreadAndFiber ft;
    while(true) {
        ft.reset();
        bool tickle_me = false;
        bool is_active = false;
        {
            Mutex::Lock lock(m_mutex);
            auto it = m_fibers.begin();
            while(it != m_fibers.end()) {
                if(it->thread_id != -1 && it->thread_id != siem::getThreadID()) {
                    ++it;
                    tickle_me = true;
                    continue;
                }

                SIEM_ASSERT(it->m_fiber || it->m_cb);
                if(it->m_fiber && it->m_fiber->getState() == Fiber::EXEC) {
                    ++it;
                    continue;
                }

                ft = *it;
                m_fibers.erase(it++);
                ++m_activeThreadCount;
                is_active = true;
                break;
            }
            tickle_me |= it != m_fibers.end();
        }

        if(tickle_me) {
            tickle();
        }

        if(ft.m_fiber && (ft.m_fiber->getState() != Fiber::TERM
                        && ft.m_fiber->getState() != Fiber::EXCEPTION)) {
            ft.m_fiber->swapIn();
            --m_activeThreadCount;

            if(ft.m_fiber->getState() == Fiber::READY) {
                schedule(ft.m_fiber);
            } else if(ft.m_fiber->getState() != Fiber::TERM
                    && ft.m_fiber->getState() != Fiber::EXCEPTION) {
                // ft.m_fiber->m_state = Fiber::HOLD;
                ft.m_fiber->setState(Fiber::HOLD);
            }
            ft.reset();
        } else if(ft.m_cb) {
            if(cb_fiber) {
                cb_fiber->reset(ft.m_cb);
            } else {
                cb_fiber.reset(new Fiber(ft.m_cb));
            }
            ft.reset();
            cb_fiber->swapIn();
            --m_activeThreadCount;
            if(cb_fiber->getState() == Fiber::READY) {
                schedule(cb_fiber);
                cb_fiber.reset();
            } else if(cb_fiber->getState() == Fiber::EXCEPTION
                    || cb_fiber->getState() == Fiber::TERM) {
                cb_fiber->reset(nullptr);
            } else {//if(cb_fiber->getState() != Fiber::TERM) {
                // cb_fiber->m_state= Fiber::HOLD;
                cb_fiber->setState(Fiber::HOLD);
                cb_fiber.reset();
            }
        } else {
            if(is_active) {
                --m_activeThreadCount;
                continue;
            }
            if(idle_fiber->getState() == Fiber::TERM) {
                {
                    LOG_INFO(GET_LOG_ROOT()) << "idle fiber term";
                }
                break;
            }

            ++m_idleThreadCount;
            idle_fiber->swapIn();
            --m_idleThreadCount;
            if(idle_fiber->getState() != Fiber::TERM
                    && idle_fiber->getState() != Fiber::EXCEPTION) {
                // idle_fiber->m_state = Fiber::HOLD;
                idle_fiber->setState(Fiber::HOLD);
            }
        }
    }
}

void Scheduler::start(void)
{
    Mutex::Lock lock(m_mutex);
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

    lock.unlock();
}

void Scheduler::stop(void)
{
    m_autoStop = true;
    if (m_threadCount == 0 && m_rootFiber 
        && (m_rootFiber->getState() == Fiber::TERM || m_rootFiber->getState() == Fiber::INIT)) {
        {
            INFO() << "this scheduler stop";
        }
        m_stopping = true;
        if (stopping()) {
            return;
        }
    }

    //bool exit_on_this_fiber = false;
    if(m_rootThread != -1) {
        SIEM_ASSERT(getThis() == this);
    } else {
        SIEM_ASSERT(getThis() != this);
    }

    m_stopping = true;
    for(size_t i = 0; i < m_threadCount; ++i) {
        tickle();
    }

    if(m_rootFiber) {
        tickle();
    }

    if(m_rootFiber) {
        //while(!stopping()) {
        //    if(m_rootFiber->getState() == Fiber::TERM
        //            || m_rootFiber->getState() == Fiber::EXCEPT) {
        //        m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, true));
        //        SYLAR_LOG_INFO(g_logger) << " root fiber is term, reset";
        //        t_fiber = m_rootFiber.get();
        //    }
        //    m_rootFiber->call();
        //}
        if(!stopping()) {
            m_rootFiber->call();
        }
    }

    std::vector<Thread::ptr> thrs;
    {
        Mutex::Lock lock(m_mutex);
        thrs.swap(m_threads);
    }

    for(auto& i : thrs) {
        i->join();
    }
    //if(exit_on_this_fiber) {
    //}
}

void Scheduler::tickle(void)
{
    LOG_INFO(GET_LOG_BY_NAME(system)) << "tickle";
}

void Scheduler::idle(void)
{
    {
        LOG_INFO(GET_LOG_BY_NAME(system)) << "idle";
    }
    while(!stopping()) {
        Fiber::yieldToHold();
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