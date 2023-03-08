#include"server.h"
#include<stdio.h>   // sprintf
#include<sys/socket.h>  // socket,setsockopt, bind, listen
#include<sys/types.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/stat.h>
#include<sys/sendfile.h>

#include<fcntl.h>
#include<memory.h>
#include<strings.h>   
#include<string.h>  // strstr
#include<errno.h>
#include<assert.h>
#include<dirent.h>
#include<unistd.h>
// #include<memory.h>
#include<malloc.h>   // free
#include<pthread.h>  
#include<ctype.h>   // isxdigit


int initListenFD(unsigned short port){
    // 创建 套接字
    int lfd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    if(lfd==-1){
        perror("socket create error");
        return -1;
    }
    //端口复用
    int opt=1024;
    int ret = setsockopt(lfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    if(ret==-1){
        perror("setsockopt error");
        return -1;
    }

    //绑定
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));       //每个字节都用0填充
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;
    // addr.sin_addr.s_addr = inet_addr("172.25.146.96");

    ret = bind(lfd,(struct sockaddr*)&addr,sizeof(addr));
    if(ret==-1){
        perror("setsockopt error");
        return -1;
    }

    // 监听
    ret = listen(lfd,128);
    if(ret==-1){
        perror("setsockopt error");
        return -1;
    }
    return lfd;
}

struct FDInfo
{
    pthread_t tid;
    int fd;
    int epfd;

};


int epollRun(int lfd)
{   
    int epfd = epoll_create(1);
    if(epfd==-1){
        perror("epoll_create error");
        return -1;
    }
    struct epoll_event ev;
    ev.data.fd = lfd;
    ev.events = EPOLLIN;
    int ret = epoll_ctl(epfd,EPOLL_CTL_ADD, lfd, &ev);

    if(ret==-1){
        perror("epoll_ctl error");
        return -1;
    }
    struct epoll_event evs[1024];
    int size = sizeof(evs)/sizeof(struct epoll_event);

    while(1){
        int num = epoll_wait(epfd,evs,size,-1);
        for(int i=0;i<num;++i){

            int fd = evs[i].data.fd;
            struct FDInfo* info = (struct FDInfo*)malloc(sizeof(struct FDInfo));
            info->epfd = epfd;
            info->fd = fd;
            
            if(fd == lfd){
                //建立新连接
                // acceptClient(lfd,epfd);
                pthread_create(&info->tid,NULL,acceptClient,info);
            }else{
                // 读数据
                pthread_create(&info->tid,NULL,recvHttpRequest,info);
            }
        }
    }
    return 0;
}

void* acceptClient(void* arg)
{
    struct FDInfo* info = (struct FDInfo*)arg;
    printf("建立连接线程：%ld\n",pthread_self());
    int cfd = accept(info->fd,NULL,NULL);
    if(cfd==-1){
        perror("accept error");
        return NULL;
    }
    // 设置边沿非阻塞
    int flag = fcntl(cfd,F_GETFL);
    flag |= O_NONBLOCK;
    fcntl(cfd,F_SETFL,flag);
    // 添加到epoll 模型

    struct epoll_event ev;
    ev.data.fd = cfd;
    ev.events=EPOLLIN | EPOLLET;
    int ret = epoll_ctl(info->epfd,EPOLL_CTL_ADD,cfd,&ev);

    if(ret==-1){
        perror("epoll_ctl add error..");
        return NULL;
    }
    free(info);

    return NULL;
}

void* recvHttpRequest(void* arg)
{   
    struct FDInfo* info = (struct FDInfo*)arg;

    printf("接收数据线程：%ld\n",pthread_self());
    int len = 0,total=0;
    char tmp[4096] = {0};
    char buff[4096] = {0};
    int size  = 4096;
    while((len =  recv(info->fd,tmp,size,0)) > 0){
        printf("len: %i\n",len);
            if(total+len<sizeof(buff)){
                memcpy(buff+total,tmp,len);
            }
            total += len;
    }
     // 判断数据是否接收完毕
    if(len == -1 && errno == EAGAIN){
        // 解析请求行
        char* pt = strstr(buff,"\r\n");
        int reqlen = pt-buff;
        buff[reqlen] = '\0';
        parseRequestLine(buff,info->fd);
    }
    else if(len==0){
        epoll_ctl(info->epfd,EPOLL_CTL_DEL,info->fd,NULL);
        close(info->fd);
    }
    else{
        perror("recv error...");
    }
    free(info);

    return 0;
}

int parseRequestLine(const char *line, int cfd)
{
    printf("parser header...\n");
    char method[12];
    char path[1024];
    sscanf(line,"%[^ ] %[^ ]",method,path);
    printf("method: %s, path: %s", method,path);
    if(strcasecmp(method,"GET")!=0){
        return -1;
    } else{
        decodeMsg(path,path);   
        char * file = NULL;
        // 相对路径
        if(strcmp(path,"/")==0){
            file = "./";
        }else{
            file = path + 1;
        }
        
        struct stat st;
        int ret = stat(file,&st);
        if(ret == -1){
            // 文件不存在，回复404
            sendHeaderMsg(cfd,404,"Not Found",getFileType(".html"),-1);
            sendFile("404.html",cfd);
            return 0;
        }
        if(S_ISDIR(st.st_mode)){
            //发送目录内容
            printf("返回目录内容\n");
            sendHeaderMsg(cfd,200,"OK",getFileType(".html"),-1);
            sendDir(file,cfd);
            printf("返回完成\n");

        }
        else{
            // sendRequestHeader()
            //发送文件内容
            printf("返回文件内容\n");
            sendHeaderMsg(cfd,200,"OK",getFileType(file),-1);
            sendFile(file,cfd);
        }

    }
    return 0;
}

int sendFile(const char *filename, int cfd)
{
    int fd = open(filename,O_RDONLY);
    assert(fd>0);
#if 0
    while(1){
        char buff[1024];
        int len = read(fd,buff,1024);
        if(len>0){
            send(cfd,buff,len,0);  // 发送到客户端
            usleep(10);            // 让接收端休息一下
        }if(len==0){
            break;
        }
        else{
            perror("read");
        }
    }
#else
    off_t offset = 0;
    int size = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    while (offset < size)
    {
        int ret = sendfile(cfd, fd, &offset, size - offset);   // 会自动修改offset的值
        // printf("ret value: %d\n", ret);
        // if (ret == -1 && errno == EAGAIN)
        // {
        //     // printf("没数据...\n");
        // }
    }
#endif
    close(fd);
    return 0;

}

int sendHeaderMsg(int cfd, int status, const char *desc, const char *content_type, int length)
{
    char buff[4096];
    sprintf(buff,"http/1.1 %d %s\r\n",status,desc);
    sprintf(buff+strlen(buff),"content-type: %s\r\n",content_type);
    sprintf(buff+strlen(buff),"content-length: %d\r\n\r\n",length);
    send(cfd,buff,strlen(buff),0);

    return 0;
}

const char* getFileType(const char *name)
{
    char* dot = strrchr(name,'.');
    if(dot==NULL){
        return "text/plain; charset=utf-8";
    }
    if(strcmp(name,".html")==0 || strcmp(name,".htm")==0){
        return "text/html; charset=utf-8";
    }
    if(strcmp(name,".jpg")==0 || strcmp(name,".jpeg")==0){
        return "image/jpeg";
    }
    if(strcmp(name,".gif")==0){
        return "image/gif";
    }
    if(strcmp(name,".css")==0){
        return "text/css";
    }
    if(strcmp(name,".au")==0){
        return "audio/basic";
    }
    if(strcmp(name,".wav")==0){
        return "audio/wav";
    }
    if(strcmp(name,".avi")==0){
        return "vidio/x-msvideo";
    }
    if(strcmp(name,".mov")==0 || strcmp(name,".qt")==0){
        return "vidio/quicktime";
    }
    if(strcmp(name,".mpeg")==0 || strcmp(name,".mpe")==0){
        return "vidio/mpeg";
    }
        if(strcmp(name,".mp3")==0){
        return "audio/mpeg";
    }

    return "text/plain; charset=utf-8";
}

int sendDir(const char *dirName, int cfd)
{
    struct dirent** namelist;
    char buff[4096];
    sprintf(buff,"<html><head><title>DIR: %s</title></head><body><table>",dirName);
    int num = scandir(dirName,&namelist,NULL,alphasort);
    printf("文件夹%s内文件数：%i\n",dirName, num);
    for(int i=0;i<num;i++){
        // 取出文件名, 指针的数组 struct dirent* tmp   和数组指针完全不同
        char* name = namelist[i]->d_name;
        struct stat st;
        char subpath[1024] = {0};
        sprintf(subpath,"%s/%s",dirName,name);

        stat(subpath,&st);
        if(S_ISDIR(st.st_mode)){
            sprintf(buff+strlen(buff), "<tr><td><a href=\"%s/\">%s</a></td><td>%ld</td></tr>", name,name,st.st_size);
        }else{
            sprintf(buff+strlen(buff), "<tr><td><a href=\"%s\">%s</a></td><td>%ld</td></tr>", name,name,st.st_size);
        }
        
        send(cfd,buff,strlen(buff),0);
        memset(buff,0,strlen(buff));
        free(namelist[i]);
    }
    sprintf(buff,"</table></body></html>");
    send(cfd,buff,strlen(buff),0);
    free(namelist);
    return 0;
}

//
int hexToDec(char c){
    if(c >='0' && c <= '9')
        return c- '0';
    if(c >='a' && c<='f')
        return c-'a' + 10;
    if(c >='A' && c <= 'F')
        return c- 'A' + 10;
    return 0;
}

void decodeMsg(char* to, char* from){
    for(;*from != '\0';++to,++from){
        // isxdigit -> 判断字符是不是16进制格式, 取值在 0-f
        // Linux%E5%86%85%E6%A0%B8.jpg
        if(from[0]=='%' && isxdigit(from[1]) && isxdigit(from[2])){
            // 将16进制的数 -> 十进制 将这个数值赋值给了字符 int -> char
            // B2 == 178
            // 将3个字符, 变成了一个字符, 这个字符就是原始数据
            *to = hexToDec(from[1])*16 + hexToDec(from[2]);
            from += 2;
        } 
        else{
            *to = *from;
        }
    }
    *to = '\0';
}