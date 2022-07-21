#include <Configurator/Configurator.h>
#include <iostream>

using namespace siem;
using namespace std;

ConfigVar<Logger::ptr>::ptr syslog = Config::lookup<Logger::ptr>("system.log", GET_LOG_BY_NAME(testFromYaml));
// ConfigVar<LogAppenderWrapper>::ptr wrap = Config::lookup<LogAppenderWrapper>("system.wrap", LogAppenderWrapper());

int main()
{
    siem::Config::saveAsYaml("");

    siem::Config::loadFromYaml("/home/book/Siempre/test/bbb.yaml");

    cout << syslog->toString() << endl;

    cout << "-------------" << endl;

    // cout << wrap->toString() << endl;

    siem::Config::saveAsYaml("");

    cout << "------------------" << endl;

    return 0;
}