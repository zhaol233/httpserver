#include<stdio.h>
#include<stdlib.h>   // atoi
#include<string.h>
#include<unistd.h>
#include"server.h"


int main(int argc,char* argv[]){
    if(argc<3){
        printf("./a.out port path \n");
        return 0;
    }

    chdir(argv[2]);
    unsigned short port = atoi(argv[1]);
    int lfd = initListenFD(port);
    
    epollRun(lfd);

    // 启动服务器程序

    return 0;
}