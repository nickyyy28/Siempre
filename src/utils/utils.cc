#include "utils/utils.h"

namespace siem{

pid_t getThreadID(void)
{
    return syscall(SYS_gettid);
}

}