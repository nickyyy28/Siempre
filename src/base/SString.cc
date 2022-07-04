#include "base/SString.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "Logger/Logger.h"
#include "Thread/Thread.h"
#include "utils/utils.h"

namespace siem {

std::ostream& operator<<(std::ostream& os, const SString& str)
{
    os << str.m_data;
    return os;
}

std::istream& operator>>(std::istream& is, SString& str)
{
    std::stringstream ss;

    char ch;
    while (is.get(ch)) {
        if (ch != '\n' && ch != '\0' && ch != EOF) {
            ss.put(ch);
        } else {
            break;
        }
    }

    str.append(ss.str());

    return is;
}

size_t SString::calculateCapacity(size_t size)
{
    return pow(2, ceil(log2(size)));
}

SString::SString()
    : m_cap(128)
    , m_size(0) {
    m_data = new char[m_cap];
    std::memset(m_data, '\0', m_cap);
}

SString::SString(const SString& str)
{
    // std::cout << "SString::SString(const SString& str)" << std::endl;
    m_cap = str.m_cap;
    m_size = str.m_size;
    m_data = new char[m_cap];
    std::memset(m_data, '\0', m_cap);
    std::memcpy(m_data, str.m_data, m_size);
}

SString::SString(const std::string& str)
{
    // std::cout << "SString::SString(const std::string& str)" << std::endl;
    size_t size = str.size();
    size_t newSize = calculateCapacity(size);
    if (newSize <= 128) newSize = 128;

    m_data = new char[newSize];
    std::memset(m_data, '\0', newSize);
    m_cap = newSize;
    m_size = size;

    std::memcpy(m_data, str.c_str(), size);
}

SString::SString(SString&& str)
{
    // std::cout << "SString::SString(SString&& str)" <<std::endl;
    m_cap = str.m_cap;
    m_size = str.m_size;
    m_data = str.m_data;
    str.m_data = nullptr;

}

SString::~SString()
{
    if (m_data != nullptr) {
        delete[] m_data;
        m_data = nullptr;
    }
}

SString& SString::operator=(const SString& str)
{
    // std::cout << "SString& SString::operator=(const SString& str)" << std::endl;
    if (this->m_data != nullptr && str.m_data != nullptr && this->m_data == str.m_data) {
        return *this;
    }

    if (m_cap >= str.size()) {
        m_size = str.m_size;
        std::memset(m_data, '\0', m_cap);
    } else {
        if (m_data){
            delete[] m_data;
            m_data = nullptr;
        }

        m_cap = str.m_cap;
        m_size = str.m_size;
        m_data = new char[m_cap];
        std::memset(m_data, '\0', m_cap);
    }

    std::memcpy(m_data, str.m_data, m_size);
    return *this;
}

SString& SString::operator=(SString&& str)
{
    // std::cout << "SString& SString::operator=(SString&& str)" << std::endl;
    delete [] m_data;
    m_data = str.m_data;
    str.m_data = nullptr;
    m_cap = str.m_cap;
    m_size = str.m_size;

    return *this;
}

SString& SString::operator+=(const SString& str)
{
    size_t size = str.size();

    size_t newCap = calculateCapacity(size + m_size);

    if (newCap > m_cap) {
        char* newData = new char[newCap];
        std::memset(newData, '\0', newCap);

        std::memcpy(newData, m_data, m_size);
        std::memcpy(newData + m_size, str.m_data, str.size());

        delete [] m_data;
        m_data = newData;
        m_cap = newCap;
    } else {
        std::memcpy(m_data + m_size, str.m_data, str.size());
    }

    m_size = size + m_size;

    return *this;
}

SString SString::operator+(const SString &str)
{
    SString tempStr(*this);
    tempStr += str;
    return tempStr;
}

char SString::operator[](int pos)
{
    if (pos >= m_size) {
        throw std::invalid_argument("the pos out of bound");
        std::exit(-1);
    }

    return m_data[pos];
}

const char* SString::c_str() const
{
    return m_data;
}

size_t SString::size() const
{
    return m_size;
}

size_t SString::capacity() const
{
    return m_cap;
}

void SString::clear()
{
    std::memset(m_data, '\0', m_cap);
    m_size = 0;
}

bool SString::empty() const
{
    return (m_size == 0);
}

void SString::resize(size_t newSize)
{
    char* newData = new char[newSize];
    size_t newCap = newSize;

    std::memset(newData, '\0', newSize);
    if (m_size <= newSize) {
        std::memcpy(newData, m_data, m_size);
    } else {
        std::memcpy(newData, m_data, newSize);
        m_size = newSize;
    }

    m_cap = newCap;

    delete [] m_data;
    m_data = newData;
}

SString& SString::append(const SString& newStr)
{
    return append(newStr.c_str(), newStr.size());
}

SString& SString::append(const std::string& newStr)
{
    return append(newStr.data(), newStr.size());
}

SString& SString::append(const char* data, size_t length)
{
    if (m_size + length > m_cap) {
        resize(calculateCapacity(m_size + length));
    }

    std::memcpy(m_data + m_size, data, length);

    m_size += length;

    return *this;
}

SString SString::substr(size_t pos, size_t size)
{
    if (pos > m_size) {
        throw std::invalid_argument("the pos out of bound");
    } else if (pos + size > m_size) {
        throw std::invalid_argument("the size out of bound");
    }

    size_t tempCap = calculateCapacity(size);
    if (tempCap < 128) tempCap = 128;

    SString tempStr;

    tempStr.resize(tempCap);
    tempStr.m_size = size;

    std::memcpy(tempStr.m_data, m_data + pos, size);

    return tempStr;
}

SString::SString(char* data, size_t cap, size_t size)
{
    m_data = data;
    m_cap = cap;
    m_size = size;
}

SString& SString::replace(size_t pos, const SString& str)
{
    return replace(pos, str.m_data, str.size());
}

SString& SString::replace(size_t pos, const std::string& str)
{
    return replace(pos, str.c_str(), str.size());
}

SString& SString::replace(size_t pos, const char* str, size_t length)
{
    if(pos + length > m_size) {
        throw std::invalid_argument("the length out of bound");
    }

    if (pos + length > m_cap) {
        resize(calculateCapacity(pos + length));
    }

    std::memcpy(m_data + pos, str, length);

    return *this;
}

SString& SString::insert(size_t pos, const SString& str)
{
    return insert(pos, str.c_str(), str.size());
}

SString& SString::insert(size_t pos, const std::string& str)
{
    return insert(pos, str.c_str(), str.size());
}

SString& SString::insert(size_t pos, const char* str, size_t length)
{
    if(pos > m_size) {
        throw std::invalid_argument("the pos out of bound");
    }

    int newSize = size() + length;

    int newCap = calculateCapacity(newSize);
    if (newCap > capacity()) {
        resize(newCap);
    }

    size_t needMoveSize = size() - pos; //需要移动几个
    int offset = length;                //向右移动几个字节
    char * base = m_data + pos;         //数据基地址

    for (int i = 1 ; i <= needMoveSize ; ++i) {
        *(base + offset + (needMoveSize - i)) = *(base + (needMoveSize - i));
    }

    std::memcpy(m_data + pos, str, length);

    m_size = newSize;

    return *this;
}

SStringList SString::split(const SString& str)
{
    SStringList sl;

    return sl;
}

SStringList SString::split(const std::string& str)
{
    SStringList sl;

    return sl;
}

SStringList SString::split(const char* str, size_t length)
{

    SStringList sl;

    return sl;
}

SStringList SString::split(char ch)
{

    SStringList sl;
    std::deque<size_t> indexs;
    std::vector<std::pair<size_t, size_t>> strs;

    size_t index = 0;
    size_t last_index = 0;

    for (; index < size() ; ++index) {
        if (*(m_data + index) == ch) {
            // indexs.push_back(index);
            indexs.push_back(index);
        }
    }

    if (indexs.size() == 0) {
        sl.addString(*this);
    }

    while (indexs.size() != 0) {
        index = indexs.front();

        if (index == last_index) {
 
            continue; 
        }
    }


    return sl;
}

bool SString::contains(const SString& str)
{
    return false;
}

bool SString::contains(const std::string& str)
{
    return false;
}

SStringList::SStringList()
{

}

SStringList::SStringList(SStringList &&list)
{
    for (auto it = list.begin() ; it != list.end() ; ++it) {
        m_strs.push_back(std::move(*it));
    }
}

SStringList::~SStringList()
{

}

SString SStringList::operator[](int index)
{
    return m_strs[index];
}

size_t SStringList::size() const
{
    return m_strs.size();
}

std::vector<SString>::iterator SStringList::begin()
{
    return m_strs.begin();
}

std::vector<SString>::iterator SStringList::end()
{
    return m_strs.end();
}

}

