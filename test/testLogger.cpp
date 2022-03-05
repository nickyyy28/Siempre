#include <iostream>
#include <Logger/Logger.h>

int main(void)
{
    std::shared_ptr<siempre::Logger> logger(new siempre::Logger("root"));
    
    siempre::StdoutLogAppender::ptr appender(new siempre::StdoutLogAppender());
    
    siempre::LogFormatter::ptr formatter(new siempre::LogFormatter("%p%T%n"));
    
    //siempre::LogEvent::ptr event(new siempre::LogEvent(std::string("testEvent"), logger, __FILE__, __LINE__, 0, 2, std::string("master"), 0, time(0), siempre::LogLevel::DEBUG));

    siempre::LogEvent::ptr event(new siempre::LogEvent(std::string("testEvent"), logger, __FILE__, __LINE__, 0, 1, "main", 1, 0, siempre::LogLevel::DEBUG));

    siempre::ThreadIdFormatItem::ptr item(new siempre::ThreadIdFormatItem());

    logger.get()->addAppender(appender);
    logger.get()->setLevel(siempre::LogLevel::DEBUG);

    appender.get()->setLevel(siempre::LogLevel::DEBUG);
    appender.get()->setFormatter(formatter);

    formatter.get()->addFormatItem(item);

    logger.get()->debug(event);
    logger.get()->info(event);
    logger.get()->warn(event);
    logger.get()->error(event);
    logger.get()->fatal(event);

    return 0;
}