#pragma once

#include <iostream>
#include <string>
#include <strstream>
#include <ostream>
#include <stdint.h>
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <vector>
#include <fstream>
#include <memory>

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

};

//日志事件
class LogEvent{
public:
    typedef std::shared_ptr<LogEvent> ptr;

private:
    const char* m_name = nullptr;   //文件名
    int32_t m_line = 0;                 //行号
    uint64_t m_elapse = 0;              //程序启动到现在的时间(ms)
    uint32_t m_thread_id = 0;           //线程id
    uint32_t m_fiber_id = 0;            //协程id
    uint64_t m_time;                    //时间戳
    std::string m_content;              //

public:
    const char* getFile() const { return m_name; }
    int32_t getLine() const { return m_line; }
    uint64_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_thread_id; }
    uint32_t getFiberId() const { return m_fiber_id; }
    uint64_t getTime() const { return m_time; }
    std::string getContent() const { return m_content; }

    LogEvent(const char * name);
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
        virtual ~FormatItem(){};    
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };

private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;

public:
    LogFormatter(const std::string& pattern = "HH-mm-ss");
    ~LogFormatter();
    void addFormatItem(FormatItem::ptr item);
    void delFormatItem(FormatItem::ptr item);
    void init();
    std::string format(LogEvent::ptr event);
    const std::string getPattern() const { return this->m_pattern; };

};


class ThreadIdFormatItem : public LogFormatter::FormatItem{

public:
    ThreadIdFormatItem();
    ~ThreadIdFormatItem();
    void format(std::ostream& os, LogEvent::ptr event) override;
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
    virtual void Log(LogLevel::Level level,LogEvent::ptr event) = 0;
    virtual ~LogAppender();
};

//日志器
class Logger {
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
    void Log(LogLevel::Level level,LogEvent::ptr event) override ;
private:
};

//输出到文件
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    ~FileLogAppender();
    void Log(LogLevel::Level level,LogEvent::ptr event) override ;
    bool reopen();
private:
    char* m_filename = nullptr;
    std::ofstream m_ofs;
};

}
