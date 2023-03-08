#pragma once
#include"dispatcher.h"
#include<pthread.h>

struct EventLoop
{
    bool isQuit;
    struct Dispatcher* dispatcher;
    void* dispatcherData;
    // 任务队列
    struct ChannelElement* head;
    struct ChannelElement* tail;
    // map
    struct ChannelMap* channelMap;
    // 线程id, name, mutex
    pthread_t threadID;
    char threadName[32];
    pthread_mutex_t mutex;
    int socketPair[2];  // 存储本地通信的fd 通过socketpair 初始化
};