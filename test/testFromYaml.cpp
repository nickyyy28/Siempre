#include <Configurator/Configurator.h>
#include <Logger/Logger.h>

siem::ConfigVar<int>::ptr port = siem::Config::lookup<int>("port", 80, "the system port");
siem::ConfigVar<std::vector<int>>::ptr vec = siem::Config::lookup<std::vector<int>>("int_vec", 
    std::vector<int>{0}, "");
siem::ConfigVar<std::list<int>>::ptr list = siem::Config::lookup<std::list<int>>("int_list", 
    std::list<int>{0});
siem::ConfigVar<std::set<int>>::ptr set = siem::Config::lookup<std::set<int>>("int_set", 
    std::set<int>{0});


int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r] %p%f%l%n%m");

    YAML::Node root = YAML::LoadFile("/home/book/Siempre/test/bbb.yaml");

    siem::Config cfg;

    cfg.loadFromYaml(root);

    std::cout << port->toString() << std::endl;

    std::cout << vec->toString() << std::endl;

    std::cout << list->toString() << std::endl;

    std::cout << set->toString() << std::endl;

    

    return 0;
}