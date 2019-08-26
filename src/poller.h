#pragma once

#define OPERATION_ADD 0
#define OPERATION_MOD 1
#define OPERATION_DEL 2

#define EVENT_READ 1
#define EVENT_WRITE 2

typedef struct poller_s poller_t;
typedef int(*poller_event_operation_func)(int, poller_t*, int, int, void*);
typedef int (*loop_func)(poller_t*);
typedef int (*dispatch_func)(poller_t*, void* events, int size);
typedef int (*event_func)(void*, int);

struct poller_s {
    int listen_fd;
    int timeout;
    int poll_size;
    poller_event_operation_func event_op;
    loop_func loop;
    dispatch_func dispatch;
    event_func handle_event;
};