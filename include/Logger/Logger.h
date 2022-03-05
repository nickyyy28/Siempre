#pragma once

#include <iostream>
#include <string>
#include <strstream>
#include <sstream>
#include <ostream>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <vector>
#include <fstream>
#include <memory>
#include <map>
#include <functional>

namespace siempre {

class LogLevel;
class Logger;
class LogEvent;
class LogFormatter;
class LogAppender;

//日志级别
class LogLevel{
public:
    enum Level{
        DEBUG = 1,
        INFO,
        WRAN,
        ERROR,
        FATAL
    };

    static std::string getStringByLevel(Level level);

};

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;

private:
    std::string m_name;                 //日志名
    const char* m_filename = nullptr;   //文件名
    int32_t m_line = 0;                 //行号
    uint64_t m_elapse = 0;              //程序启动到现在的时间(ms)
    uint32_t m_thread_id = 0;           //线程id
    std::string m_thread_name;          //线程名
    uint32_t m_fiber_id = 0;            //协程id
    uint64_t m_time;                    //时间戳
    LogLevel::Level m_level;            //日志级别
    std::shared_ptr<Logger> m_logger;               //日志器
    std::stringstream m_sstream;        //信息

public:
    const char* getFile() const { return m_filename; }
    const std::string& getEventName() const { return m_name; }
    uint32_t getLine() const { return m_line; }
    uint64_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_thread_id; }
    const std::string& getThreadName() const { return m_thread_name; }
    uint32_t getFiberId() const { return m_fiber_id; }
    uint64_t getTime() const { return m_time; }
    void setMessage(std::string& msg);
    std::string getContent() const { return m_sstream.str(); }
    std::stringstream& getSStream() { return m_sstream; }

    LogEvent(std::string name, std::shared_ptr<Logger> logger, const char * filename, uint32_t line, uint64_t elapse, 
        uint32_t thread_id, const std::string thread_name,uint32_t fiber_id, uint64_t time, LogLevel::Level level);
    LogEvent();
    ~LogEvent();
};

//日志格式化器
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

public:
    class FormatItem{
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem() = default;
        FormatItem(const std::string& str) : m_str(str) {};
        virtual ~FormatItem(){};    
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    protected:
        std::string m_str;
    };

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error;

public:
    LogFormatter(const std::string& pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f:%l%T%m%n");
    ~LogFormatter();
    void addFormatItem(FormatItem::ptr item);
    void delFormatItem(FormatItem::ptr item);
    void init();
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    const std::string& getPattern() const { return this->m_pattern; }

};

class DateTimeFormatItem : public LogFormatter::FormatItem {
public:
    DateTimeFormatItem(const std::string& format = "%Y-%m-%d %H:%M:%S")
        :m_format(format) {
        if(m_format.empty()) {
            m_format = "%Y-%m-%d %H:%M:%S";
        }
    }

    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        struct tm tm;
        time_t time = event->getTime();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), m_format.c_str(), &tm);
        os << buf;
    }
private:
    std::string m_format;
};

class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string& str = "") : FormatItem(str) {};
    ~ThreadIdFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
}; 

class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str = "")
        : FormatItem(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_str;
    }
private:
};

class ElapseFormatItem : public LogFormatter::FormatItem{

public:
    ElapseFormatItem(const std::string& str) : FormatItem(str) {};
    ~ElapseFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
}; 

class BaseFormatItem : public LogFormatter::FormatItem{
public:
    BaseFormatItem(const std::string& str) : FormatItem(str) {};
    ~BaseFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string& str) : FormatItem(str) {};
    ~TabFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class ThreadNameFormatItem : public LogFormatter::FormatItem{
public:
    ThreadNameFormatItem(const std::string& str) : FormatItem(str) {};
    ~ThreadNameFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class FiberIDFormatItem : public LogFormatter::FormatItem{
public:
    FiberIDFormatItem(const std::string& str) : FormatItem(str) {};
    ~FiberIDFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string& str) : FormatItem(str) {};
    ~LevelFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string& str) : FormatItem(str) {};
    ~NameFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    FileNameFormatItem(const std::string& str) : FormatItem(str) {};
    ~FileNameFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class LineFormatItem : public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string& str) : FormatItem(str) {};
    ~LineFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string& str) : FormatItem(str) {};
    ~MessageFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string& str) : FormatItem(str) {};
    ~NewLineFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

//日志输出地
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;

protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;

public:
    LogAppender();
    void setLevel(LogLevel::Level level) { this->m_level = level; };
    void setFormatter(LogFormatter::ptr formatter) { this->m_formatter = formatter; };
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) = 0;
    virtual ~LogAppender();
};

//日志器
class Logger : public std::enable_shared_from_this<Logger> {
public:
    typedef std::shared_ptr<Logger> ptr;

private:
    std::string m_name;         //日志名称
    std::list<LogAppender::ptr> m_appenders;  //日志输出地列表
    LogLevel::Level m_level;    //日志级别， 高于某级别才会输出

public:
    Logger(const std::string &name = "root");
    void Log(LogLevel::Level level,LogEvent::ptr event);
    virtual ~Logger();
    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    void setLevel(LogLevel::Level level){ this->m_level = level; };
    LogLevel::Level getLevel(void) const {return this->m_level; };
    const std::string& getLoggerName(void) const { return m_name; };
    void debug(const LogEvent::ptr event);
    void info(const LogEvent::ptr event);
    void warn(const LogEvent::ptr event);
    void error(const LogEvent::ptr event);
    void fatal(const LogEvent::ptr event);
};

//输出到控制台
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    StdoutLogAppender();
    ~StdoutLogAppender();
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override ;
private:
};

//输出到文件
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    ~FileLogAppender();
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override ;
    bool reopen();
private:
    char* m_filename = nullptr;
    std::ofstream m_ofs;
};

}
