#include "Logger/Logger.h"

namespace siempre{

LogEvent::LogEvent()
{

}

LogEvent::LogEvent(const char * filename)
{
    this->m_filename = filename;
}

LogEvent::~LogEvent()
{

}

LogAppender::LogAppender()
{

}

LogAppender::~LogAppender()
{

}

LogFormatter::LogFormatter()
{

}

LogFormatter::~LogFormatter()
{

}

std::string LogFormatter::format(LogEvent::ptr event)
{

}

Logger::Logger(const std::string &name)
{

}

Logger::~Logger()
{

}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event)
{
    if (level >= this->m_level)
    {
        for(auto &i : this->m_appenders){
            i->Log(level, event);
        }
    }
    
}

void Logger::addAppender(LogAppender::ptr appender)
{
    this->m_appenders.push_back(appender);
}

void Logger::delAppender(LogAppender::ptr appender)
{
    for (auto it = this->m_appenders.begin() ; it != this->m_appenders.end() ; it++)
    {
        if (*it == appender)
        {
            this->m_appenders.erase(it);
            break;
        }
        
    }
    
}

void Logger::debug(const LogEvent::ptr event)
{
    Log(LogLevel::DEBUG, event);
}

void Logger::info(const LogEvent::ptr event)
{
    Log(LogLevel::INFO, event);
}

void Logger::warn(const LogEvent::ptr event)
{
    Log(LogLevel::WRAN, event);
}

void Logger::error(const LogEvent::ptr event)
{
    Log(LogLevel::ERROR, event);
}

void Logger::fatal(const LogEvent::ptr event)
{
    Log(LogLevel::FATAL, event);
}

FileLogAppender::FileLogAppender(const std::string& filename)
{
    this->m_filename = (char *)filename.c_str();
    this->m_ofs.open(this->m_filename);
    if(!this->m_ofs.is_open()){
        throw "Open File Fial";
    }
}

FileLogAppender::~FileLogAppender()
{
    if (this->m_ofs.is_open())
    {
        this->m_ofs.close();
    }
    
}

void StdoutLogAppender::Log(LogLevel::Level level,LogEvent::ptr event)
{
    if (level >= this->m_level)
    {
        std::cout << (this->m_formatter).get()->format(event);
    }
    
}

void FileLogAppender::Log(LogLevel::Level level,LogEvent::ptr event)
{

}

}
