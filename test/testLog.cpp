#include <iostream>
#include <siem/siem>
#include <unistd.h>
#include <sys/stat.h>

int main(void)
{
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

    siem::LoggerMgr::getInstance()->addRootFileAppender("/home/book/testlog.log");

    siem::LoggerMgr::getInstance()->setRootFormat("[%d:%r] %p%f%l%n%m");



    //LOG_DEBUG(siem::LoggerMgr::getInstance()->getRoot()) << "hello world";

    int cnt = 0;

    while(cnt < 10) {
        sleep(1);

        INFO() << "msg " << cnt;
        cnt ++;
    }

    return 0;
}