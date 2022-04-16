#include <siem>

int main(int argc, char** argv)
{
    siem::Scheduler sc(10, true, "scheduler");

    sc.start();
    sc.stop();

    return 0;
}