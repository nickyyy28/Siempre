#include "Logger/Logger.h"

namespace siempre{

LogEvent::LogEvent()
{

}

LogEvent::LogEvent(const char * name)
{
    this->m_name = name;
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

LogFormatter::LogFormatter(const std::string& pattern)
{
    this->m_pattern = pattern;
}

LogFormatter::~LogFormatter()
{

}

void LogFormatter::init()
{
    std::vector<std::pair<std::string, int>> vec;
    size_t last_pos = 0;

    for(size_t i = 0; i < m_pattern.size(); ++i){
        if (m_pattern[i] == '%')
        {
            
        }
        
    }
}

std::string LogFormatter::format(LogEvent::ptr event)
{
	std::strstream ss;
    for(auto& i : this->m_items){
        i->format(ss, event);
    }

	return ss.str();
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
    this->m_ofs.open(this->m_filename, std::fstream::app);
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

bool FileLogAppender::reopen()
{
    if (this->m_ofs.is_open())
    {
        this->m_ofs.flush();
        this->m_ofs.close();
    }

    this->m_ofs.open(this->m_filename, std::fstream::app);
    return !!this->m_ofs;
}

StdoutLogAppender::StdoutLogAppender()
{

}

StdoutLogAppender::~StdoutLogAppender()
{

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
    if (level >= this->m_level){
        this->m_ofs << this->m_formatter.get()->format(event);
        this->m_ofs.flush();
    }
}

}
