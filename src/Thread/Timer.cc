#include "Thread/Timer.h"
#include "Logger/Logger.h"
#include "Thread/Thread.h"
#include "common/macro.h"
#include <asm-generic/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <string>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

namespace siem {

int SocketTimer::timer_cnt = 0;

BaseTimer::BaseTimer(int timeout, TimerCallBack cb)
{
    this->m_timeout = timeout;
    this->m_callback = cb;
}

void BaseTimer::setCallBack(TimerCallBack cb)
{
    this->m_callback = cb;
}

void BaseTimer::setTimeout(int timeout)
{
    this->m_timeout = timeout;
}

SocketTimerListener::SocketTimerListener()
{
    m_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_fd == -1) {
        SIEM_ASSERT_STR(false, "Create Socket fd fail...");
    }

    memset(&m_addr, 0, sizeof(m_addr));
    m_addr.sin_family = AF_INET;
    m_addr.sin_port = htons(DEFAULT_TIMER_LISTEN_PORT);
    m_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt = 1;
    setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    int ret = bind(m_fd, (struct sockaddr*)&m_addr, (socklen_t)sizeof(m_addr));
    if (ret == -1) {
        SIEM_ASSERT_STR(false, "bind fail...");
    }

    ret = listen(m_fd, 64);
    if (ret == -1) {
        SIEM_ASSERT_STR(false, "listen fail...");
    }

    m_epfd = epoll_create(1024);
    if (m_epfd == -1) {
        SIEM_ASSERT_STR(false, "epoll create fail...");
    }

    m_ev.data.fd = m_fd;
    m_ev.events = EPOLLIN;
    ret = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_fd, &m_ev);
    if (ret == -1) {
        SIEM_ASSERT_STR(false, "epoll ctl error...");
    }

    m_listener.reset(new Thread([this](){
        struct epoll_event evs[1024];
        int size = sizeof(evs) / sizeof(struct epoll_event);
        int ret;

        while (true) {
            // 调用一次, 检测一次
            int num = epoll_wait(this->m_epfd, evs, size, -1);
            for(int i = 0 ; i < num ; ++i) {
                // 取出当前的文件描述符
                int curfd = evs[i].data.fd;
                // 判断这个文件描述符是不是用于监听的
                if (curfd == this->m_fd) {
                    // 建立新的连接
                    int cfd = accept(curfd, NULL, NULL);
                    // 新得到的文件描述符添加到epoll模型中, 下一轮循环的时候就可以被检测了
                    this->m_ev.events = EPOLLIN;    // 读缓冲区是否有数据
                    this->m_ev.data.fd = cfd;
                    ret = epoll_ctl(this->m_epfd, EPOLL_CTL_ADD, cfd, &this->m_ev);
                    if (ret == -1) {
                        SIEM_ASSERT_STR(false, "epoll ctl accept error");
                    }
                }
                else
                {
                    // 处理通信的文件描述符
                    // 接收数据
                    char buf[1024];
                    memset(buf, 0, sizeof(buf));
                    int len = recv(curfd, buf, sizeof(buf), 0);
                    if(len == 0)
                    {
                        // 将这个文件描述符从epoll模型中删除
                        epoll_ctl(this->m_epfd, EPOLL_CTL_DEL, curfd, NULL);
                        close(curfd);
                    }
                    else if(len > 0)
                    {
                        printf("客户端say: %s\n", buf);
                        send(curfd, buf, len, 0);
                    }
                    else
                    {
                        perror("recv");
                        exit(0);
                    } 
                }
            }
        }
    }, "timer_listener_thread"));
}

SocketTimerListener::~SocketTimerListener()
{

}

SocketTimerListener::ptr SocketTimerListener::getInstance()
{
    static SocketTimerListener::ptr var(new SocketTimerListener());

    return var;
}

SocketTimer::SocketTimer(int timeout, TimerCallBack cb) : BaseTimer(timeout, cb)
{
    int ret;
    struct sockaddr_in address;
    bzero (&address, sizeof (address) );
    address.sin_family = AF_INET;
    address.sin_port = htons (DEFAULT_TIMER_LISTEN_PORT);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);  // 非阻塞模式

    struct timeval timer;
    timer.tv_sec = 0;
    timer.tv_usec = timeout;
    socklen_t len = sizeof(timer);
    // 这里设置超时机制
    ret = setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timer, len);
    assert(ret != -1);

    m_thread.reset(new Thread([&](){
        while(true) {
            INFO() << "running...";
            int ret = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
            if (ret < 0) {
                // 超时对应的错误码
                if (errno == EINPROGRESS) {
                    this->m_callback();
                }
            }
            if (this->isSingle) {
                break;
            }
        }
    }, "timer_" + std::to_string(timer_cnt)));

}

SocketTimer::~SocketTimer()
{

}

void SocketTimer::start()
{
    isSingle = false;
    m_thread->detach();
}

void SocketTimer::startOnce()
{
    isSingle = true;
    m_thread->detach();
}

}