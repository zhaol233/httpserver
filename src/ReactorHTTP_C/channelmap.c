#include "channelmap.h"
#include<stdio.h>

struct ChannelMap *channelMapInit(int size)
{
    struct ChannelMap* map = (struct ChannelMap*)malloc(sizeof(struct ChannelMap));
    map->size = size;
    return map;
}

void ChannelMapClear(struct ChannelMap *map)
{
    if(!map){
        for(int i=0;i<map->size;++i){
            free(map->list[i]);
        }
    }
    free(map->list);
    map->list = NULL;
    map->size = 0;
}

bool makeMapRoom(struct ChannelMap *map, int new_size, int unitSize)
{
    int curSize = map->size;
    if(curSize < new_size){
        while(curSize<new_size){
            curSize *=2;
        }
        struct Channel** temp = realloc(map->list,curSize*unitSize);
        if(temp==NULL){
            return false;
        }
        map->list =temp;
        memset(&map->list[map->size],0,(curSize-map->size)*unitSize);
        map->size = curSize;
    }

    return true;
}
