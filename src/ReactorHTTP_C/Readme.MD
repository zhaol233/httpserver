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
1. main.cpp  类初始化TcpServer(port, threadNum),新建主反应堆模型main eventloop，主反应堆模型选择dispatch,初始化线程池,设置监听listen fd。
2. server.run()-->m_threadpool.run()，线程池中新建子线程workerthread，并且运行子线程。每个子线程中也有一个eventloop。eventloop中的有一个通信描述符用于读取数据。
3. m_mainLoop->addTask **主线程**：将listenfd封装为一个channel，Channel检测**读**事件,回调函数为TcpServer.acceptConnection。所有的连接通过主反应堆模型的检测listenfd，建立新的连接，进入回调函数。

4. 在acceptConnction中处理客户端的连接。从TcpServer.m_threadPool线程池中取出一个子线程的eventloop,新建一个TcpConnection(cfd,eventloop)。

5. TcpConnect()类中将客户端通信描述符cfd包裹为一个m_channel，事件是**读**事件,添加processRead, processWrite, destroy函数, 并把类作为参数传递进去this。然后把这个m_channel放到取出的子线程的eventloop中的任务队列m_taskQ中，,在任务队列中标注channel的类型是新增还是删除还是修改。初始肯定是新增任务。

6. 子线程eventloop添加任务addTask时，如果是主线程添加的，调用taskWakeup，通知taskWakeup子线程来任务了。如果是子线程添加的，直接处理processTask。

7. 子线程processTask。取出任务队列m_taskQ中的ChannelElement，通过任务类型判断操作方式，初始默认是新增。把channel添加到m_channelMap<fd,channel>,添加到dispatch的epfd中进行检测。

8. thread

将channel添加到主线程eventloop->channelmap中，将listenfd(事件为EPOLL_IN)通过epoll_event添加到eventloop->EpollData->epfd中（epolldata初始化）


```C++
// epoll相关操作
epoll_create(int size) -> int  // epoll_create()会在内存中开辟一块空间，用来存放epoll中fd(epfd)，参数size只是对内核初始分配内部数据结构的一个建议
epoll_ctl(int epfd,int op,int fd,epoll_event *events)   // 会把每一个socket的fd数据结构放到epoll_create()创建内存空间epfd;
epoll_wait(int epfd,epoll_event *events,int maxevents,int timeout) // 只有当epoll_create()中创建的内存空间中的fd有事件发生，才会把这些fd放到就绪链表中（封装了有事件发生的socket链表），并返回就绪链表的个数
```

```C++
// poll相关操作
```

```C++
// Select相关操作
fd_set rset;  // 初始化
int fd =  select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int FD_ZERO(int fd, fd_set *fdset);   //一个 fd_set类型变量的所有位都设为 0
int FD_CLR(int fd, fd_set *fdset);    //清除某个位时可以使用
int FD_SET(int fd, fd_set *fd_set);   //设置变量的某个位置位
int FD_ISSET(int fd, fd_set *fdset);  //测试某个位是否被置位
```

```mutex
std::unique_lock对象以独占所有权的方式(unique owership)管理mutex对象的上锁和解锁操作，即在unique_lock对象的声明周期内，它所管理的锁对象会一直保持上锁状态；而unique_lock的生命周期结束之后，它所管理的锁对象会被解锁。

```
