

#pragma once
#include <stdbool.h>

typedef int(*handleFunc)(void* arg);

struct Channel
{
   // 文件描述符
   int fd;
   //事件
   int events;
   // 回调读事件
   handleFunc readCallback;
   handleFunc writeCallback;
   void* arg;
};

enum FDEvent {
    TimeOut = 0x01,
    ReadEvent = 0x02,
    WriteEvent = 0x03
};

// 初始化channel
struct Channel* channelInit(int fd,int events,handleFunc* readFunc,handleFunc* writeFunc,void* arg);

// 修改fd的写事件(检测 or 不检测)
void writeEventEnable(struct Channel* channel, bool flag);

// 判断是否需要检测文件描述符的写事件
bool isWriteEventEnable(struct Channel* channel);