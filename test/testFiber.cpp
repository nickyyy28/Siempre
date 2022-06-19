#include <siem>
#include <unistd.h>

siem::Logger::ptr root = GET_LOG_ROOT();

void run_in_fiber();

int main(int argc, char** argv)
{
    siem::Scheduler::getThis();
    siem::Fiber::getThis();

    siem::Fiber::ptr fib(new siem::Fiber(run_in_fiber));

    DEBUG() << "main swap before";

    if (fib) {
        fib->swapIn();
    } else {
        ERROR() << "error fiber";
    }

    DEBUG() << "main swap after";

    if (fib) {
        fib->swapIn();
    } else {
        ERROR() << "error fiber";
    }

    DEBUG() << "main swap end";

    return 0;
}

void run_in_fiber()
{
    INFO() << "fiber run start";

    siem::Fiber::yieldToHold();

    INFO() << "fiber id: " << siem::Fiber::getCurFiberID();

    INFO() << "fiber run end";

}
