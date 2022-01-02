#include "Logger/Logger.h"
#include "config/console_color.h"

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
    std::vector<std::tuple<std::string, std::string, int> > vec;
    size_t last_pos = 0;

    std::string nstr;

    for(size_t i = 0; i < m_pattern.size(); ++i){
        if (m_pattern[i] != '%')
        {
            nstr.append(1, this->m_pattern[i]);
            continue;
        } 

        if( i + 1 < m_pattern.size()){
            if (m_pattern[i + 1] == '%'){
                nstr.append(1, m_pattern[i + 1]);
            }
        }


        size_t n = i + 1;
        int fmt_status = 0;
        int fmt_begin = 0;

        std::string str;
        std::string fmt;

        while(n < this->m_pattern.size()){
            if(m_pattern[n] == ' '){
                break;
            }

            if (fmt_status == 0){
                if(m_pattern[n] == '{'){
                    str = m_pattern.substr(i + 1, n - i);
                    fmt_status = 1;
                    n++;
                    fmt_begin = n;
                    continue;

                }
            }

            if (fmt_status == 1){
                if(m_pattern[n] == '}'){
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    fmt_status = 2;
                    n++;
                    break;
                }
            }
            
        }
        
        if(fmt_status == 0){
            if(nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            str = m_pattern.substr(i + 1, n -i - 1);
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        } else if(fmt_status == 1){
            std::cout << "pattern parse error: " << m_pattern << "-" << m_pattern.substr(i) << std::endl;
        } else if(fmt_status == 2){
            if(nstr.empty()){
                vec.push_back(std::make_tuple(nstr, "", 0));
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n;
        }

        // if(i + 1 == m_pattern.size()){

        // }
    }

    if(!m_pattern.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

    // %T : Tab[\t]            TabFormatItem
    // %t : 线程id             ThreadIdFormatItem
    // %N : 线程名称           ThreadNameFormatItem
    // %F : 协程id             FiberIdFormatItem
    // %p : 日志级别           LevelFormatItem       
    // %c : 日志名称           NameFormatItem
    // %f : 文件名             FilenameFormatItem
    // %l : 行号               LineFormatItem
    // %m : 日志内容           MessageFormatItem
    // %n : 换行符[\r\n]       NewLineFormatItem

}

std::string LogFormatter::format(LogEvent::ptr event)
{
	std::strstream ss;
    for(auto& i : this->m_items){
        // std::cout << "执行了LogFormatter::format" << std::endl;
        i->format(ss, event);
    }

	return ss.str();
}

void LogFormatter::addFormatItem(FormatItem::ptr item)
{
    this->m_items.push_back(item);
}
void LogFormatter::delFormatItem(FormatItem::ptr item)
{
    for (auto it = this->m_items.begin() ; it != this->m_items.end() ; it++)
    {
        if (*it == item)
        {
            this->m_items.erase(it);
            break;
        }
        
    }
}

ThreadIdFormatItem::ThreadIdFormatItem()
{

}

ThreadIdFormatItem::~ThreadIdFormatItem()
{

}

void ThreadIdFormatItem::format(std::ostream& os, LogEvent::ptr event)
 {
    // std::cout << "执行了ThreadIdFormatItem::format" << std::endl;
    os << event.get()->getFile();
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
            // std::cout << "执行了Logger::Log" << std::endl;
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
    // std::cout << "执行了Logger::debug" << std::endl;
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
        // std::cout << "执行了StdoutLogAppender::Log" << std::endl;
        switch (level)
        {
        case LogLevel::DEBUG:
            std::cout << L_BLUE << "[DEBUG]: " << (this->m_formatter).get()->format(event) << BLUE << std::endl;
            break;
        case LogLevel::INFO:
            std::cout << L_GREEN << "[INFO ]: " << (this->m_formatter).get()->format(event) << GREEN << std::endl;
            break;
        case LogLevel::WRAN:
            std::cout << L_YELLOW << "[WARN ]: " << (this->m_formatter).get()->format(event) << YELLOW << std::endl;
            break;
        case LogLevel::ERROR:
            std::cout << L_RED << "[ERROR]: " << (this->m_formatter).get()->format(event) << RED << std::endl;
            break;
        case LogLevel::FATAL:
            std::cout << BL_RED << L_BLACK << "[FATAL]: " << (this->m_formatter).get()->format(event) << BLACK << B_RED << std::endl;
            break;
        default:
            std::cout << (this->m_formatter).get()->format(event) << std::endl;
            break;
        }
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
