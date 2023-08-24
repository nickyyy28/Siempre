#include "net/InetAddress.h"
#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <strings.h>
#include <sys/socket.h>

namespace siem {

namespace net {

InetAddress::InetAddress(uint16_t port, const std::string& ip)
    : m_port(port),
    m_ip(ip)
{
    memset(&m_addr, 0, sizeof(struct sockaddr_in));
    m_addr.sin_port = htons(m_port);
    m_addr.sin_family = AF_INET;
    m_addr.sin_addr.s_addr = inet_addr(ip.c_str());
}

InetAddress::InetAddress(const struct sockaddr_in& addr)
    : m_addr(addr)
{
    
}

InetAddress::~InetAddress()
{

}

std::string InetAddress::toIp() const
{
    char buff[64];
    inet_ntop(AF_INET, &m_addr.sin_addr, buff, sizeof(buff));
    return std::string(buff);
}

std::string InetAddress::toIpPort() const
{
    return toIp() + ":" + std::to_string(ntohs(m_addr.sin_port));
}

uint16_t InetAddress::toPort() const
{
    return ntohs(m_addr.sin_port);
}



}

}