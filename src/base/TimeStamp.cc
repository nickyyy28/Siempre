#include "base/TimeStamp.h"
#include <cstddef>
#include <ctime>
#include <sstream>

namespace siem {

TimeStamp::TimeStamp(int microSecondsSinceEpoch) : m_microSecondsSinceEpoch(microSecondsSinceEpoch)
{

}

TimeStamp::~TimeStamp()
{
}

TimeStamp TimeStamp::now()
{
    return TimeStamp(time(NULL));
}

std::string TimeStamp::toString() const 
{
    std::stringstream ss;
    struct tm* time_value = std::localtime(reinterpret_cast<const time_t*>(&m_microSecondsSinceEpoch));

    ss << time_value->tm_year + 1900 << "-" << 
        time_value->tm_mon + 1 << "-" <<
        time_value->tm_mday << "\t" <<
        time_value->tm_hour << ":" <<
        time_value->tm_min << ":" <<
        time_value->tm_sec;

    return ss.str();
}

}