#ifndef __SIEM__SString_H
#define __SIEM__SString_H

#include <cstddef>
#include <string>
#include <iostream>
#include <vector>

#include <cmath>
#include <cstring>
#include <cstdint>
#include <cstdlib>

namespace siem{

class SString;
class SStringList;

std::ostream& operator<<(std::ostream& os, const SString& str);

class SString
{
    friend std::ostream& operator<<(std::ostream&, const SString&);
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

    /**
     * @brief 从指定位置裁剪一段字符串
     * 
     * @param pos 
     * @param size 
     * @return SString 
     */
    SString substr(size_t pos, size_t size);

    /**
     * @brief 
     * 
     * @param pos 
     * @param str 
     * @return SString& 
     */
    SString& replace(size_t pos, const SString& str);
    SString& replace(size_t pos, const std::string& str);

    SString& insert(size_t pos, const SString& str);
    SString& insert(size_t pos, const std::string& str);

    SStringList split(const SString& str);
    SStringList split(const std::string& str);

    bool contains(const SString& str);
    bool contains(const std::string& str);

    int find(const SString& str);
    int find(const std::string& str);

    bool startWith(const SString& str);
    bool startWith(const std::string& str);

    bool endWith(const SString& str);
    bool endWith(const std::string& str);

    SString toLowerCase();
    SString toUpperCase();

    static SString toString(int val);
    static SString toString(char val);
    static SString toString(short val);
    static SString toString(float val);
    static SString toString(double val);
    static SString toString(long val);

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
    ~SStringList();

    SString operator[](int);


private:
    std::vector<SString> m_strs;
};

}

#endif

