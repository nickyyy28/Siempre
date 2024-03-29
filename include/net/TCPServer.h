#pragma once

#include "../Thread/ThreadPool.h"
#include "../base/SString.h"
#include "../base/TimeStamp.h"
#include "../common/nocopyable.h"
#include "../net/TCPConn.h"
#include <functional>
#include <string>
#include <unordered_map>

namespace siem {

namespace net {

class EventLoop;

typedef siem::SString Buffer;

class TCPServer : NoCopyAble{
public:
    typedef std::function<void(const TCPConnPtr&, TimeStamp)> onConnectCallback;
    typedef std::function<void(const TCPConnPtr&, const Buffer*, TimeStamp)> onMessageCallback;
    typedef std::function<void(EventLoop*)> threadInitCallback;
    typedef std::unordered_map<int, TCPConnPtr> ConnectionMap;
    typedef std::string InetAddress;

    TCPServer(const std::string& ip, int port);
    TCPServer(EventLoop* eventLoop, const InetAddress& listenAddr, const std::string& args);
    ~TCPServer();

    /**
     * @brief 开启tcp服务器
     * 
     */
    void start();

    /**
     * @brief 设置最大工作线程
     * 
     * @param threshold 
     */
    void setThreadPoolThreshold(int threshold);

    /**
     * @brief 设置连接/断开回调
     * 
     * @param cb 
     */
    void setOnConnectionCallback(const onConnectCallback& cb);

    /**
     * @brief 设置接收消息回调
     * 
     * @param cb 
     */
    void setOnMessageCallback(const onMessageCallback& cb);

    const std::string& getIpPort();
private:

    void operator=(const TCPServer&) = delete;

    ConnectionMap m_conns;
    onConnectCallback m_connectionCallback;
    onMessageCallback m_messageCallback;
    ThreadPool *m_pool;
    int m_threadThreshold;

    int m_ip;

};

}

}