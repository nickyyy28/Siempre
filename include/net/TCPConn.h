#pragma once

#include <memory>
namespace siem {

namespace net {

class TCPConn;
using TCPConnPtr = std::shared_ptr<TCPConn> ;

class TCPConn{
public:
    TCPConn(int fd);
    ~TCPConn();

private:
    int m_fd;
};



}

}