#include <Configurator/Configurator.h>
#include <iostream>

using namespace siem;
using namespace std;

ConfigVar<Logger::ptr>::ptr syslog = Config::lookup<Logger::ptr>("system.log", GET_LOG_BY_NAME(testFromYaml));
// ConfigVar<LogAppenderWrapper>::ptr wrap = Config::lookup<LogAppenderWrapper>("system.wrap", LogAppenderWrapper());
ConfigVar<int>::ptr var = Config::lookup<int>("system.test_var", 0);

int main()
{
    siem::Config::saveAsYaml("");

    siem::Config::loadFromYaml("/home/book/Siempre/test/bbb.yaml");

    cout << syslog->toString() << endl;

    cout << "var = " << var->getValue() << endl;

    cout << "-------------" << endl;

    // cout << wrap->toString() << endl;

    siem::Config::saveAsYaml("");

    cout << "------------------" << endl;

    return 0;
}