#include"channel.h"
#include<stdio.h>
#include<stdlib.h>

struct Channel *channelInit(int fd, int events, handleFunc *readFunc, handleFunc *writeFunc, void *arg)
{
    struct Channel* ch = (struct Channel*)malloc(sizeof(struct Channel));
    ch->fd = fd;
    ch->events = events;
    ch->readCallback = readFunc;
    ch->writeCallback = writeFunc;
    ch->arg = arg;
    return ch;
}

void writeEventEnable(struct Channel *channel, bool flag)
{
    if (flag)
    {
        channel->events |= WriteEvent;
    }
    else
    {
        channel->events = channel->events & ~WriteEvent;
    }
}

bool isWriteEventEnable(struct Channel *channel)
{
    return channel->events & WriteEvent;
}
