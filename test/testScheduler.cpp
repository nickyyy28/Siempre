#include <siem>

siem::Logger::ptr fiber_log = GET_LOG_BY_NAME(fiber);

void test_fiber()
{
    LOG_ERROR(fiber_log) << "in fiber run";
    static int size = 5;
    sleep(1);
    while (size --) {
        siem::Scheduler::getThis()->schedule(&test_fiber);
    }
}

int main(int argc, char** argv)
{
    siem::Scheduler sc(3, true, "scheduler");

    sc.start();

    std::cout << "共有" << siem::Fiber::getTotalFibers() << std::endl;

    sc.schedule([&](){
        LOG_ERROR(fiber_log) << "in scheduler";
    });

    sc.stop();

    return 0;
}