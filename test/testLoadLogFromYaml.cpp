#include <Configurator/Configurator.h>

using namespace siem;
using namespace std;

ConfigVar<Logger::ptr>::ptr syslog = Config::lookup<Logger::ptr>("system.log", GET_LOG_BY_NAME(testFromYaml));
ConfigVar<LogAppenderWrapper>::ptr wrap = Config::lookup<LogAppenderWrapper>("system.wrap", LogAppenderWrapper());

int main()
{
    siem::Config::loadFromYaml("/home/book/Siempre/test/bbb.yaml");

    cout << syslog->toString() << endl;

    cout << "-------------" << endl;

    cout << wrap->toString() << endl;

    return 0;
}