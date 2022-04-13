#include <Thread/Thread.h>
#include <Logger/Logger.h>
#include <unistd.h>

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r]%N%t %p%f%l%n%m");

    siem::Thread::setThisName("main");

    siem::Thread thread1([&](){
        int i = 10;
        while (i--)
        {
            INFO() << "in aaa thread";
            std::cout << "hello world" << std::endl;
            usleep(1000000);
        }
        
    }, "aaa");

    siem::Thread thread2([&](){
        int i = 10;
        while (i--)
        {
            INFO() << "in bbb thread";
            std::cout << "wow!" << std::endl;
            usleep(1000000);
        }
        
    }, "bbb");

    thread1.join();
    thread2.join();

    return 0;
}