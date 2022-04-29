#include "Configurator/Env.h"

namespace siem{

Env::Env()
{

}

Env::~Env()
{
}

void Env::init(int argc, char** argv)
{
    std::stringstream ss;
    ss << argv[0];
    processName = ss.str();

    bool isKey = false;
    bool lastIsKey = false;
    std::string key;
    std::string value;

    for (int i = 1 ; i < argc ; i ++) {
        ss.str("");
        ss << argv[i];

        std::string temp = ss.str();

        if (temp[0] == '-') {
            if (lastIsKey) {
                add(key, "");
            }

            isKey = true;
            key = temp.substr(1, temp.size());

            if (i == argc - 1) {
                add(key, "");
            }

        } else {
            isKey = false;
            value = temp;

            if (lastIsKey) {
                add(key, value);
            } else {
                ERROR() << "invalid argument, Except -XXX " << value << " or -xxx";
            }
        }
        
        lastIsKey = isKey;
    }

    if (has("h")) {
        printHelp();
        std::exit(0);
    }
}

void Env::add(const std::string& key, const std::string& value)
{
    RWMutex::WriteLock lock(m_mutex);

    m_args[key] = value;
}

void Env::del(const std::string& key)
{
    RWMutex::WriteLock lock(m_mutex);
    if (has(key)) {
        m_args.erase(key);
    }
}

bool Env::has(const std::string& key)
{
    RWMutex::ReadLock lock(m_mutex);
    if (m_args.find(key) != m_args.end()) {
        return true;
    }

    return false;
}

const std::string& Env::get(const std::string& key, const std::string& val)
{
    RWMutex::ReadLock lock(m_mutex);

    if (has(key)) {
        return m_args[key];
    }

    return val;
}

void Env::addHelp(const std::string& key, const std::string& help)
{
    RWMutex::WriteLock lock(m_mutex);
    for (auto it = m_helps.begin() ; it != m_helps.end() ; ++it) {
        if (it->first == key) {
            m_helps.erase(it);
        }
    }
    m_helps.push_back(std::make_pair(key, help));
}

void Env::delHelp(const std::string& key)
{
    RWMutex::WriteLock lock(m_mutex);
    for (auto it = m_helps.begin() ; it != m_helps.end() ; ++it) {
        if (it->first == key) {
            m_helps.erase(it);
        }
    }

}

void Env::printHelp(void)
{
    RWMutex::ReadLock lock(m_mutex);

    std::cout << "usage: " << processName << " [options] ... [-c mod] ... " << std::endl;
    
    for (auto& h : m_helps) {
        std::cout << "-" << h.first << "\t: " << h.second << std::endl;
    }
}

void Env::printArgs(void)
{
    RWMutex::ReadLock lock(m_mutex);
    for (auto& it : m_args) {
        std::cout << "key: " << it.first << " value: " << it.second << std::endl;
    }
}

static Env::ptr local_env = SingleTonPtr<Env>::getInstancePtr();

}
