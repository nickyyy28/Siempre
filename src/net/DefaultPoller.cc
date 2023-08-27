#include "Configurator/Configurator.h"
#include "common/macro.h"
#include "net/EpollPoller.h"
#include "net/Poller.h"
#include <string>

namespace siem {

namespace net {

static ConfigVar<std::string>::ptr default_poller_config = Config::lookup<std::string>("default_poller", "epoll");

Poller* Poller::newDefaultPoller(EventLoop *loop)
{
    std::string default_poller = default_poller_config->getValue();
    if (default_poller == "epoll") {
        return new EpollPoller(loop);
    } else if (default_poller == "poll") {
        return nullptr;
    } else if (default_poller == "select") {
        return nullptr;
    } else {
        SIEM_ASSERT_STR(true, default_poller + " not found");
        return nullptr;
    }
}

}

}