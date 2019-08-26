#include "tcp_server.h"
#include "log.h"
#include "../src/util.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <thread>
#include <functional>
#include <thread>
#include <vector>

const std::string kHost = "172.28.43.2";
constexpr uint16_t kListenPort = 60000;
void clientThread()
{
    struct sockaddr_in addr; 
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(kHost.c_str());
    addr.sin_port = htons(kListenPort);
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        return;
    }
    if (::connect(fd, (const struct sockaddr *)&addr, sizeof(addr)) < 0) {
        LOGD("failed to connect, error: %s", strerror(errno));
        return;
    }
    set_nodelay(fd, 1);
    set_nonblock(fd, 0);
    do {
        char hello[5] = {'h', 'e', 'l', 'l', 'o'};
        char ok[2] = {0};
        if (::send(fd, hello, 5, 0) != 5) {
            LOGD("send to %s:%d failed, error: %s", kHost.c_str(), kListenPort, strerror(errno));
            abort();
        }
        //LOGD("send to %s:%d, data: %.*s", kHost.c_str(), kListenPort, 5, hello);
        if (::recv(fd, ok, 2, 0) != 2) {
            LOGD("recv %s:%d failed, error: %s", kHost.c_str(), kListenPort, strerror(errno));
            abort();
        }
        //LOGD("recv from %s:%d, data: %.*s", kHost.c_str(), kListenPort, 2, ok);
        //std::this_thread::sleep_for(std::chrono::seconds(3));
    } while (true);
}

int main(int argc, const char* args[])
{
    TcpServer server(kHost.c_str(), kListenPort, 128);
    server.init();

    std::thread ts(std::bind(&TcpServer::start, &server));
    std::vector<std::thread> clients;
    for (int i = 0; i < 8; i++) {
        clients.push_back(std::thread(clientThread));
    }
    ts.join();
    for (auto& t : clients) {
        t.join();
    }
    return 0;
}