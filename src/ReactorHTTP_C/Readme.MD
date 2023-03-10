##reactor流程

1.初始化server，server里有通信描述符listenfd

2.主线程，将listenfd封装为一个channel(事件检测读事件)，将channel添加到主线程eventloop->channelmap中，将listenfd(事件为EPOLL_IN)通过epoll_event添加到eventloop->EpollData->epfd中（epolldata初始化）

3.运行主线程反应堆模型，开始dispatcher(检测读写事件fd),通过channelmap找到fd对应的callback函数，开始执行。第一次是读回调，建立连接,调用注册好的acceptConnection，获得cfd，并且从线程池中选一个子线程,获取主线程的反应堆(takeWorkerEventLoop)，交给子线程去处理cfd(tcpConnectionInit).

