#include "tcp_server.h"
#include "tcp_session.h"
#include "log.h"
#include "../src/util.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <functional>

static int handlePollerEvent(void* data, int type)
{
    IPollerEventHandler* handler = (IPollerEventHandler*)data;
    if (handler) {
        handler->handle(type);
    }
}

TcpServer::TcpServer(const char* host, uint16_t local_port, int backlog) 
    : host_(host), local_port_(local_port), backlog_(backlog), poller_(nullptr)
{
    
}

TcpServer::~TcpServer()
{
    if (poller_) {
        poller_epoll_uninitialize(poller_);
        poller_ = nullptr;
    }
}

int TcpServer::init()
{
    struct sockaddr_in addr; 
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host_.c_str());
    addr.sin_port = htons(local_port_);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return -1;
    }
    if (::bind(fd, (const struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOGD("bind to %s:%d faild, error: %s", host_.c_str(), local_port_, strerror(errno));
    }
    if (::listen(fd, backlog_) < 0) {
        LOGD("failed to listen to %s:%d, error: %s", host_.c_str(), local_port_, strerror(errno));
    }
    set_nodelay(fd, 1);
    set_nonblock(fd, 1);
    poller_ = poller_epoll_create();
    if (poller_ == nullptr) {
        return -1;
    }
    poller_->listen_fd = fd;
    if (poller_epoll_initialize(poller_) < 0) {
        return -1;
    }
    poller_->handle_event = handlePollerEvent;
    poller_->event_op(poller_->listen_fd, poller_, OPERATION_ADD, EVENT_READ, this);
    return 0;
}

int TcpServer::handle(int type)
{
    if ((type & EVENT_READ) == 0) {
        return -1;
    }
    if (accept() < 0) {
        return -1;
    }
    return 0;
}

int TcpServer::accept()
{
    struct sockaddr_in sa;
    socklen_t len = sizeof(sa);
    int newfd = ::accept(poller_->listen_fd, (sockaddr *)(&sa), &len);
    if (newfd == -1) {
        LOGD("failed to accept, error: %s", strerror(errno));
        return -1;
    }
    LOGD("fd(%d) connected", newfd);
    TcpSession* session = new TcpSession(newfd, this);
    poller_->event_op(newfd, poller_, OPERATION_ADD, EVENT_READ, session);
    return 0;
}

void TcpServer::start()
{
    do {
        int rc = poller_->loop(poller_);
        if (rc < 0) {
            /*log error*/
        }

    } while(true);
}