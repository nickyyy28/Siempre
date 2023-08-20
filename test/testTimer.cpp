#include <siem/siem>
#include <siem/base/TimeStamp.h>
#include <siem/Thread/Timer.h>
#include <unistd.h>

class AAA : public siem::TimerManager{
public:
    AAA() = default;
    ~AAA() = default;
    void onTimerInsertedAtFront() override
    {

    }
};

int main(int argc, const char** argv)
{
    AAA mgr;
    siem::Timer::ptr t1 = mgr.addTimer(1000, [&](){

    });

    return 0;
}