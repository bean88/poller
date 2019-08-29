#include "poller.h"

#ifdef __cplusplus
extern "C" {
#endif
poller_t* epoll_poller_create();
int epoll_poller_initialize(poller_t* poller);
int epoll_poller_uninitialize(poller_t* poller);

#ifdef __cplusplus
}
#endif