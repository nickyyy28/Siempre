#include <iostream>
#include <siem>

int main(void)
{
    std::shared_ptr<siem::Logger> logger(new siem::Logger("root"));
    
    siem::StdoutLogAppender::ptr appender(new siem::StdoutLogAppender());
    
    siem::LogFormatter::ptr formatter(new siem::LogFormatter("%p%T%n"));
    
    //siem::LogEvent::ptr event(new siempre::LogEvent(std::string("testEvent"), logger, __FILE__, __LINE__, 0, 2, std::string("master"), 0, time(0), siempre::LogLevel::DEBUG));

    siem::LogEvent::ptr event(new siem::LogEvent(std::string("testEvent"), logger, __FILE__, __LINE__, 0, 1, "main", 1, 0, siem::LogLevel::DEBUG));

    siem::ThreadIdFormatItem::ptr item(new siem::ThreadIdFormatItem());

    logger.get()->addAppender(appender);
    logger.get()->setLevel(siem::LogLevel::DEBUG);

    appender.get()->setLevel(siem::LogLevel::DEBUG);
    appender.get()->setFormatter(formatter);

    formatter.get()->addFormatItem(item);

    logger.get()->debug(event);
    logger.get()->info(event);
    logger.get()->warn(event);
    logger.get()->error(event);
    logger.get()->fatal(event);

    return 0;
}