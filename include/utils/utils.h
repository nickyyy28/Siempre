#ifndef __UTILS_H
#define __UTILS_H

#include <cxxabi.h>

namespace siem{

template<class T>
const char* TypeToName() {
    static const char* s_name = abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr);
    return s_name;
}

}

#endif