#include <siem/siem>

int main(void)
{
    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r] %p%f%l%n%m");

    YAML::Node root = YAML::LoadFile("/home/book/Siempre/test/aaa.yaml");

    //siem::Config::loadFromYaml(root);

    siem::Config cfg;
    cfg.loadFromYaml(root);

    auto v = siem::Config::lookup<std::vector<int>>("vector");

    if (v) {
        std::cout << v->toString() << std::endl;
    } else {
        WARN() << "could not find the vector";
    }

    auto ret = siem::Config::lookup<std::vector<std::vector<float>>>("mul_vec", 
        std::vector<std::vector<float>>{std::vector<float>{1.1, 2.0}, std::vector<float>{3.3, 4.4}});

    if (ret) {
        std::cout << ret->toString() << std::endl;
    }

    auto ret1 = siem::Config::lookup<std::set<int>>("set", std::set<int>{1,2,3});

    if (ret1) {
        std::cout << ret1->toString() << std::endl;
    }

    return 0;
}