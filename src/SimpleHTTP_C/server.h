#pragma once

// 新建监听描述符
int initListenFD(unsigned short port);

// 新建epoll
int epollRun(int lfd);
// 接受连接
void* acceptClient(void* arg);

// 开始通信
void* recvHttpRequest(void* ar);

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

// 十六进字符转换成十进制数
int hexToDec(char c);

// 解码中文
void decodeMsg(char* to, char* from);



