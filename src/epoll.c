#include "epoll.h"
#include <sys/epoll.h>
#include <stdlib.h>
#include <unistd.h>

struct epoll_poller_s {
    struct poller_s poller;
    int epfd;
    struct epoll_event* events; 
};

int epoll_event_operation(int fd, 
                          poller_t* poller, 
                          int operation,
                          int event_type, 
                          void* data)
{
    struct epoll_event ev;
    struct epoll_poller_s* epoll_poller = (struct epoll_poller_s*)poller; 
    ev.data.fd = fd;
    ev.data.ptr = data;
    ev.events = 0;
    if (event_type & EVENT_READ) {
        ev.events |= EPOLLIN;
    }
    if (event_type & EVENT_WRITE) {
        ev.events |= EPOLLOUT;
    }
    ev.events |= EPOLLET;
    int rc = 0;
    switch (operation)
    {
    case OPERATION_ADD:
        rc = epoll_ctl(epoll_poller->epfd, EPOLL_CTL_ADD, fd, &ev);
        break;
    case OPERATION_MOD:
        rc = epoll_ctl(epoll_poller->epfd, EPOLL_CTL_MOD, fd, &ev);
        break;
    case OPERATION_DEL:
        rc = epoll_ctl(epoll_poller->epfd, EPOLL_CTL_DEL, fd, &ev);
        break;
    default:
        rc = -1;
        break;
    }
    return rc;
}

int epoll_dispatch(poller_t* poller, void* events, int size)
{
    struct epoll_poller_s* epoll_poller = (struct epoll_poller_s*)poller;
    struct epoll_event* e = events;
    for (int i = 0; i < size; i++) {
        uint32_t revents = e->events;
        /* add EPOLLIN and EPOLLOUT events to handle the error*/
        if (revents & (EPOLLERR | EPOLLHUP)) {
            revents |= (EPOLLIN | EPOLLOUT);
        }

        int type = 0;
        /* read event */
        if (revents & EPOLLIN) {
            type |= EVENT_READ;
        }

        /* write event */
        if (revents & EPOLLOUT) {
            type |= EVENT_WRITE;
        }
        epoll_poller->poller.handle_event(e->data.ptr, type);
        e++;
    }
}

int epoll_loop(poller_t* poller)
{
    struct epoll_poller_s* epoll_poller = (struct epoll_poller_s*)poller;
    do {
        int rc = epoll_wait(epoll_poller->epfd, epoll_poller->events, epoll_poller->poller.poll_size, epoll_poller->poller.timeout);
        /* error when rc == -1 */
        if (rc == -1) {
            return -1;
        }
        if (rc > 0) {
            epoll_poller->poller.dispatch(poller, epoll_poller->events, rc);
        }
        
    } while (1);
    return 0;
}

poller_t* poller_epoll_create()
{
    struct epoll_poller_s* epoll_poller = (struct epoll_poller_s*)malloc(sizeof(struct epoll_poller_s));
    if (epoll_poller != NULL) {
        epoll_poller->poller.listen_fd = -1;
        epoll_poller->poller.timeout = -1;
        epoll_poller->poller.poll_size = 128;
        epoll_poller->poller.event_op = epoll_event_operation;
        epoll_poller->poller.loop = epoll_loop;
        epoll_poller->poller.dispatch = epoll_dispatch;
        epoll_poller->epfd = -1;
        epoll_poller->events = NULL;
    }
    return (poller_t*)epoll_poller;
}

int poller_epoll_initialize(poller_t* poller)
{
    struct epoll_poller_s* epoll_poller = (struct epoll_poller_s*)poller;
    epoll_poller->events = (struct epoll_event*)malloc(sizeof(struct epoll_event) * epoll_poller->poller.poll_size);
    if (epoll_poller->events == NULL) {
        return -1;
    }
    int epfd = epoll_create(1);
    if (epfd == -1) {
        return -1;
    }
    epoll_poller->epfd = epfd;
    return 0;
}

int poller_epoll_uninitialize(poller_t* poller)
{
    struct epoll_poller_s* epoll_poller = (struct epoll_poller_s*)poller;
    if (epoll_poller == NULL) {
        return -1;
    }
    close(epoll_poller->poller.listen_fd);
    if (epoll_poller->events) {
        free(epoll_poller->events);
    }
    free(epoll_poller);
    return 0;
}