#pragma once
#include "../src/poller.h"
#include "poller_event_handler.h"

class TcpSession : public IPollerEventHandler{
public:
    TcpSession(int fd, poller_t* poller);
    ~TcpSession();
    int onRead();
    int onWrite();

    int handle(int type) override;
private:
    int fd_;
    poller_t* poller_;
};