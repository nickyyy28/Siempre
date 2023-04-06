#pragma once

#include <string>
#include "../utils/utils.h"
#include <memory>

namespace siem {

#define GET_FULL_TYPE(__var__)  \
    (siem::type_traits<decltype(__var__)>().type)

template<class T>
struct type_traits{
    const std::string type = siem::TypeToName<T>();
};

template<class T>
struct type_traits<const T>{
    const std::string type = std::string("const ") + type_traits<T>().type;
};

template<class T>
struct type_traits<T&>{
    const std::string type = type_traits<T>().type + "&";
};

template<class T>
struct type_traits<const T&>{
    const std::string type = "const " + type_traits<T>().type + "&";
};

template<class T>
struct type_traits<T&&>{
    const std::string type = type_traits<T>().type + "&&";
};

template<class T>
struct type_traits<const T&&>{
    const std::string type = "const " + type_traits<T>().type + "&&";
};

template<class T>
struct type_traits<T*>{
    const std::string type = type_traits<T>().type + "*";
};

template<class T>
struct type_traits<std::shared_ptr<T>>{
    const std::string type = "std::shared_ptr<" + type_traits<T>().type + ">";
};

}