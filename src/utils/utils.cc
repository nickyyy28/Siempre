#include "utils/utils.h"
#include "Logger/Logger.h"
#include "Fiber/Fiber.h"

namespace siem{

Logger::ptr syslog = GET_LOG_BY_NAME(system);

pid_t getThreadID(void)
{
    return syscall(SYS_gettid);
}

static std::string demangle(const char* str) {
    size_t size = 0;
    int status = 0;
    std::string rt;
    rt.resize(256);
    if(1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) {
        char* v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
        if(v) {
            std::string result(v);
            free(v);
            return result;
        }
    }
    if(1 == sscanf(str, "%255s", &rt[0])) {
        return rt;
    }
    return str;
}

void BackTrace(std::vector<std::string>& vec, size_t size, int skip)
{
    void** array = (void**)malloc(sizeof(void*) * size);
    size_t s = backtrace(array, size);

    char** strs = backtrace_symbols(array, s);

    if (strs == NULL) {
        LOG_ERROR(syslog) << "BackTrace Exception Ocurred";
        return;
    }

    for (size_t i = skip ; i < s ; i++) {
        vec.push_back(demangle(strs[i]));
    }


    free(array);
    free(strs);

}

std::string BackTraceToString(size_t size, int skip)
{
    std::vector<std::string> vec;
    std::stringstream ss;

    BackTrace(vec, size, skip);

    for (size_t s = 0 ; s < vec.size() ; s++) {
        if (s != vec.size() - 1) {
            ss << vec[s] << std::endl;
        } else {
            ss << vec[s];
        }
    }

    return ss.str();
}

uint64_t getFiberID(void)
{
    return siem::Fiber::getCurFiberID();
}

}