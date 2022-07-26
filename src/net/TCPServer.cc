#include "net/TCPServer.h"

namespace siem {

namespace net {

TCPServer::TCPServer(const std::string& ip, int port)
{

}

TCPServer::~TCPServer()
{

}

void TCPServer::setOnConnectionCallback(const onConnectCallback& cb)
{
    m_connectionCallback = cb;
}

void TCPServer::setOnMessageCallback(const onMessageCallback& cb)
{
    m_messageCallback = cb;
}

void TCPServer::setThreadPoolThreshold(int threshold)
{

}

void TCPServer::start()
{
    
}

}

}