#pragma once
#include "../src/poller.h"
#include "poller_event_handler.h"

class TcpServer;

class TcpSession : public IPollerEventHandler{
public:
    TcpSession(int fd, TcpServer* tcp_server);
    ~TcpSession();
    int onRead();
    int onWrite();

    int handle(int type) override;
private:
    int fd_;
    TcpServer* tcp_server_;
};