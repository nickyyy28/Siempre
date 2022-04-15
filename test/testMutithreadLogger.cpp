#include <siem>

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r]%N%t %p%f%l%n%m");

    siem::Thread thread1([&](){
        int i = 20;
        while (i--)
        {
            INFO() << "thread1, count: " << i;

            std::cout << "test1" << std::endl;

            // sleep(1);
        }
    }, "thread1");

    siem::Thread thread2([&](){
        int i = 20;
        while (i--)
        {
            INFO() << "thread2, count: " << i;

            std::cout << "test2" << std::endl;

            // sleep(1);
        }
    }, "thread2");

    thread1.join();
    thread2.join();

    return 0;
}
