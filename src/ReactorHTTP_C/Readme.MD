## reactor流程

1.初始化server，server里有通信描述符listenfd

2.主线程，将listenfd封装为一个channel(事件检测读事件)，将channel添加到主线程eventloop->channelmap中，将listenfd(事件为EPOLL_IN)通过epoll_event添加到eventloop->EpollData->epfd中（epolldata初始化）

3.运行主线程反应堆模型，开始dispatcher(检测读写事件fd),通过channelmap找到fd对应的callback函数，开始执行。第一次是读回调，建立连接,调用注册好的acceptConnection，获得cfd，并且从线程池中选一个子线程,获取主线程的反应堆(takeWorkerEventLoop)，交给子线程去处理cfd(tcpConnectionInit).

4.

## 运行流程
C使用malloc实例化类，通过函数初始化类，
1. main.c   函数tcpServerInit(port, threadNum)  tcpServer服务器实例化, 并初始化 listener fd(创建，端口复用，绑定，监听) ，初始化mainLoop（函数eventLoopInit()），threadPool
2. t

## CPP 运行
1. main.cpp  类初始化TcpServer(port, threadNum),

