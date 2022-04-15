#include "Logger/Logger.h"
#include "Logger/console_color.h"

#include "Configurator/Configurator.h"

namespace siem{

static LoggerManager* Mgr = LoggerMgr::getInstance();
static Logger::ptr root_logger = GET_LOG_ROOT();
static Logger::ptr syslog = GET_LOG_BY_NAME(system);

unsigned long long getThisThreadID()
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();

    return std::stoull(std::string(oss.str()));
}

std::string LogLevel::getStringByLevel(Level level)
{
#define XX(name)    \
    case name:      \
    return #name;
    switch (level)
    {
        XX(DEBUG);
        XX(INFO);
        XX(WARN);
        XX(ERROR);
        XX(FATAL);
        default:
            return "UNKOWN";
    }
#undef XX
}

LogLevel::Level LogLevel::getLevelByString(const std::string& str)
{
#define XX(_type_)  \
    if (#_type_ == str) { \
        return LogLevel::_type_; \
    }

    XX(DEBUG)
    XX(INFO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
    return LogLevel::UNKNOW;
}

LogEvent::LogEvent()
{

}

LogEvent::LogEvent(std::string name, std::shared_ptr<Logger> logger, const char * filename, uint32_t line, uint64_t elapse, 
        uint32_t thread_id, const std::string thread_name,uint32_t fiber_id, uint64_t time, LogLevel::Level level)
{
    this->m_name = name;
    this->m_line = line;
    this->m_filename = filename;
    this->m_elapse = elapse;
    this->m_thread_id = thread_id;
    this->m_thread_name = thread_name;
    this->m_fiber_id = fiber_id;
    this->m_time = time;
    this->m_level = level;
    this->m_logger = logger;
}

void LogEvent::setMessage(std::string& msg)
{
    msg.clear();
    this->m_sstream << msg;
}

LogEvent::~LogEvent()
{

}

void LogEvent::format(const char* fmt, ...) {
    va_list al;
    va_start(al, fmt);
    format(fmt, al);
    va_end(al);
}

void LogEvent::format(const char* fmt, va_list al) {
    char* buf = nullptr;
    int len = vasprintf(&buf, fmt, al);
    if(len != -1) {
        m_sstream << std::string(buf, len);
        free(buf);
    }
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

    init();
}

LogFormatter::~LogFormatter()
{

}

void LogFormatter::init()
{
    std::vector<std::tuple<std::string, std::string, int> > vec;
    std::string nstr;
    for(size_t i = 0; i < m_pattern.size(); ++i) {
        if(m_pattern[i] != '%') {
            nstr.append(1, m_pattern[i]);
            continue;
        }

        if((i + 1) < m_pattern.size()) {
            if(m_pattern[i + 1] == '%') {
                nstr.append(1, '%');
                continue;
            }
        }

        size_t n = i + 1;
        int fmt_status = 0;
        size_t fmt_begin = 0;

        std::string str;
        std::string fmt;
        while(n < m_pattern.size()) {
            if(!fmt_status && (!isalpha(m_pattern[n]) && m_pattern[n] != '{'
                    && m_pattern[n] != '}')) {
                str = m_pattern.substr(i + 1, n - i - 1);
                break;
            }
            if(fmt_status == 0) {
                if(m_pattern[n] == '{') {
                    str = m_pattern.substr(i + 1, n - i - 1);
                    //std::cout << "*" << str << std::endl;
                    fmt_status = 1; //解析格式
                    fmt_begin = n;
                    ++n;
                    continue;
                }
            } else if(fmt_status == 1) {
                if(m_pattern[n] == '}') {
                    fmt = m_pattern.substr(fmt_begin + 1, n - fmt_begin - 1);
                    //std::cout << "#" << fmt << std::endl;
                    fmt_status = 0;
                    ++n;
                    break;
                }
            }
            ++n;
            if(n == m_pattern.size()) {
                if(str.empty()) {
                    str = m_pattern.substr(i + 1);
                }
            }
        }

        if(fmt_status == 0) {
            if(!nstr.empty()) {
                vec.push_back(std::make_tuple(nstr, std::string(), 0));
                nstr.clear();
            }
            vec.push_back(std::make_tuple(str, fmt, 1));
            i = n - 1;
        } else if(fmt_status == 1) {
            std::cout << "pattern parse error: " << m_pattern << " - " << m_pattern.substr(i) << std::endl;
            m_error = true;
            vec.push_back(std::make_tuple("<<pattern_error>>", fmt, 0));
        }
    }

    if(!nstr.empty()) {
        vec.push_back(std::make_tuple(nstr, "", 0));
    }
    static std::map<std::string, std::function<FormatItem::ptr(const std::string& str)>> s_format_items = {
#define XX(str, C) \
        {#str, [](const std::string& fmt) { return FormatItem::ptr(new C(fmt));}}

        XX(m, MessageFormatItem),           //m:消息
        XX(p, LevelFormatItem),             //p:日志级别
        XX(r, ElapseFormatItem),            //r:累计毫秒数
        XX(c, NameFormatItem),              //c:日志名称
        XX(t, ThreadIdFormatItem),          //t:线程id
        XX(n, NewLineFormatItem),           //n:换行
        XX(d, DateTimeFormatItem),          //d:时间
        XX(f, FileNameFormatItem),          //f:文件名
        XX(l, LineFormatItem),              //l:行号
        XX(T, TabFormatItem),               //T:Tab
        XX(F, FiberIDFormatItem),           //F:协程id
        XX(N, ThreadNameFormatItem),        //N:线程名称
#undef XX
    };

    for(auto& i : vec) {
        if(std::get<2>(i) == 0) {
            m_items.push_back(FormatItem::ptr(new StringFormatItem(std::get<0>(i))));
        } else {
            auto it = s_format_items.find(std::get<0>(i));
            if(it == s_format_items.end()) {
                m_items.push_back(FormatItem::ptr(new StringFormatItem("<<error_format %" + std::get<0>(i) + ">>")));
                m_error = true;
            } else {
                m_items.push_back(it->second(std::get<1>(i)));
            }
        }

        //std::cout << "(" << std::get<0>(i) << ") - (" << std::get<1>(i) << ") - (" << std::get<2>(i) << ")" << std::endl;
    }
    //std::cout << m_items.size() << std::endl;

    if(!m_pattern.empty()){
        vec.push_back(std::make_tuple(nstr, "", 0));
    }

}

std::string LogFormatter::format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
	std::stringstream ss;
    for(auto& i : this->m_items){
        // std::cout << "执行了LogFormatter::format" << std::endl;
        i->format(ss, logger, level,event);
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

void ThreadIdFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
 {
    os << event.get()->getThreadId();
}

void TabFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << "\t";
}

void ThreadNameFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    //printf("len : %d\n", event->getThreadName().size());
    os << event.get()->getThreadName().c_str();
}

void FiberIDFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << event.get()->getFiberId();
}

void LevelFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << LogLevel::getStringByLevel(level);
}

void FileNameFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << "\"" << event.get()->getFile() << "\"";
}

void LineFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << event.get()->getLine();
}

void MessageFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << "\"" << event.get()->getContent() << "\"";
}

void NewLineFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << "\r\n";
}

void NameFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << event.get()->getEventName();
}

void ElapseFormatItem::format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event)
{
    os << event.get()->getElapse();
}

Logger::Logger(const std::string &name)
{
    m_name = name;
}

Logger::~Logger()
{

}

void Logger::Log(LogLevel::Level level, LogEvent::ptr event)
{
    typename Mutex::Lock loglock(m_mutex);
    if (level >= this->m_level)
    {
        for(auto &i : this->m_appenders){
            i->Log(shared_from_this(), level, event);
        }
    }
}

void Logger::addAppender(LogAppender::ptr appender)
{
    this->m_appenders.push_back(appender);
}

void Logger::setFormatter(const std::string& fmt)
{
    Mutex::Lock lock(m_mutex);
    for (auto& v : m_appenders) {
        v->setFormatter(LogFormatter::ptr(new LogFormatter(fmt)));
    }
}

void Logger::addAppender(const std::list<LogAppender::ptr>& list)
{
    Mutex::Lock lock(m_mutex);
    for (auto& v : list) {
        Mutex::Lock appLock(v->getMutex());
        addAppender(v);
    }
}

void Logger::delAppender(LogAppender::ptr appender)
{
    Mutex::Lock delLock(m_mutex);
    for (auto it = this->m_appenders.begin() ; it != this->m_appenders.end() ; it++)
    {
        if (*it == appender)
        {
            Mutex::Lock applock(appender->getMutex());
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
    Log(LogLevel::WARN, event);
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
    this->m_filename = filename;
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

void FileLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event)
{
    if (level >= this->m_level){
        this->m_ofs << this->m_formatter.get()->format(logger, level, event) << std::endl;
        this->m_ofs.flush();
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

void StdoutLogAppender::Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event)
{
    if (level >= this->m_level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            std::cout << L_BLUE << (this->m_formatter).get()->format(logger, level, event) << NONE << std::endl;
            break;
        case LogLevel::INFO:
            std::cout << L_GREEN << (this->m_formatter).get()->format(logger, level, event) << NONE << std::endl;
            break;
        case LogLevel::WARN:
            std::cout << L_YELLOW << (this->m_formatter).get()->format(logger, level, event) << NONE << std::endl;
            break;
        case LogLevel::ERROR:
            std::cout << L_RED << (this->m_formatter).get()->format(logger, level, event) << NONE << std::endl;
            break;
        case LogLevel::FATAL:
            std::cout << BL_RED << (this->m_formatter).get()->format(logger, level, event) << NONE << std::endl;
            break;
        default:
            std::cout << (this->m_formatter).get()->format(logger, level, event) << std::endl;
            break;
        }
    }

    std::cout.flush();

}

LoggerManager::LoggerManager()
{
    Logger::ptr p(new Logger("root"));
    m_root = p;
    m_root->addAppender(LogAppender::ptr(new StdoutLogAppender));

    for ( auto appender : m_root->getAppenders()) {
        appender->setFormatter(LogFormatter::ptr(new LogFormatter("[%p%T%d] <thread: %t>\nFile: %f, Line: %l\nMessage: %m")));
    }

    m_loggerMap[m_root->m_name] = m_root;

}

LoggerManager::~LoggerManager()
{

}

Logger::ptr LoggerManager::getLogger(const std::string& loggerName)
{
    auto it = m_loggerMap.find(loggerName);
    if (it != m_loggerMap.end()) {
        return it->second;
    } else {
        Logger::ptr logger(new Logger(loggerName));
        logger->addAppender(LogAppender::ptr(new StdoutLogAppender()));
        m_loggerMap[loggerName] = logger;
        logger->setFormatter("[%p %d] <thread: %t>\nFile: %f, Line: %l\n<%c>: %m");
        return logger;
    }
}

Logger::ptr LoggerManager::getRoot()
{
    return m_root;
}

void LoggerManager::setRootFormat(const std::string& format)
{
    for ( auto appender : m_root->getAppenders()) {
        appender->setFormatter(LogFormatter::ptr(new LogFormatter(format)));
    }
}

void LoggerManager::addRootFileAppender(const std::string& filePath)
{
    if (filePath.empty()) {
        return;
    }

    m_root->addAppender(LogAppender::ptr(new FileLogAppender(filePath)));
}

LogEventWrapper::LogEventWrapper(LogEvent::ptr e) : event(e)
{
}

LogEventWrapper::~LogEventWrapper()
{
    event->getLogger()->Log(event->getLevel(), event);
}

std::stringstream& LogEventWrapper::getSS()
{
    return event->getSStream();
}

}
