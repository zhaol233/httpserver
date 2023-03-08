#pragma once
#include<stdbool.h>

struct ChannelMap{
    struct Channel** list;
    int size;
};

// 初始化
struct ChannelMap* channelMapInit(int size);
//清空channelmap
void ChannelMapClear(struct ChannelMap* map);

bool makeMapRoom(struct ChannelMap* map,int new_size,int unitSize);