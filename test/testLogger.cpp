#include <iostream>
#include <Logger/Logger.h>

int main(void)
{
    siempre::Logger::ptr logger(new siempre::Logger("root"));
    siempre::StdoutLogAppender::ptr appender(new siempre::StdoutLogAppender());
    siempre::LogFormatter::ptr formatter(new siempre::LogFormatter());
    siempre::LogEvent::ptr event(new siempre::LogEvent("test logger"));
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