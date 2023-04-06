#pragma once

#include "./nocopyable.h"

#include <memory>
#include <mutex>

namespace siem {

/**
 * @brief 普通单例模式
 * 
 * @tparam T 需要单例的类
 */
template<class T>
class SingleTon : NoCopyAble {
public:

    /**
        * @brief Get the Instance object
        *
        * @return T*
        */
    static T *getInstance() {
        static T *t = new T();
        return t;
    }
};

/**
 * @brief 智能指针单例模式
 * 
 * @tparam T 需要单例的类
 */
template<class T>
class SingleTonPtr : public NoCopyAble {
public:

    /**
        * @brief Get the Instance Ptr object
        *
        * @return std::shared_ptr<T>
        */
    static std::shared_ptr<T> getInstancePtr() {
        static std::shared_ptr<T> ptr = std::make_shared<T>();
        return ptr;
    }
};


} // namespace siem