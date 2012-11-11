#ifndef __MYSOCK_H
#define __MYSOCK_H


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <winsock2.h>
#pragma   comment(lib, "Ws2_32.lib ") 

#define BUFF_SIZE 4096

bool InitSocklib();				//触发sock库
void DestorySocklib();		//关闭sock库

void sockError(const char *format,...);		//报告sock错误

SOCKET connectSock(const char *host, const char *service , const char *transType);           //连接套接字

SOCKET connectTCP(const char *host , const char *service);										//连接TCP套接字
SOCKET connectUDP(const char *host , const char *service);										//连接UDP套接字

SOCKET passiveSock(const char *service , const char *transType , int qlen);						//建立被动套接字

SOCKET passiveTCP(const char *service ,int qlen);														//建立被动TCP套接字并监听
SOCKET passiveUDP(const char *service);																	//建立被动UDP套接字

int readChar(SOCKET s ,char *ptr );																	//从SOCKET中读一个字节
int readLine(SOCKET s ,char FAR *buff ,int maxSize);													//从SOCKET中读一行

int writeSocket(SOCKET s , char FAR *buff ,int n);										//从SOCKET中写n个字节
int readSocket(SOCKET s , char FAR *buff , int n);
#endif