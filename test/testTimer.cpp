#include "Logger/Logger.h"
#include "Thread/Timer.h"
#include <siem>
#include <unistd.h>

int main(int argc, const char** argv)
{
    siem::SocketTimer timer(1000, [&]{
        DEBUG() << "in timer...";
    });

    timer.start();

    while(true) {
        sleep(1);
    }
    return 0;
}