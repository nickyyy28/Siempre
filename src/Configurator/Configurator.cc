#include "Configurator/Configurator.h"

namespace siem
{

using MemberList = std::list<std::pair<std::string, const YAML::Node>>;
Config::ConfigVarMap Config::m_datas;

static ConfigVar<std::string>::ptr tomcat_port = Config::lookup<std::string>("tomcat_port", "8080", "the tomcat server default port");

static ConfigVar<std::vector<int>>::ptr vec = Config::lookup<std::vector<int>>("vector", std::vector<int>{1, 2, 3}, "this a vector config");

static void listAllMember(const std::string& prefix, const YAML::Node& node, MemberList& output);

void Config::loadFromYaml(const YAML::Node& root)
{
    MemberList list;
    listAllMember("", root, list);

    for (auto i : list) {
        std::string key = i.first;

        INFO() << "key: " << key << " value: " << i.second.Scalar();

        if (key.empty()) {
            continue;
        }

        ConfigVarBase::ptr var = lookUpBase(key);

        // 如果找到了配置
        if (var) {
            //如果是普通数据类型
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
                INFO() << "普通数据";
            } else {
                INFO() << "复杂数据";
                std::stringstream ss;
                WARN() << "数据" << i.second.Scalar();
                ss << i.second.Scalar();
                var->fromString(ss.str());
            }
        }
    }
}

static void listAllMember(const std::string& prefix, const YAML::Node& node, MemberList& output)
{
    if (prefix.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != std::string::npos) {
        ERROR() << "prefix invalid";
        throw std::invalid_argument(prefix);
    }

    output.push_back(std::make_pair(prefix, node));

    if (node.IsMap()) {
        for (auto it = node.begin() ; it != node.end() ; ++it) {
            listAllMember((prefix.empty() ? it->first.Scalar() : (prefix + "." + it->first.Scalar())), it->second, output);
        }
    }
}

} // namespace siem
