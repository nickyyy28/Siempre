#pragma once

#include "./Fiber.h"
#include "../build_config.h"

#include <list>

namespace siem{

class SimpleScheduler{
public FUNCTION:
    SimpleScheduler ();

    void schedule(Fiber::ptr fiber);

    void run();

private FUNCTION:

public VARIABLE:
    std::list<Fiber::ptr> m_tasks;

};

};