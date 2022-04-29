#include <siem>

class Person{
public:
    Person(const std::string& name, bool sex, int age)
    : m_name(name)
    , m_age(age)
    , m_sex(sex) {

    }

    Person()
    {

    }

    std::string m_name = "";
    bool m_sex = false;
    int m_age = 0;
};

bool operator==(const Person& p1, const Person& p2) {
    return p1.m_name == p2.m_name && p1.m_age == p2.m_age && p1.m_sex == p2.m_sex;
}

namespace siem{

template<>
class TypeCast<std::string, Person>{
public:
    Person operator()(const std::string& str)
    {
        YAML::Node node = YAML::Load(str);
        Person p;
        p.m_name = node["name"].as<std::string>();
        p.m_age = node["age"].as<int>();
        p.m_sex = node["sex"].as<bool>();
        return p;
    }
};

template<>
class TypeCast<Person, std::string>{
public:
    std::string operator()(const Person& vec)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;
        
        node["name"] = vec.m_name;
        node["age"] = vec.m_age;
        node["sex"] = vec.m_sex;

        ss << node;
        return ss.str();
    }
};
}

siem::ConfigVar<std::vector<Person>>::ptr person = siem::Config::lookup<std::vector<Person>>("system.persons", std::vector<Person>{});

int main(void)
{
    YAML::Node root = YAML::LoadFile("/home/book/Siempre/test/bbb.yaml");

    siem::Config cfg;

    cfg.loadFromYaml(root);

    std::cout << person->toString() << std::endl;

    for(auto& v : person->getValue()) {
        std::cout << "name: " << v.m_name << ", age: " << v.m_age << ", sex: " << (v.m_sex ? "male" : "female") << std::endl;
    }

    return 0;
}
