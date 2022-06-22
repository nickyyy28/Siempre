#include "Logger/Logger.h"
#include "fs/fs.h"
#include <iostream>
#include <siem>

int main()
{
    siem::ThreadPool pool(0, 10);
    std::cout << "Hello world" << std::endl;
    return 0;
}

