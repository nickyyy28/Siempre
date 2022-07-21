#include "Thread/ThreadPool.h"
#include <siem>
#include <unistd.h>

int count = 0;

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r]%N%t %p%f%l%n%m");

    // siem::Thread::setThisName("main");

    // siem::RWMutex rw;

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

    siem::_mutex mtx;

    std::list<siem::_threadPtr> list;
    for (int i = 0 ; i < 5 ; i ++) {
        list.push_back(std::make_shared<siem::_thread>([&](){
            int i = 1000000;
            while (i--)
            {
                // siem::_scopeLock lock(mtx);
                count++;
            }
        }));
    }

    for (auto& th : list) {
        th->detach();
    }

    sleep(5);

    std::cout << "count = " << count;

    return 0;
}