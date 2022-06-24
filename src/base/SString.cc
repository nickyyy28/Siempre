#include "base/SString.h"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>

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
    m_cap = str.m_cap;
    m_size = str.m_size;
    m_data = new char[m_cap];
    std::memset(m_data, '\0', m_cap);
    std::memcpy(m_data, str.m_data, m_size);
}

SString::SString(const std::string& str)
{
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

    // char* tempData = new char[tempCap];

    // std::memcpy(tempData, m_data + pos, size);
    // // SString&& rval = SString(tempData, tempCap, size);
    // // return rval;

    // return std::forward<SString>(SString(tempData, tempCap, size));
}

SString::SString(char* data, size_t cap, size_t size)
{
    m_data = data;
    m_cap = cap;
    m_size = size;
}

SString& SString::replace(size_t pos, const SString& str)
{

}

SString& SString::replace(size_t pos, const std::string& str)
{

}

SString& SString::replace(size_t pos, const char* str, size_t length)
{

}

SString& SString::insert(size_t pos, const SString& str)
{

}

SString& SString::insert(size_t pos, const std::string& str)
{

}

SString& SString::insert(size_t pos, const char* str, size_t length)
{

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

SStringList::~SStringList()
{
    
}

}

