#ifndef __LOGGER_H
#define __LOGGER_H

#include <iostream>
#include <string>
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

#include <cstring>
#include <cstdarg>
#include <cstdlib>

#include "common/singleton.h"
#include "utils/utils.h"
#include "Thread/Thread.h"
#include "Thread/Mutex.h"

/**
 * @brief 使用流式方式将日志级别level的日志写入到logger
 */
#define LOG_LEVEL(logger, level) \
    if(logger->getLevel() <= level) \
        siem::LogEventWrapper(siem::LogEvent::ptr(new siem::LogEvent(logger->getLoggerName(), logger, \
            __FILE__, __LINE__, 0, siem::getThreadID(), siem::Thread::getThisName(), siem::getFiberID(), \
            time(0), level))).getSS()

/**
 * @brief 使用流式方式将日志级别debug的日志写入到logger
 */
#define LOG_DEBUG(logger) LOG_LEVEL(logger, siem::LogLevel::DEBUG)

/**
 * @brief 使用流式方式将日志级别info的日志写入到logger
 */
#define LOG_INFO(logger) LOG_LEVEL(logger, siem::LogLevel::INFO)

/**
 * @brief 使用流式方式将日志级别warn的日志写入到logger
 */
#define LOG_WARN(logger) LOG_LEVEL(logger, siem::LogLevel::WARN)

/**
 * @brief 使用流式方式将日志级别error的日志写入到logger
 */
#define LOG_ERROR(logger) LOG_LEVEL(logger, siem::LogLevel::ERROR)

/**
 * @brief 使用流式方式将日志级别fatal的日志写入到logger
 */
#define LOG_FATAL(logger) LOG_LEVEL(logger, siem::LogLevel::FATAL)

#define DEBUG() \
    LOG_DEBUG(siem::LoggerMgr::getInstance()->getRoot())

#define INFO() \
    LOG_INFO(siem::LoggerMgr::getInstance()->getRoot())

#define WARN() \
    LOG_WARN(siem::LoggerMgr::getInstance()->getRoot())

#define ERROR() \
    LOG_ERROR(siem::LoggerMgr::getInstance()->getRoot())

#define FATAL() \
    LOG_FATAL(siem::LoggerMgr::getInstance()->getRoot())

/**
 * @brief 使用格式化方式将日志级别level的日志写入到logger
 */
#define LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        siem::LogEventWrapper(siem::LogEvent::ptr(new siem::LogEvent(logger->getLoggerName(), logger, \
            __FILE__, __LINE__, 0, siem::getThreadID(), siem::Thread::getThisName(), siem::getFiberID(), \
            time(0), level))).getEvent()->format(fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别debug的日志写入到logger
 */
#define LOG_FMT_DEBUG(logger, fmt, ...) LOG_FMT_LEVEL(logger, siem::LogLevel::DEBUG, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别info的日志写入到logger
 */
#define LOG_FMT_INFO(logger, fmt, ...)  LOG_FMT_LEVEL(logger, siem::LogLevel::INFO, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别warn的日志写入到logger
 */
#define LOG_FMT_WARN(logger, fmt, ...)  LOG_FMT_LEVEL(logger, siem::LogLevel::WARN, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别error的日志写入到logger
 */
#define LOG_FMT_ERROR(logger, fmt, ...) LOG_FMT_LEVEL(logger, siem::LogLevel::ERROR, fmt, __VA_ARGS__)

/**
 * @brief 使用格式化方式将日志级别fatal的日志写入到logger
 */
#define LOG_FMT_FATAL(logger, fmt, ...) LOG_FMT_LEVEL(logger, siem::LogLevel::FATAL, fmt, __VA_ARGS__)


#define GET_LOG_ROOT()          \
    siem::LoggerMgr::getInstance()->getRoot()

#define GET_LOG_BY_NAME(name)   \
    siem::LoggerMgr::getInstance()->getLogger(#name)

namespace siem {

unsigned long long getThisThreadID();

class LogLevel;
class Logger;
class LogEvent;
class LogFormatter;
class LogAppender;
class StdoutLogAppender;
class FileLogAppender;
class LoggerManager;
class LogEventWrapper;
class LogAppenderWrapper;

/**
 * @brief 日志级别
 * 
 */
class LogLevel{
public:
    enum Level{
        UNKNOW = -1,
        DEBUG = 1,
        INFO,
        WARN,
        ERROR,
        FATAL
    };

    static std::string getStringByLevel(Level level);

    static LogLevel::Level getLevelByString(const std::string& str);

};

/**
 * @brief 日志事件
 * 
 */
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
    std::shared_ptr<Logger> m_logger;   //日志器
    std::stringstream m_sstream;        //信息

public:
    /**
     * @brief 获取文件名
     * 
     * @return const char* 
     */
    const char* getFile() const { return m_filename; }

    /**
     * @brief 获取事件名
     * 
     * @return const std::string& 
     */
    const std::string& getEventName() const { return m_name; }

    /**
     * @brief 获取行号
     * 
     * @return uint32_t 
     */
    uint32_t getLine() const { return m_line; }

    /**
     * @brief 获取程序执行时间
     * 
     * @return uint64_t 
     */
    uint64_t getElapse() const { return m_elapse; }

    /**
     * @brief 获取线程ID
     * 
     * @return uint32_t 
     */
    uint32_t getThreadId() const { return m_thread_id; }

    /**
     * @brief 获取线程名
     * 
     * @return const std::string& 
     */
    const std::string& getThreadName() const { return m_thread_name; }

    /**
     * @brief 获取协程ID
     * 
     * @return uint32_t 
     */
    uint32_t getFiberId() const { return m_fiber_id; }

    /**
     * @brief 获取
     * 
     * @return uint64_t 
     */
    uint64_t getTime() const { return m_time; }

    /**
     * @brief 设置日志消息
     * 
     * @param msg 
     */
    void setMessage(std::string& msg);

    /**
     * @brief 获取日志消息
     * 
     * @return std::string 
     */
    std::string getContent() const { return m_sstream.str(); }

    /**
     * @brief 获取消息流
     * 
     * @return std::stringstream& 
     */
    std::stringstream& getSStream() { return m_sstream; }

    /**
     * @brief 获取日志器
     * 
     * @return std::shared_ptr<Logger> 
     */
    std::shared_ptr<Logger> getLogger() { return m_logger; }

    /**
     * @brief 获取日志级别
     * 
     * @return LogLevel 
     */
    LogLevel::Level getLevel() { return m_level; };

    /**
     * @brief 格式化写入日志事件
     */
    void format(const char* fmt, ...);

    /**
     * @brief 格式化写入日志内容
     */
    void format(const char* fmt, va_list al);

    /**
     * @brief 构造日志事件
     * 
     * @param name          日志名
     * @param logger        日志
     * @param filename      文件名
     * @param line          行号
     * @param elapse        程序执行时间
     * @param thread_id     线程ID
     * @param thread_name   线程名
     * @param fiber_id      协程ID
     * @param time          时间
     * @param level         日志等级
     */
    LogEvent(
        std::string name, 
        std::shared_ptr<Logger> logger, 
        const char * filename, 
        uint32_t line, 
        uint64_t elapse, 
        uint32_t thread_id, 
        const std::string thread_name,
        uint32_t fiber_id, 
        uint64_t time, 
        LogLevel::Level level);
    LogEvent();
    ~LogEvent();
};

/**
 * @brief 日志格式化器
 * 
 */
class LogFormatter {
public:
    typedef std::shared_ptr<LogFormatter> ptr;

public:
    class FormatItem{
    public:
        // %T : Tab[\t]            TabFormatItem
        // %r : 累计毫秒数          ElapseFormatItem
        // %d : 日期                DateTimeFormatItem
        // %t : 线程id             ThreadIdFormatItem
        // %N : 线程名称           ThreadNameFormatItem
        // %F : 协程id             FiberIdFormatItem
        // %p : 日志级别           LevelFormatItem
        // %c : 日志名称           NameFormatItem
        // %f : 文件名             FilenameFormatItem
        // %l : 行号               LineFormatItem
        // %m : 日志内容           MessageFormatItem
        // %n : 换行符[\r\n]       NewLineFormatItem

        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem() = default;
        FormatItem(const std::string& str) : m_str(str) {};
        virtual ~FormatItem(){};

        /**
         * @brief 将日志格式化入日志流中
         * 
         * @param os        日志输入流
         * @param logger    日志器
         * @param level     日志器
         * @param event     日志事件
         */
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    protected:
        std::string m_str;
    };

private:
    Mutex m_mutex;
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error;

public:

    /**
     * @brief 根据字符串构造日志格式化器
     * 
     * @param pattern 日志格式
     */
    LogFormatter(const std::string& pattern = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f:%l%T%m%n");
    ~LogFormatter();

    /**
     * @brief 添加格式化项
     * 
     * @param item 某项
     */
    void addFormatItem(FormatItem::ptr item);
    void delFormatItem(FormatItem::ptr item);
    void init();

    /**
     * @brief 格式化日志
     * 
     * @param logger    日志器
     * @param level     日志等级
     * @param event     日志事件
     * @return std::string
     */
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    const std::string& getPattern() const { return this->m_pattern; }

};

/**
 * @brief 格式化日期
 * 
 */
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

/**
 * @brief 格式化线程ID
 * 
 */
class ThreadIdFormatItem : public LogFormatter::FormatItem{
public:
    ThreadIdFormatItem(const std::string& str = "") : FormatItem(str) {};
    ~ThreadIdFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
}; 

/**
 * @brief 格式化日志消息
 * 
 */
class StringFormatItem : public LogFormatter::FormatItem {
public:
    StringFormatItem(const std::string& str = "")
        : FormatItem(str) {}
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override {
        os << m_str;
    }
private:
};

/**
 * @brief 格式化程序执行时间
 * 
 */
class ElapseFormatItem : public LogFormatter::FormatItem{

public:
    ElapseFormatItem(const std::string& str) : FormatItem(str) {};
    ~ElapseFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
}; 

/**
 * @brief 格式化普通信息
 * 
 */
class BaseFormatItem : public LogFormatter::FormatItem{
public:
    BaseFormatItem(const std::string& str) : FormatItem(str) {};
    ~BaseFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化制表符
 * 
 */
class TabFormatItem : public LogFormatter::FormatItem{
public:
    TabFormatItem(const std::string& str) : FormatItem(str) {};
    ~TabFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化线程名
 * 
 */
class ThreadNameFormatItem : public LogFormatter::FormatItem{
public:
    ThreadNameFormatItem(const std::string& str) : FormatItem(str) {};
    ~ThreadNameFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化协程ID
 * 
 */
class FiberIDFormatItem : public LogFormatter::FormatItem{
public:
    FiberIDFormatItem(const std::string& str) : FormatItem(str) {};
    ~FiberIDFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化日志等级
 * 
 */
class LevelFormatItem : public LogFormatter::FormatItem{
public:
    LevelFormatItem(const std::string& str) : FormatItem(str) {};
    ~LevelFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化日志名
 * 
 */
class NameFormatItem : public LogFormatter::FormatItem{
public:
    NameFormatItem(const std::string& str) : FormatItem(str) {};
    ~NameFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化文件名
 * 
 */
class FileNameFormatItem : public LogFormatter::FormatItem{
public:
    FileNameFormatItem(const std::string& str) : FormatItem(str) {};
    ~FileNameFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化行号
 * 
 */
class LineFormatItem : public LogFormatter::FormatItem{
public:
    LineFormatItem(const std::string& str) : FormatItem(str) {};
    ~LineFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 格式化消息
 * 
 */
class MessageFormatItem : public LogFormatter::FormatItem{
public:
    MessageFormatItem(const std::string& str) : FormatItem(str) {};
    ~MessageFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};
/**
 * @brief 格式化新行
 * 
 */
class NewLineFormatItem : public LogFormatter::FormatItem{
public:
    NewLineFormatItem(const std::string& str) : FormatItem(str) {};
    ~NewLineFormatItem(){};
    void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) override;
};

/**
 * @brief 日志输出地
 * 
 */
class LogAppender {
public:
    typedef std::shared_ptr<LogAppender> ptr;

protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    LogFormatter::ptr m_formatter;
    Mutex m_mutex;
public:
    LogAppender();

    /**
     * @brief 设置日志等级
     * 
     * @param level     日志等级
     */
    void setLevel(LogLevel::Level level) { this->m_level = level; };

    /**
     * @brief 设置日志格式化器
     * 
     * @param formatter     日志格式化器
     */
    void setFormatter(LogFormatter::ptr formatter) {
        Mutex::Lock fmtLock(m_mutex);
        this->m_formatter.reset();
        this->m_formatter = formatter;
    }

    /**
     * @brief 获取格式化器
     * 
     * @return LogFormatter::ptr 
     */
    LogFormatter::ptr getFormatter(void) const { return m_formatter; }

    /**
     * @brief 获取日志等级
     * 
     * @return LogLevel::Level 
     */
    LogLevel::Level getLevel(void) const { return m_level; }

    /**
     * @brief 获取互斥量
     * 
     * @return Mutex& 
     */
    Mutex& getMutex(void) { return m_mutex; }

    /**
     * @brief 输出日志
     * 
     * @param logger    日志器
     * @param level     日志等级
     * @param event     日志事件
     */
    virtual void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) = 0;
    virtual ~LogAppender();

    virtual std::string getTypeName(void) { return TypeToName<LogAppender>(); };
};

/**
 * @brief 日志器
 * 
 */
class Logger : public std::enable_shared_from_this<Logger> {
public:
    friend class LoggerManager;
    typedef std::shared_ptr<Logger> ptr;

private:
    /**
     * @brief 日志器名
     * 
     */
    std::string m_name;

    /**
     * @brief 日志输出地列表
     * 
     */
    std::list<LogAppender::ptr> m_appenders;

    /**
     * @brief 日志器等级
     * 
     */
    LogLevel::Level m_level = LogLevel::DEBUG;

    /**
     * @brief 日志锁
     * 
     */
    Mutex m_mutex;

public:

    /**
     * @brief 构造日志器
     * 
     * @param name      日志器名
     */
    Logger(const std::string &name = "root");

    /**
     * @brief 输出日志
     * 
     * @param level     日志等级
     * @param event     日志事件
     */
    void Log(LogLevel::Level level,LogEvent::ptr event);
    virtual ~Logger();

    /**
     * @brief 添加日志输出地
     * 
     * @param appender 日志输出地
     */
    void addAppender(LogAppender::ptr appender);

    /**
     * @brief 添加日志输出地
     * 
     * @param list 
     */
    void addAppender(const std::list<LogAppender::ptr>& list);

    /**
     * @brief 删除日志输出地
     * 
     * @param appender 日志输出地
     */
    void delAppender(LogAppender::ptr appender);

    /**
     * @brief 设置日志器日志等级
     * 
     * @param level 
     */
    void setLevel(LogLevel::Level level){ this->m_level = level; };

    /**
     * @brief 获取日志器日志等级
     * 
     * @return LogLevel::Level 
     */
    LogLevel::Level getLevel(void) const {return this->m_level; };

    /**
     * @brief 获取日志器名
     * 
     * @return const std::string&
     */
    const std::string& getLoggerName(void) const { return m_name; }

    /**
     * @brief 获取日志输出地
     * 
     * @return const std::list<LogAppender::ptr>& 
     */
    const std::list<LogAppender::ptr>& getAppenders() const { return m_appenders; }

    /**
     * @brief 设置appenders的format格式
     * 
     * @param fmt 
     */
    void setFormatter(const std::string& fmt);

    /**
     * @brief 输出debug信息
     * 
     * @param event 日志等级
     */
    void debug(const LogEvent::ptr event);

    /**
     * @brief 输出正常信息
     * 
     * @param event 日志等级
     */
    void info(const LogEvent::ptr event);

    /**
     * @brief 输出警告信息
     * 
     * @param event 日志等级
     */
    void warn(const LogEvent::ptr event);

    /**
     * @brief 输出错误信息
     * 
     * @param event 日志等级
     */
    void error(const LogEvent::ptr event);

    /**
     * @brief 输出危险信息
     * 
     * @param event 
     */
    void fatal(const LogEvent::ptr event);
};

//输出到控制台
class StdoutLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    StdoutLogAppender();
    ~StdoutLogAppender();
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override ;

    std::string getTypeName(void) override { return TypeToName<StdoutLogAppender>(); }
private:
};

//输出到文件
class FileLogAppender : public LogAppender{
public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string &filename);
    ~FileLogAppender();
    void Log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) override ;
    const std::string& getFilename(void) const { return m_filename; }
    std::string getTypeName(void) override { return TypeToName<FileLogAppender>(); }
    bool reopen();
private:
    std::string m_filename = "";
    std::ofstream m_ofs;
};

/**
 * @brief 日志管理器
 * 
 */
class LoggerManager{
public:
    typedef std::shared_ptr<LoggerManager> ptr;

    LoggerManager();
    ~LoggerManager();

    /**
     * @brief 根据日志器名获取日志器, 不存在就创建一个
     * 
     * @param loggerName    日志器名
     * @return Logger::ptr 
     */
    Logger::ptr getLogger(const std::string& loggerName);

    /**
     * @brief 返回根日志器
     * 
     * @return Logger::ptr 
     */
    Logger::ptr getRoot();

    /**
     * @brief 设置Root日志器格式化格式
     * 
     * @param format 
     */
    void setRootFormat(const std::string& format = "%d{%Y-%m-%d %H:%M:%S}%T%t%T%N%T%F%T%p%T%c%T%f:%l%T%m%n");

    /**
     * @brief 添加文件输出路径
     * 
     * @param filePath 
     */
    void addRootFileAppender(const std::string& filePath = "");

private:
    Logger::ptr m_root;
    std::map<std::string, Logger::ptr> m_loggerMap;
    Mutex m_mutex;
};

/**
 * @brief 事件包装器
 * 
 */
class LogEventWrapper{
public:
    LogEventWrapper(LogEvent::ptr e);
    ~LogEventWrapper();

    /**
     * @brief 获取日志流
     * 
     * @return std::stringstream& 
     */
    std::stringstream& getSS();

private:
    Mutex m_mutex;
    Logger::ptr logger;
    LogEvent::ptr event;
};

/**
 * @brief 日志事件地输出包装类
 * 
 */
class LogAppenderWrapper{
public:
    LogAppenderWrapper(LogAppender::ptr p, const std::string& type = "Stdout") 
        : m_type(type)
        , appender(p) {
        }

    LogAppenderWrapper() {}

    ~LogAppenderWrapper() {}

    /**
     * @brief 设置日志输出地
     * 
     * @param p 
     */
    void setAppender(LogAppender::ptr p) { appender = p; }

    /**
     * @brief 设置输出地类型
     * 
     * @param type 
     */
    void setType(const std::string& type) { m_type = type; }

    /**
     * @brief 获取日志输出地
     * 
     * @return LogAppender::ptr 
     */
    LogAppender::ptr getAppender(void) const { return appender; }

    /**
     * @brief 获取日志输出地类型
     * 
     * @return const std::string& 
     */
    const std::string& getType(void) const { return m_type; }

private:
    std::string m_type;
    LogAppender::ptr appender;
};

/**
 * @brief 日志管理器, 单例模式
 * 
 */
using LoggerMgr = SingleTon<LoggerManager>;

}

#endif