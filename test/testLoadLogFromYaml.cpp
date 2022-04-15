#include <Configurator/Configurator.h>

using namespace siem;
using namespace std;

ConfigVar<Logger::ptr>::ptr syslog = Config::lookup<Logger::ptr>("system.log", GET_LOG_BY_NAME(system));
ConfigVar<LogAppenderWrapper>::ptr wrap = Config::lookup<LogAppenderWrapper>("system.wrap", LogAppenderWrapper());

int main(void)
{
    siem::Config::loadFromYaml("/home/book/Siempre/test/bbb.yaml");

    cout << Config::lookup<Logger::ptr>("system.log")->toString() << endl;

    cout << "-------------" << endl;

    cout << wrap->toString() << endl;

    return 0;
}