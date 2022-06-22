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
     * @brief Ĭ�Ϲ���
     * 
     */
    SString();

    /**
     * @brief ��������
     * 
     * @param str 
     */
    SString(const SString& str);

    /**
     * @brief ��׼��std::string�Ŀ�������
     * 
     * @param str 
     */
    SString(const std::string& str);

    /**
     * @brief ��ֵ���ÿ�������
     * 
     * @param str 
     */
    SString(SString&& str);

    /**
     * @brief ����
     * 
     */
    ~SString();

    /**
     * @brief ���ں�����
     * 
     * @param str 
     * @return const SString& 
     */
    SString& operator =(const SString& str);

    /**
     * @brief ��ֵ���õ��ں�����
     * 
     * @param str 
     * @return SString& 
     */
    SString& operator =(SString&& str);

    /**
     * @brief +=����
     * 
     * @param str 
     * @return const SString& 
     */
    SString& operator +=(const SString& str);

    /**
     * @brief +������
     * 
     * @param str 
     * @return SString 
     */
    SString operator +(const SString& str);

    /**
     * @brief []����
     * 
     * @return char 
     */
    char operator [](int);

    /**
     * @brief ������c���ַ���
     * 
     * @return const char* 
     */
    const char* c_str() const;

    /**
     * @brief ��ȡ�ַ�����С
     * 
     * @return size_t 
     */
    size_t size() const;

    /**
     * @brief ��ȡ����
     * 
     * @return size_t 
     */
    size_t capacity() const;

    /**
     * @brief �ж��ַ����Ƿ�Ϊ��
     * 
     * @return true 
     * @return false 
     */
    bool empty() const;

    /**
     * @brief ��������ַ���
     * 
     */
    void clear();

    /**
     * @brief ���·���ռ�
     * 
     * @param newSize 
     */
    void resize(size_t newSize);

    /**
     * @brief �ַ���β׷��
     * 
     * @param newStr 
     * @return SString& 
     */
    SString& append(const SString& newStr);
    SString& append(const std::string& newStr);

    /**
     * @brief ��ָ��λ�òü�һ���ַ���
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
     * @brief �����´�С����������
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

