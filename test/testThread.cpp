#include <siem>
#include <unistd.h>

int count = 0;

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r]%N%t %p%f%l%n%m");

    siem::Thread::setThisName("main");

    siem::RWMutex rw;

    /*siem::Thread thread1([&](){
        int i = 1000000;
        while (i--)
        {
            siem::RWMutex::WriteLock lock(rw);
            count ++;
        }
        
    }, "write thread1");

    siem::Thread thread2([&](){
        int i = 1000000;
        while (i--)
        {
            siem::RWMutex::WriteLock lock(rw);
            count++;
        }
        
    }, "write thread2");*/

    std::list<siem::Thread::ptr> list;
    for (int i = 0 ; i < 5 ; i ++) {
        list.push_back(std::make_shared<siem::Thread>([&](){
            int i = 1000000;
            while (i--)
            {
                siem::RWMutex::WriteLock lock(rw);
                count++;
            }
        }, "therad: " + std::to_string(i)));
    }

    for (auto& th : list) {
        th->detach();
    }

    sleep(5);

    std::cout << "count = " << count;

    return 0;
}