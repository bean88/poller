#include "poller.h"

#ifdef __cplusplus
extern "C" {
#endif
poller_t* poller_epoll_create();
int poller_epoll_initialize(poller_t* poller);
int poller_epoll_uninitialize(poller_t* poller);

#ifdef __cplusplus
}
#endif