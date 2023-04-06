#include "Fiber/SimpleScheduler.h"

namespace siem {

SimpleScheduler::SimpleScheduler()
{

}

void SimpleScheduler::schedule(Fiber::ptr task)
{
    m_tasks.push_back(task);
}

void SimpleScheduler::run()
{
    Fiber::ptr task;
    auto it = m_tasks.begin();

    while(it != m_tasks.end()) {
        task = *it;
        m_tasks.erase(it++);
        task->resume();
    }
}

};