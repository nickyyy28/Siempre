#include <functional>
#include <iostream>
#include <siem/siem>
#include <string>
#include <unistd.h>
#include <vector>



class Test{
public:
    void log(int age, const std::string& name)
    {
        INFO() << "name: " << name << " age: " << age;
    }
private:

};

int main()
{
    siem::ThreadPool pool(4);
    int i = 0;
    siem::Mutex mtx;

    Test t;
    
    std::function<void()> func = [&]{
        siem::Mutex::Lock lock(mtx);
        WARN() << "running..." << i;
        i++;
    };

    INFO() << "thread pool before start";

    pool.start();

    INFO() << "thread pool after start";

    pool.addTask(std::bind(&Test::log, &t, 20, "nick"));

    pool.addTask([&](){
        int i = 0;
        while(true) {
            DEBUG() << "i = " << i;
            usleep(1000 * 500);
            i++;
            if (i > 20) break;
        }
    });

    while(true) {
        // INFO() << "while loop...";
        pool.addTask(func);
        usleep(1000 * 10);
    }

    return 0;
}

