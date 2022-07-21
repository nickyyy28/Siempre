#pragma once

#include <cstddef>
#include <istream>
#include <string>
#include <iostream>
#include <type_traits>
#include <vector>
#include <sstream>

#include <cmath>
#include <cstring>
#include <cstdint>
#include <cstdlib>

#include "../utils/utils.h"

#define DEBUG_STR(__str__) \
    siem::debug_show_str(__str__)



namespace siem{

class SString;
class SStringList;

/**
 * @brief 输出流重载
 * 
 * @param os 
 * @param str 
 * @return std::ostream& 
 */
std::ostream& operator<<(std::ostream& os, const SString& str);

/**
 * @brief 输入流重载
 * 
 * @param is 
 * @param str 
 * @return std::istream& 
 */
std::istream& operator>>(std::istream& is, SString& str);

class SString
{
    friend std::ostream& operator<<(std::ostream&, const SString&);
    friend std::istream& operator>>(std::istream&, SString&);
public:

    /**
     * @brief 默认构造
     * 
     */
    SString();

    /**
     * @brief 拷贝构造
     * 
     * @param str 
     */
    SString(const SString& str);

    /**
     * @brief 标准库std::string的拷贝构造
     * 
     * @param str 
     */
    SString(const std::string& str);

    /**
     * @brief 右值引用拷贝构造
     * 
     * @param str 
     */
    SString(SString&& str);

    /**
     * @brief 析构
     * 
     */
    ~SString();

    /**
     * @brief 等于号重载
     * 
     * @param str 
     * @return const SString& 
     */
    SString& operator =(const SString& str);

    /**
     * @brief 右值引用等于号重载
     * 
     * @param str 
     * @return SString& 
     */
    SString& operator =(SString&& str);

    /**
     * @brief +=重载
     * 
     * @param str 
     * @return const SString& 
     */
    SString& operator +=(const SString& str);

    /**
     * @brief +号重载
     * 
     * @param str 
     * @return SString
     */
    SString operator +(const SString& str);

    /**
     * @brief []重载
     * 
     * @return char 
     */
    char operator [](int);

    /**
     * @brief 返回类c的字符串
     * 
     * @return const char* 
     */
    const char* c_str() const;

    /**
     * @brief 获取字符串大小
     * 
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief 获取容量
     * 
     * @return size_t 
     */
    size_t capacity() const;

    /**
     * @brief 判断字符串是否为空
     * 
     * @return true 
     * @return false 
     */
    bool empty() const;

    /**
     * @brief 清除所有字符串
     * 
     */
    void clear();

    /**
     * @brief 重新分配空间
     * 
     * @param newSize 
     */
    void resize(size_t newSize);

    /**
     * @brief 字符串尾追加
     * 
     * @param newStr 
     * @return SString& 
     */
    SString& append(const SString& newStr);
    SString& append(const std::string& newStr);
    SString& append(const char* data, size_t length = 1);

    /**
     * @brief 从指定位置裁剪一段字符串
     * 
     * @param pos 
     * @param size 
     * @return SString 
     */
    SString substr(size_t pos, size_t size);

    /**
     * @brief 从指定位置替换
     * 
     * @param pos 
     * @param str 
     * @return SString& 
     */
    SString& replace(size_t pos, const SString& str);
    SString& replace(size_t pos, const std::string& str);
    SString& replace(size_t pos, const char* str, size_t length);

    /**
     * @brief 从指定位置插入
     * 
     * @param pos 
     * @param str 
     * @return SString& 
     */
    SString& insert(size_t pos, const SString& str);
    SString& insert(size_t pos, const std::string& str);
    SString& insert(size_t pos, const char* str, size_t length);

    /**
     * @brief 分割字符串
     * 
     * @param str 
     * @return SStringList 
     */
    SStringList split(const SString& str);
    SStringList split(const std::string& str);
    SStringList split(const char* str, size_t length);
    SStringList split(char ch);

    /**
     * @brief 查找是否存在字符串
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    bool contains(const SString& str);
    bool contains(const std::string& str);
    bool contains(const char* str, size_t length);

    /**
     * @brief 查找字符串并返回位置
     * 
     * @param str 
     * @return int 
     */
    int find(const SString& str);
    int find(const std::string& str);

    /**
     * @brief 是否以某字符串开头
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    bool startWith(const SString& str);
    bool startWith(const std::string& str);

    /**
     * @brief 是否以某字符串结尾
     * 
     * @param str 
     * @return true 
     * @return false 
     */
    bool endWith(const SString& str);
    bool endWith(const std::string& str);

    /**
     * @brief 转换为小写字符
     * 
     * @return SString 
     */
    SString& toLowerCase();

    /**
     * @brief 转换为大写字符
     * 
     * @return SString& 
     */
    SString& toUpperCase();

    template <class T>
    SString& operator<<(T var)
    {
        // std::cout << "SString& operator<<(" << TypeToName<T>() << " var)" << std::endl;
        std::stringstream ss;
        ss << var;
        append(ss.str());
        return *this;
    }

    template<class T>
    static SString toString(T var)
    {
        std::stringstream ss;
        ss << var;
        return SString(ss.str());
    }

    template<class T>
    static T toVariable(const SString& str)
    {
        std::stringstream ss;
        ss << str.m_data;
        T var;
        ss >> var;
        return var;
    }

    static char toChar(const SString& str);
    static short toShort(const SString& str);
    static int toInt(const SString& str);
    static float toFloat(const SString& str);
    static double toDouble(const SString& str);
    static long toLong(const SString& str);

private:

    /**
     * @brief 根据新大小计算新容量
     * 
     * @param size 
     * @return size_t 
     */
    static size_t calculateCapacity(size_t size);

    SString(char* data, size_t cap, size_t size);

    char* m_data;
    size_t m_cap;
    size_t m_size;
};

class SStringList{
public:
    SStringList();
    SStringList(SStringList &&list);
    ~SStringList();

    SString operator[](int);

    size_t size() const;

    template <class T>
    void addString(T &&val)
    {
        m_strs.push_back(std::forward<T>(val));
    }

    void addString(const char* val)
    {
        m_strs.push_back(std::string(val));
    }

    std::vector<SString>::iterator begin();
    std::vector<SString>::iterator end();

private:
    std::vector<SString> m_strs;
};

}
