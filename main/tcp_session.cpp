#include "tcp_session.h"
#include "log.h"
#include "tcp_server.h"
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

TcpSession::TcpSession(int fd, TcpServer* tcp_server)
    : fd_(fd), tcp_server_(tcp_server)
{
    
}

TcpSession::~TcpSession()
{
    if (fd_ >= 0) {
        close(fd_);
    }
}
int TcpSession::onRead()
{
    static uint64_t count = 0;
    char buffer[256] = {0};
    int size = 5;
    ssize_t rc = ::recv(fd_, buffer, size, 0);
    if (rc == -1) {
        LOGD("recv fd(%d) error: %s", fd_, strerror(errno));
        return -1;
    }
    //LOGD("received fd(%d) data: %.*s", fd_, size, buffer);
    poller_t* poller = tcp_server_->getPoller();
    poller->event_op(fd_, poller, OPERATION_MOD, EVENT_READ | EVENT_WRITE, this);
    if ((++count % 10000) == 0) {
        LOGD("count: %lu", count);
    }
    return 0;
}

int TcpSession::onWrite()
{
    char buffer[2] = {'o', 'k'};
    int size = 2;
    ssize_t rc = ::send(fd_, buffer, size, 0);
    if (rc == -1) {
        LOGD("send fd(%d) error: %s", fd_, strerror(errno));
        return -1;
    }
    //LOGD("send fd(%d) data: %.*s", fd_, size, buffer);
    poller_t* poller = tcp_server_->getPoller();
    poller->event_op(fd_, poller, OPERATION_MOD, EVENT_READ, this);
    return 0;
}

int TcpSession::handle(int type)
{
    if ((type & EVENT_READ)) {
        if (onRead() < 0) {
            delete this;
            return -1;
        }
    }

    if ((type & EVENT_WRITE)) {
        if (onWrite() < 0) {
            delete this;
            return -1;
        }
    }
    return 0;
}