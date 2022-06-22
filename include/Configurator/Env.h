#ifndef __SIEM_ENV_H
#define __SIEM_ENV_H

#include "Logger/Logger.h"
#include "Thread/Thread.h"
#include "common/singleton.h"

#include <map>
#include <list>

#define GET_GLOBAL_ENV() \
    (Global_Env)

/**
 * @brief 获取命令行参数解析对象指针
 * 
 */
#define GET_ENV()   \
    (siem::SingleTonPtr<Env>::getInstancePtr())

namespace siem{

/**
 * @brief 环境变量类
 * 
 */
class Env : public NoCopyAble{
public:
    typedef std::shared_ptr<Env> ptr;
    typedef std::map<std::string, std::string> ArgMap;
    typedef std::vector<std::pair<std::string, std::string>> ArgHelp;

    Env();
    ~Env();

    /**
     * @brief 解析命令行参数
     * 
     * @param argc 参数数量
     * @param argv 参数数组
     */
    void init(int argc, char** argv);

    /**
     * @brief 添加配置参数
     * 
     * @param key   键
     * @param value 值
     */
    void add(const std::string& key, const std::string& value);

    /**
     * @brief 删除配置参数
     * 
     * @param key 键
     */
    void del(const std::string& key);

    /**
     * @brief 判断是否有该键值
     * 
     * @param key   键
     * @return true 
     * @return false 
     */
    bool has(const std::string& key);

    /**
     * @brief 根据键获取值
     * 
     * @param key 
     * @param val 
     * @return const std::string& 
     */
    const std::string& get(const std::string& key, const std::string& val = "");

    /**
     * @brief 添加帮助信息
     * 
     * @param key   键
     * @param help  键的帮助信息
     */
    void addHelp(const std::string& key, const std::string& help);

    /**
     * @brief 删除帮助信息
     * 
     * @param key 键
     */
    void delHelp(const std::string& key);

    /**
     * @brief 打印所有帮助信息
     * 
     */
    void printHelp(void);

    /**
     * @brief 打印所有解析的命令行参数
     * 
     */
    void printArgs(void);

private:
    RWMutex m_mutex;
    ArgMap m_args;
    ArgHelp m_helps;

    std::string processName;
};

extern Env::ptr Global_Env;

}

#endif //__SIEM_ENV_H