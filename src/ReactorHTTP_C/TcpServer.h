#pragma once
#include "EventLoop.h"
#include "ThreadPool.h"

// 监听文件描述符
struct Listener
{
    int lfd;
    unsigned short port;
};


struct TcpServer
{
    //线程个数
    int threadNum;
    // 主反应堆模型
    struct EventLoop* mainLoop;
    // 线程池
    struct ThreadPool* threadPool;
    // 监听文件描述符
    struct Listener* listener;
};

// 初始化
struct TcpServer* tcpServerInit(unsigned short port, int threadNum);
// 初始化监听文件描述符
struct Listener* listenerInit(unsigned short port);
// 启动服务器
void tcpServerRun(struct TcpServer* server);

