#pragma once

#include <vector>
#include <cxxabi.h>
#include <syscall.h>
#include <execinfo.h>
#include <sys/time.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <string>

namespace siem{

/**
 * @brief 通过abi将类型转化为字符串
 * 
 * @tparam T 
 * @return const char* 
 */
template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}

/**
 * @brief 获取当前线程pid
 * 
 * @return pid_t 
 */
pid_t getThreadID(void);

/**
 * @brief 获取程序栈调用
 * 
 * @param vec   保存栈的vector
 * @param size  调用栈深度
 * @param skip  跳过层数
 */
void BackTrace(std::vector<std::string>& vec, size_t size, int skip);

/**
 * @brief 获取程序栈调用到字符串
 * 
 * @param size 调用栈深度
 * @param skip 
 * @return std::string 
 */
std::string BackTraceToString(size_t size, int skip);

/**
 * @brief 获取当前协程ID
 * 
 * @return uint64_t 
 */
uint64_t getFiberID(void);

}