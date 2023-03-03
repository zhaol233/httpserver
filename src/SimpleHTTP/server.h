#pragma once

// 新建监听描述符
int initListenFD(unsigned short port);

// 新建epoll
int epollRun(int lfd);
// 接受连接
int acceptClient(int lfd,int epfd);
// 开始通信
int recvHttpRequest(int cfd,int epfd);

// 解析请求行
int parseRequestLine(const char* line,int cfd);

// 发送文件内容
int sendFile(const char* filename,int cfd);

//发送响应头 状态行，响应头
int sendHeaderMsg(int cfd,int status,const char* desc,const char* content_type,int length);

// 根据文件后缀获取content-type
const char* getFileType(const char* name);

//发送目录
int sendDir(const char* dirName, int cfd);

