#include "dispatcher.h"

#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define Max 1024
struct PollData
{
    int maxfd;
    struct pollfd fds[Max];
};

static void* pollInit();
static int pollAdd(struct Channel* channel, struct EventLoop* evLoop);
static int pollRemove(struct Channel* channel, struct EventLoop* evLoop);
static int pollModify(struct Channel* channel, struct EventLoop* evLoop);
static int pollDispatch(struct EventLoop* evLoop, int timeout); // 单位: s
static int pollClear(struct EventLoop* evLoop);
static int pollCtl(struct Channel* channel, struct EventLoop* evLoop, int op);

struct Dispatcher PollDispatcher = {
    pollInit,
    pollAdd,
    pollRemove,
    pollModify,
    pollDispatch,
    pollClear
};

// epoll 输出化,epollData

static void* pollInit()
{   
    
    struct PollData* data = (struct PollData*)malloc(sizeof(struct PollData));
    data->maxfd = 0;
    for(int i=0;i<Max;i++){
        data->fds[i].fd=-1;
        data->fds[i].events=0;
        data->fds[i].revents=0;
    }
    return data;
}

static int pollAdd(struct Channel* channel, struct EventLoop* evLoop)
{ 
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int events = 0;
    if (channel->events & ReadEvent)
    {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent)
    {
        events |= POLLOUT;
    }
    int i = 0;
    for (; i < Max; ++i)
    {
        if (data->fds[i].fd == -1)
        {
            data->fds[i].events = events;
            data->fds[i].fd = channel->fd;
            data->maxfd = i > data->maxfd ? i : data->maxfd;
            break;
        }
    }
    if (i >= Max)
    {
        return -1;
    }
    return 0;
}

static int pollRemove(struct Channel* channel, struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int events = 0;
    if (channel->events & ReadEvent)
    {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent)
    {
        events |= POLLOUT;
    }
    int i=0;
    for(;i<Max;i++){
        if(data->fds[i].fd=channel->fd){
            data->fds[i].events=0;
            data->fds[i].revents=0;
            data->fds[i].fd=-1;
            break;
        }
    }
    if(i>=Max){
        return -1;
    }
    return 0;
}

static int epollModify(struct Channel* channel, struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int events = 0;
    if (channel->events & ReadEvent)
    {
        events |= POLLIN;
    }
    if (channel->events & WriteEvent)
    {
        events |= POLLOUT;
    }
    int i = 0;
    for (; i < Max; ++i)
    {
        if (data->fds[i].fd == channel->fd)
        {
            data->fds[i].events = events;
            data->fds[i].fd = channel->fd;
            break;
        }
    }
    if (i >= Max)
    {
        return -1;
    }
    return 0;
}

static int pollDispatch(struct EventLoop* evLoop, int timeout)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    int count = poll(data->fds, data->maxfd+1, timeout*1000);
    if(count==-1){
        perror("poll error");
        exit(0);
    }
    for (int i = 0; i < count; ++i)
    {

        int fd = data->fds[i].fd;
        if(fd==-1) continue;
        if ( data->fds[i].events & POLLIN)
        {
            // eventActivate(evLoop, fd, ReadEvent);
        }
        if (data->fds[i].events & POLLOUT)
        {
            // eventActivate(evLoop, fd, WriteEvent);
        }
    }
    return 0;
}

static int epollClear(struct EventLoop* evLoop)
{
    struct PollData* data = (struct PollData*)evLoop->dispatcherData;
    free(data);
    return 0;
}
