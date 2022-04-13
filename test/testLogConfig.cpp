#include <Configurator/Configurator.h>

using namespace siem;
using namespace std;

int main(void)
{
    siem::ConfigVar<siem::Logger::ptr>::ptr logger = siem::Config::lookup<siem::Logger::ptr>("root", GET_LOG_ROOT());


    std::cout << logger->toString() << endl;

    cout << "root name: " << GET_LOG_ROOT()->getLoggerName() << endl;

    LogLevel::Level l= LogLevel::DEBUG;
    LogFormatter::ptr fmt(new LogFormatter("%d%n"));
    StdoutLogAppender::ptr app(new StdoutLogAppender());

    LogAppenderWrapper wrap(app, "Stdout");

    app->setFormatter(fmt);
    app->setLevel(l);

    ConfigVar<LogLevel::Level>::ptr level = Config::lookup<LogLevel::Level>("level", l);
    ConfigVar<LogFormatter::ptr>::ptr formatter = Config::lookup<LogFormatter::ptr>("formatter", fmt);
    ConfigVar<StdoutLogAppender::ptr>::ptr appender = Config::lookup<StdoutLogAppender::ptr> ("appender", app);
    ConfigVar<LogAppenderWrapper>::ptr wrapper = Config::lookup<LogAppenderWrapper>("wrap", wrap);

    cout << level->toString() << endl;
    
    cout << "-------------------" << endl;

    cout << formatter->toString() << endl;

    cout << "-------------------" << endl;

    cout << appender->toString() << endl;

    cout << "-------------------" << endl;

    cout << wrapper->toString() << endl;

    return 0;
}