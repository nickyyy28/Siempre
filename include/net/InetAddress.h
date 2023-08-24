#pragma once

#include <arpa/inet.h>
#include <cstdint>
#include <string>
#include <netinet/in.h>
#include <sys/socket.h>

namespace siem {

namespace net {

class InetAddress {
public:
    explicit InetAddress(uint16_t port, const std::string& ip = "127.0.0.1");
    explicit InetAddress(const struct sockaddr_in& addr);
    ~InetAddress();

    std::string toIp() const;
    std::string toIpPort() const;
    uint16_t toPort() const;
    const struct sockaddr_in* getSockAddr() const { return &m_addr; }

private:
    struct sockaddr_in m_addr;
    std::string m_ip;
    uint16_t m_port;

};


}

}