#include <Configurator/Configurator.h>

using namespace siem;
using namespace std;

ConfigVar<Logger::ptr>::ptr syslog = Config::lookup<Logger::ptr>("system.log", GET_LOG_BY_NAME(sys_log));
ConfigVar<LogAppenderWrapper>::ptr wrap = Config::lookup<LogAppenderWrapper>("system.wrap", LogAppenderWrapper());

int main(void)
{
    Config cfg;

    YAML::Node node = YAML::LoadFile("/home/book/Siempre/test/bbb.yaml");

    cfg.loadFromYaml(node);

    cout << Config::lookup<Logger::ptr>("system.log")->toString() << endl;

    cout << "-------------" << endl;

    cout << wrap->toString() << endl;

    return 0;
}