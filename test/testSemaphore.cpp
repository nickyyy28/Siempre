#include "Configurator/Env.h"
#include "Logger/Logger.h"
#include "Thread/Mutex.h"
#include "Thread/Thread.h"
#include <siem>
#include <unistd.h>

int main(int argc, const char** argv)
{
    siem::Env env;
    env.addHelp("p", "config file path");
    env.init(argc, const_cast<char**>(argv));
    
    DEBUG() << env.get("p");

    siem::Semaphore sem;
    int cnt = 0;

    siem::Thread th2([&](){
        while(true) {
            sem.notify();
            cnt ++;
            sleep(2);
        }
    });

    siem::Thread::ptr th1(new siem::Thread([&](){
        while (true) {
            if (sem.trywait()) {
                // sem.wait();
                if (cnt > 5) {
                    th2.destroy();
                    th1->exit();
                }
                DEBUG() << "receiving...";
            } else {
                INFO() << "waiting...";
                usleep(1000 * 1000);
            }
        }
    }));

    th1->join();
    th2.join();

    return 0;
}