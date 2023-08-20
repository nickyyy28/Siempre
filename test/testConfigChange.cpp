#include <siem/Configurator/Configurator.h>

siem::ConfigVar<int>::ptr var = siem::Config::lookup<int>("int", 20);

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r] %p%f%l%n%m");

    std::stringstream ss;
    ss << var->toString();

    uint64_t hash = CityHash64(ss.str().c_str(), std::strlen(ss.str().c_str()));

    std::cout << "hash = " << hash << std::endl;

    // var->addListener()

    return 0;
}