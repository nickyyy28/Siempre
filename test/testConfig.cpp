#include <siem>

#include <iostream>

// class intConfig : siem::ConfigVar<int>{
//     intConfig(){}
//     ~intConfig(){}
// }

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r] %p%f%l%n%m");

    FATAL() << "error";

    int a = 10;

    //siem::ConfigVar<int> ptr(new siem::ConfigVar<int>("num", "this is a number", a));

    //siem::ConfigVar<double> val("num", 10);

    //std::cout << val.toString() << std::endl;

    //val.fromString("20");

    siem::ConfigVar<int>::ptr cfg = siem::Config::lookup<int>("test", 500);

    if (cfg) {
        std::cout << cfg->toString() << std::endl;
    }

    // if (!siem::Config::lookup<int>(std::string("num"), 20)) {
    //     INFO() << "cant find root config";
    // }

    std::cout << "hello world";

    siem::Config::lookup<double>(".test", 114.514, "error config");

    return 0;
}