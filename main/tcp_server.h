#include "../src/epoll.h"
#include "poller_event_handler.h"
#include <inttypes.h>
#include <string>
#include <set>

class TcpServer : public IPollerEventHandler{
public:
    TcpServer(const char* host, uint16_t local_port, int backlog);
    ~TcpServer();
    int init();
    void start();
    poller_t* getPoller() { return poller_; }

    int handle(int type) override;
private:
    int accept();
    std::string host_;
    uint16_t local_port_;
    int backlog_;
    poller_t* poller_;
};