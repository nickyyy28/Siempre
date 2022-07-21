#pragma once

#include <string.h>
#include <assert.h>
#include "../Logger/Logger.h"
#include "../utils/utils.h"

#define SIEM_ASSERT(experssion) \
    if (!(experssion)) { \
        LOG_FATAL(GET_LOG_BY_NAME(system)) << "ASSERT: " << siem::BackTraceToString(100, 2);   \
        std::exit(-1);  \
    }

#define SIEM_ASSERT_STR(experssion, description) \
    if (!(experssion)) { \
        LOG_FATAL(GET_LOG_BY_NAME(system)) << "ASSERT: " << siem::BackTraceToString(100, 2) << "\ndescription:\t" << description;   \
        std::exit(-1);  \
    }


#if defined __GNUC__ || defined __llvm__
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率成立
#   define SIEM_LIKELY(x)       __builtin_expect(!!(x), 1)
/// LIKCLY 宏的封装, 告诉编译器优化,条件大概率不成立
#   define SIEM_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define SIEM_LIKELY(x)      (x)
#   define SIEM_UNLIKELY(x)      (x)
#endif

#define foo() do{}while(0)
