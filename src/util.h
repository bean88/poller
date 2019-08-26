#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>

static int set_nodelay(int s, int on) {
  char v = (on ? 0 : 1);
  return setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(char));
}

static int set_nonblock(int s, int on) {
  int flag = fcntl(s, F_GETFL, 0);
  if (on) {
    flag |= O_NONBLOCK;
  } else {
    flag &= ~O_NONBLOCK;
  }
  return fcntl(s, F_SETFL, flag);
}