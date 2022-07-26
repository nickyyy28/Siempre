#include "base/TimeStamp.h"
#include <iostream>

int main(int argc, const char** argv)
{
    std::cout << siem::TimeStamp::now().toString() << std::endl;
    return 0;
}
