#include <iostream>
#include <yaml-cpp/yaml.h>
#include <fstream>

void test_yaml()
{
    YAML::Node root = YAML::LoadFile("/home/book/Siempre/test/aaa.yaml");
    std::cout << root << std::endl;
}

int main(void)
{
    test_yaml();

    return 0;
}