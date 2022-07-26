#pragma once

#include <cstdint>
#include <string>
namespace siem {

class TimeStamp{
public:
    explicit TimeStamp(int microSecondsSinceEpoch);
    ~TimeStamp();

    static TimeStamp now();
    std::string toString() const;

private:
    uint64_t m_microSecondsSinceEpoch;
};

}