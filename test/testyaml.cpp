#include "yaml-cpp/node/node.h"
#include <cstdlib>
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

    YAML::Node node;
    YAML::Node person;
    person["name"] = "Nick";
    person["age"] = 20;
    node["msg"] = "hello";
    node["person"] = person;

    std::cout << node << std::endl;

    return 0;
}