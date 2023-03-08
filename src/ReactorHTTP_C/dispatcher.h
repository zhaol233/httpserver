#pragma once

#include "channel.h"
#include"eventloop.h"



struct Dispatcher
{
    // init -- 初始化epoll, poll 或者 select 需要的数据块
    void* (*init)();
    // 添加channel
    int (*add)(struct Channel* channel, struct EventLoop* evLoop);
    // 删除channel
    int (*remove)(struct Channel* channel, struct EventLoop* evLoop);
    // 修改channel
    int (*modify)(struct Channel* channel, struct EventLoop* evLoop);
    // 事件监测
    int (*dispatch)(struct EventLoop* evLoop, int timeout); // 单位: s
    // 清除数据(关闭fd或者释放内存)
    int (*clear)(struct EventLoop* evLoop);
};