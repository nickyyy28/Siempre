#pragma once

#include <memory>
namespace siem {

namespace net {

class TCPConn;
typedef std::shared_ptr<TCPConn> TCPConnPtr;

class TCPConn{
public:
    TCPConn(int fd);
    ~TCPConn();

private:
    int m_fd;
};



}

}