#include "Configurator/Configurator.h"

#include <fstream>
#include <sstream>

namespace siem
{

using MemberList = std::list<std::pair<std::string, const YAML::Node>>;
Config::ConfigVarMap Config::m_datas;

static void listAllMember(const std::string& prefix, const YAML::Node& node, MemberList& output);

void Config::loadFromYaml(const YAML::Node& root)
{
    MemberList list;
    listAllMember("", root, list);

    for (auto i : list) {
        std::string key = i.first;

        if (key.empty()) {
            continue;
        }

        ConfigVarBase::ptr var = lookUpBase(key);

        // 如果找到了配置
        if (var) {
            //如果是普通数据类型
            if (i.second.IsScalar()) {
                var->fromString(i.second.Scalar());
            } else {
                std::stringstream ss;
                ss << i.second;
                var->fromString(ss.str());
            }
        }
    }
}

void Config::loadFromYaml(const std::string& path)
{
    YAML::Node node = YAML::LoadFile(path);
    loadFromYaml(node);
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

void Config::saveAsYaml(const std::string &path)
{
    // std::ofstream ofs;
    // ofs.open(path.c_str(), std::ios::trunc);
    // if (!ofs.is_open()) {
    //     return;
    // }

    

    // ofs.close();

    std::stringstream ss;

    for(auto& v : m_datas) {
        auto key = v.first;
        auto value = v.second;

        if (key.compare("system.stackSize") == 0) {
            continue;
        }

        ss << value->toString() << std::endl;
    }
}

} // namespace siem
