/**
 * @file Configurator.h
 * @author nickyyy28 (A3092989305@163.com)
 * @brief config module
 * @version 0.1
 * @date 2022-04-11
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef __SIEM_CONFIGURATOR_H
#define __SIEM_CONFIGURATOR_H

#include <memory>
#include <map>
#include <list>
#include <vector>
#include <set>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <string>
#include <type_traits>
#include <functional>

#include <boost/lexical_cast.hpp>

#include "common/singleton.h"
#include "Logger/Logger.h"
#include "Fiber/Fiber.h"
#include "utils/utils.h"

#include "city/city.h"
#include "yaml-cpp/yaml.h"

namespace siem{

/**
 * @brief 基础配置类
 * 
 */
class ConfigVarBase{
public:
    typedef std::shared_ptr<ConfigVarBase> ptr;

    /**
     * @brief 基础配置类的构造
     * 
     * @param name          配置名称
     * @param description   描述
     */
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name)
        , m_description(description) {

    }
    virtual ~ConfigVarBase() {}

    /**
     * @brief 获取配置名称
     * 
     * @return const std::string& 
     */
    const std::string& getName() const { return m_name; }

    /**
     * @brief 获取描述
     * 
     * @return const std::string& 
     */
    const std::string& getDescription() const {return m_description; }

    /**
     * @brief 将配置转换成std::string
     * 
     * @return std::string 
     */
    virtual std::string toString() = 0;

    /**
     * @brief 从字符串读取配置
     * 
     * @param str   配置字符串
     * @return true
     * @return false
     */
    virtual bool fromString(const std::string& str) = 0;

    /**
     * @brief 获取配置类型字符串
     * 
     * @return std::string 
     */
    virtual std::string getTypeName() const = 0;

protected:
    std::string m_name;
    std::string m_description;
};

/**
 * @brief 基础类型转换仿函数类
 * 
 * @tparam F from type
 * @tparam T to type
 */
template<class F, class T>
class TypeCast{
public:
    T operator()(const F& f)
    {
        return boost::lexical_cast<T>(f);
    }
};

/**
 * @brief 针对std::string到std::vector<T>的偏特化
 * 
 * @tparam T 基础类型
 */
template<class T>
class TypeCast<std::string, std::vector<T>>{
public:
    std::vector<T> operator()(const std::string& str)
    {
        std::vector<T> vec;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;

        for (size_t i = 0 ; i < node.size() ; i++) {
            ss.str("");
            ss << node[i];
            vec.push_back(TypeCast<std::string, T>()(ss.str()));
        }

        return vec;
    }
};

/**
 * @brief 针对std::vector<F>到std::string的偏特化
 * 
 * @tparam F 
 */
template<class F>
class TypeCast<std::vector<F>, std::string>{
public:
    std::string operator()(const std::vector<F>& vec)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;
        for (auto& v : vec) {
            node.push_back(YAML::Load(TypeCast<F, std::string>()(v)));
        }
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 针对std::string到std::list<T>的偏特化
 * 
 * @tparam T 基础类型
 */
template<class T>
class TypeCast<std::string, std::list<T>>{
public:
    std::list<T> operator()(const std::string& str)
    {
        std::list<T> list;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;

        for (size_t i = 0 ; i < node.size() ; i++) {
            ss.str("");
            ss << node[i];
            list.push_back(TypeCast<std::string, T>()(ss.str()));
        }

        return list;
    }
};

/**
 * @brief 针对std::list<F>到std::string的偏特化
 * 
 * @tparam F 
 */
template<class F>
class TypeCast<std::list<F>, std::string>{
public:
    std::string operator()(const std::list<F>& vec)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;
        for (auto& v : vec) {
            node.push_back(YAML::Load(TypeCast<F, std::string>()(v)));
        }
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 针对std::string到std::set<T>的偏特化
 * 
 * @tparam T 基础类型
 */
template<class T>
class TypeCast<std::string, std::set<T>>{
public:
    std::set<T> operator()(const std::string& str)
    {
        std::set<T> set;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;

        for (size_t i = 0 ; i < node.size() ; i++) {
            ss.str("");
            ss << node[i];
            // vec.push_back(TypeCast<std::string, T>()(ss.str()));
            set.insert(TypeCast<std::string, T>()(ss.str()));
        }

        return set;
    }
};

/**
 * @brief 针对std::set<F>到std::string的偏特化
 * 
 * @tparam F 
 */
template<class F>
class TypeCast<std::set<F>, std::string>{
public:
    std::string operator()(const std::set<F>& vec)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;
        for (auto& v : vec) {
            node.push_back(YAML::Load(TypeCast<F, std::string>()(v)));
        }
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 针对std::string到std::unordered_set<T>的偏特化
 * 
 * @tparam T 基础类型
 */
template<class T>
class TypeCast<std::string, std::unordered_set<T>>{
public:
    std::unordered_set<T> operator()(const std::string& str)
    {
        std::unordered_set<T> set;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;

        for (size_t i = 0 ; i < node.size() ; i++) {
            ss.str("");
            ss << node[i];
            // vec.push_back(TypeCast<std::string, T>()(ss.str()));
            set.insert(TypeCast<std::string, T>()(ss.str()));
        }

        return set;
    }
};

/**
 * @brief 针对std::unordered_set<F>到std::string的偏特化
 * 
 * @tparam F 
 */
template<class F>
class TypeCast<std::unordered_set<F>, std::string>{
public:
    std::string operator()(const std::unordered_set<F>& vec)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;
        for (auto& v : vec) {
            node.push_back(YAML::Load(TypeCast<F, std::string>()(v)));
        }
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 针对std::string到std::map<std::string, T>的偏特化
 * 
 * @tparam T 基础类型
 */
template<class T>
class TypeCast<std::string, std::map<std::string, T>>{
public:
    std::map<std::string, T> operator()(const std::string& str)
    {
        std::map<std::string, T> map;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;

        for (auto  it = node.begin() ; it != node.end() ; ++it) {
            ss.str("");
            ss << it->second;
            map.insert(std::make_pair(it->first.Scalar(),
                TypeCast<std::string, T>()(ss.str())));
        }

        return map;
    }
};

/**
 * @brief 针对std::map<std::string, F>到std::string的偏特化
 * 
 * @tparam F 
 */
template<class F>
class TypeCast<std::map<std::string, F>, std::string>{
public:
    std::string operator()(const std::map<std::string, F>& map)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;

        for (auto it = map.begin() ; it != map.end() ; ++it) {
            node[it->first] = YAML::Load(TypeCast<F, std::string>()(it->second));
        }

        ss << node;
        return ss.str();
    }
};

/**
 * @brief 针对std::string到std::unordered_map<std::string, T>的偏特化
 * 
 * @tparam T 基础类型
 */
template<class T>
class TypeCast<std::string, std::unordered_map<std::string, T>>{
public:
    std::unordered_map<std::string, T> operator()(const std::string& str)
    {
        std::unordered_map<std::string, T> map;
        YAML::Node node = YAML::Load(str);
        std::stringstream ss;

        for (auto  it = node.begin() ; it != node.end() ; ++it) {
            ss.str("");
            ss << it->second;
            map.insert(std::make_pair(it->first.Scalar(),
                TypeCast<std::string, T>()(ss.str())));
        }

        return map;
    }
};

/**
 * @brief 针对std::unordered_map<std::string, F>到std::string的偏特化
 * 
 * @tparam F 
 */
template<class F>
class TypeCast<std::unordered_map<std::string, F>, std::string>{
public:
    std::string operator()(const std::unordered_map<std::string, F>& map)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;

        for (auto it = map.begin() ; it != map.end() ; ++it) {
            node[it->first] = YAML::Load(TypeCast<F, std::string>()(it->second));
        }

        ss << node;
        return ss.str();
    }
};

/**
 * @brief 高级配置类
 * 
 * @tparam T        需要配置的类
 * @tparam FromStr  仿函数类, 提供从字符串转换成类T的仿函数
 * @tparam T>
 * @tparam ToStr    仿函数类, 提供把类T转换成字符串的仿函数
 * @tparam std::string>
 */
template<class T, class FromStr = TypeCast<std::string, T>, class ToStr = TypeCast<T, std::string>>
class ConfigVar : public ConfigVarBase, public std::enable_shared_from_this<ConfigVar<T>> {
public:
    typedef std::shared_ptr<ConfigVar<T>> ptr;
    typedef std::function<void(const T& old_val, const T& new_val)> onChangeCallback;

    ConfigVar(const std::string& name, const T& value, const std::string& description = "")
    : ConfigVarBase(name, description) {
        m_value = value;
    }

    ~ConfigVar(){};

    std::string toString() override
    {
        try {
            /* code */
            //return boost::lexical_cast<std::string>(m_value);
            return ToStr()(m_value);
        } catch(std::exception& e) {
            ERROR() << "ConfigVar::toString Exception" << e.what() << " convert " << siem::TypeToName<T>() << " to string ";
            return "";
        }

    }

    bool fromString(const std::string& str) override
    {
        try {
            //m_value = boost::lexical_cast<T>(str);
            setValue(FromStr()(str));
        } catch (std::exception& e) {
            ERROR() << "ConfigVar::toString Exception :[ " << e.what() << " ] convert string to" << TypeToName<T>();
            return false;
        }

        return false;
    }

    /**
     * @brief 设置配置值
     * 
     * @param val 
     */
    void setValue(const T& val)
    {
        /*if (val == m_value) {
            return;
        } else {
            for(auto& i : m_cbs) {
                i.second(m_value, val);
            }
        }*/
        m_value = val;
    }

    /**
     * @brief 获取配置值
     * 
     * @return const T 
     */
    const T getValue(void) const { return m_value; }

    /**
     * @brief 获取配置类型
     * 
     * @return std::string 
     */
    std::string getTypeName() const { return TypeToName<T>(); }

    /**
     * @brief 添加监听函数
     * 
     * @param key   唯一key
     * @param cb    回调
     */
    void addListener(uint64_t key, onChangeCallback cb)
    {
        m_cbs[key] = cb;
    }

    /**
     * @brief 删除回调
     * 
     * @param key 唯一key
     */
    void delListener(uint64_t key)
    {
        if (m_cbs.find(key) != m_cbs.end()) {
            m_cbs.erase(key);
        }
    }

    /**
     * @brief 返回回调函数
     * 
     * @param key 唯一key
     * @return onChangeCallback 
     */
    onChangeCallback getListener(uint64_t key)
    {
        auto it = m_cbs.find(key);
        if (it != m_cbs.end()) {
            return it->second;
        }
        return nullptr;
    }

private:
    T m_value;
    std::map<uint64_t, onChangeCallback> m_cbs;
};

class Config{
public:
    typedef std::map<std::string, ConfigVarBase::ptr> ConfigVarMap;
    //using ConfigVarMap = std::map<std::string, ConfigVarBase::ptr>;

    /**
     * @brief 查找一个配置项, 假如不存在则创建一个
     * 
     * @tparam T            配置数据类
     * @param name          配置名称
     * @param value         配置数据
     * @param description   配置描述
     * @return ConfigVar<T>::ptr 
     */
    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string& name, const T& value, const std::string& description = "")
    {
        auto it = m_datas.find(name);
        if (it != m_datas.end()) {
            auto temp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (temp) {
                INFO() << "lookup name: " << name << " exists";
                return temp;
            } else {
                ERROR() << "lookup name: " << name << " exists and type is not " << TypeToName<T>() << " but " << it->second->getTypeName();
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ._0123456789") != std::string::npos) {
            ERROR() << "lookup name invalid";
            throw std::invalid_argument(name);
        }

        typename ConfigVar<T>::ptr v(new ConfigVar<T>(name, value, description));
        m_datas[name] = std::dynamic_pointer_cast<ConfigVarBase>(v);
        return v;

    }

    /**
     * @brief 查找一个数据配置项
     * 
     * @tparam T        配置数据类
     * @param name      配置名称
     * @return ConfigVar<T>::ptr 
     */
    template<class T>
    static typename ConfigVar<T>::ptr lookup(const std::string& name)
    {
        auto it = m_datas.find(name);
        if (it != m_datas.end()) {
            return std::dynamic_pointer_cast<ConfigVar<T> >(it->second);
        } else {
            return nullptr;
        }
    }

    /**
     * @brief 返回一个Base配置项
     * 
     * @param name 配置项名称
     * @return ConfigVarBase::ptr 
     */
    static ConfigVarBase::ptr lookUpBase(const std::string& name)
    {
        auto it = m_datas.find(name);
        return it != m_datas.end() ? it->second : nullptr;
    }

    /**
     * @brief 从YAML节点中加载配置
     * 
     * @param root YAML结点
     */
    static void loadFromYaml(const YAML::Node& root);

    /**
     * @brief 
     * 
     * @param path yaml文件路径
     */
    static void loadFromYaml(const std::string& path);

private:
    static ConfigVarMap m_datas;
};

/**
 * @brief 提供std::string到LogLevel::Level的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<std::string, LogLevel::Level>{
public:
    LogLevel::Level operator()(const std::string& str)
    {
        return LogLevel::getLevelByString(str);
    }
};

/**
 * @brief 提供LogLevel::Level到std::string的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<LogLevel::Level, std::string>{
public:
    std::string operator()(const LogLevel::Level& level)
    {
        return LogLevel::getStringByLevel(level);
    }
};

/**
 * @brief 提供std::string到LogFormatter::ptr的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<std::string, LogFormatter::ptr>{
public:
    LogFormatter::ptr operator()(const std::string& str)
    {
        LogFormatter::ptr p(new LogFormatter(str));

        return p;
    }
};

/**
 * @brief 提供LogFormatter::ptr到std::string的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<LogFormatter::ptr, std::string>{
public:
    std::string operator()(const LogFormatter::ptr& p)
    {
        return p->getPattern();
    }
};

/**
 * @brief 提供std::string到StdoutLogAppender::ptr的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<std::string, StdoutLogAppender::ptr>{
public:
    StdoutLogAppender::ptr operator()(const std::string& str)
    {
        YAML::Node node = YAML::Load(str);
        
        StdoutLogAppender::ptr p(new StdoutLogAppender());

        LogFormatter::ptr fp = TypeCast<std::string, LogFormatter::ptr>()(node["formatter"].Scalar());

        LogLevel::Level level = TypeCast<std::string, LogLevel::Level>()(node["level"].Scalar());

        p->setFormatter(fp);
        p->setLevel(level);

        return p;
    }
};

/**
 * @brief 提供StdoutLogAppender::ptr到std::string的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<StdoutLogAppender::ptr, std::string>{
public:
    std::string operator()(const StdoutLogAppender::ptr& app)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;

        node["level"] = TypeCast<LogLevel::Level, std::string>()(app->getLevel());
        node["formatter"] = TypeCast<LogFormatter::ptr, std::string>()(app->getFormatter());

        ss << node;
        return ss.str();
    }
};

/**
 * @brief 提供std::string到FileLogAppender::ptr的全特化
 * 
 * @tparam 
 */
template<>
class TypeCast<std::string, FileLogAppender::ptr>{
public:
    FileLogAppender::ptr operator()(const std::string& str)
    {
        YAML::Node node = YAML::Load(str);

        std::stringstream ss;

        ss << node["filename"];

        FileLogAppender::ptr p(new FileLogAppender(ss.str()));

        LogFormatter::ptr fp = TypeCast<std::string, LogFormatter::ptr>()(node["formatter"].Scalar());

        LogLevel::Level level = TypeCast<std::string, LogLevel::Level>()(node["level"].Scalar());

        p->setFormatter(fp);
        p->setLevel(level);

        return p;
    }
};

/**
 * @brief 提供FileLogAppender::ptr到std::string的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<FileLogAppender::ptr, std::string>{
public:
    std::string operator()(const FileLogAppender::ptr& app)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;

        node["filename"] = YAML::Load(std::string(app->getFilename()));
        node["level"] = TypeCast<LogLevel::Level, std::string>()(app->getLevel());
        node["formatter"] = TypeCast<LogFormatter::ptr, std::string>()(app->getFormatter());

        ss << node;
        return ss.str();
    }
};

/**
 * @brief 提供std::string到LogAppenderWrapper的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<std::string, LogAppenderWrapper>{
public:
    LogAppenderWrapper operator()(const std::string& str)
    {
        YAML::Node node = YAML::Load(str);

        LogAppenderWrapper wrapper;

        wrapper.setType(node["type"].Scalar());

        std::stringstream ss;

        ss << node["appender"];

        if (node["type"].Scalar() == "Stdout") {
            LogAppender::ptr p = TypeCast<std::string, StdoutLogAppender::ptr>()(ss.str());
            wrapper.setAppender(p);
        } else if (node["type"].Scalar() == "File") {
            LogAppender::ptr p = TypeCast<std::string, FileLogAppender::ptr>()(ss.str());
            wrapper.setAppender(p);
        }

        return wrapper;
    }
};

/**
 * @brief 提供LogAppenderWrapper到std::string的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<LogAppenderWrapper, std::string>{
public:
    std::string operator()(const LogAppenderWrapper& appwrap)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;

        node["type"] = appwrap.getType();
        if (appwrap.getType() == "Stdout") {
            auto it = std::dynamic_pointer_cast<StdoutLogAppender>(appwrap.getAppender());
            node["appender"] = YAML::Load(TypeCast<StdoutLogAppender::ptr, std::string>()(it));
        } else if (appwrap.getType() == "File") {
            auto it = std::dynamic_pointer_cast<FileLogAppender>(appwrap.getAppender());
            node["appender"] = YAML::Load(TypeCast<FileLogAppender::ptr, std::string>()(it));
        }
        ss << node;
        return ss.str();
    }
};

/**
 * @brief 提供std::string到Logger::ptr的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<std::string, Logger::ptr>{
public:
    Logger::ptr operator()(const std::string& str)
    {
        YAML::Node node = YAML::Load(str);
        Logger::ptr p = LoggerMgr::getInstance()->getLogger(node["name"].as<std::string>());
        std::stringstream ss;

        ss << node["appenders"];

        std::list<LogAppenderWrapper> list = TypeCast<std::string, std::list<LogAppenderWrapper>>()(
            ss.str()
        );

        for (auto& v : list) {
            p->addAppender(v.getAppender());
        }

        LogLevel::Level level = TypeCast<std::string, LogLevel::Level>()(node["level"].Scalar());

        p->setLevel(level);

        return p;
    }
};

/**
 * @brief 提供Logger::ptr到std::string的全特化
 * 
 * @tparam  
 */
template<>
class TypeCast<Logger::ptr, std::string>{
public:
    std::string operator()(const Logger::ptr& log)
    {
        YAML::Node node(YAML::NodeType::Sequence);
        std::stringstream ss;

        node["name"] = YAML::Load(log->getLoggerName());
        std::list<LogAppenderWrapper> appenders;

        for (LogAppender::ptr v : log->getAppenders()) {
            if (std::dynamic_pointer_cast<StdoutLogAppender>(v)) {
                appenders.push_back(LogAppenderWrapper(v, "Stdout"));
            } else if (std::dynamic_pointer_cast<FileLogAppender>(v)) {
                appenders.push_back(LogAppenderWrapper(v, "File"));
            }
        }

        node["appenders"] = YAML::Load(TypeCast<std::list<LogAppenderWrapper>, std::string>()(appenders));

        node["level"] = YAML::Load(TypeCast<LogLevel::Level, std::string>()(log->getLevel()));

        ss << node;
        return ss.str();
    }
};

}

#endif //__SIEM_CONFIGURATOR_H