#ifndef __SIEM_MACRO_H
#define __SIEM_MACRO_H

#define NDEBUG

#include <string.h>
#include <assert.h>
#include "Logger/Logger.h"
#include "utils/utils.h"

#define SIEM_ASSERT(experssion) \
    if (!(experssion)) { \
        LOG_FATAL(GET_LOG_BY_NAME(system)) << "ASSERT: " << siem::BackTraceToString(100, 2);   \
        std::exit(-1);  \
    }

#define SIEM_ASSERT_STR(experssion, description) \
    if (!(experssion)) { \
        LOG_FATAL(GET_LOG_BY_NAME(system)) << "ASSERT: " << siem::BackTraceToString(100, 2) << "\ndescription:\t" << #description;   \
        std::exit(-1);  \
    }


#endif //__SIEM_MACRO_H