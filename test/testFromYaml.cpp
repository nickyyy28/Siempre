#include <siem>

siem::ConfigVar<int>::ptr port = siem::Config::lookup<int>("system.port", 80, "the system port");
siem::ConfigVar<std::vector<int>>::ptr vec = siem::Config::lookup<std::vector<int>>("system.int_vec", 
    std::vector<int>{0}, "");
siem::ConfigVar<std::list<int>>::ptr list = siem::Config::lookup<std::list<int>>("system.int_list", 
    std::list<int>{0});
siem::ConfigVar<std::set<int>>::ptr set = siem::Config::lookup<std::set<int>>("system.int_set", 
    std::set<int>{0});
siem::ConfigVar<std::map<std::string, int>>::ptr map = siem::Config::lookup<std::map<std::string, int>>("system.int_map",
    std::map<std::string, int>{});


int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r] %p%f%l%n%m");

    YAML::Node root = YAML::LoadFile("/home/book/Siempre/test/bbb.yaml");

    siem::Config cfg;

    cfg.loadFromYaml(root);

    std::cout << port->getName() << port->toString() << port->getDescription() << std::endl;

    std::cout << vec->getName() << vec->toString() << std::endl;

    std::cout << list->getName() << list->toString() << std::endl;

    std::cout << set->getName() << set->toString() << std::endl;

    std::cout << map->getName() << map->toString() << std::endl;

    siem::Config::lookup<int>("int1", 80);

    return 0;
}