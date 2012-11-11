#ifndef __CTRLSOCKET_H
#define __CTRLSOCKET_H

#include "mysock.h"
#include <iostream>

class CtrlSocket
{
public:
	CtrlSocket();
	virtual ~CtrlSocket();

	bool isOpen();
	bool clientTCP(const char *host ,const char *port);
	bool serverTCP(const char *port ,int qlen);
	bool acceptTCP(SOCKET servS );

	SOCKET getSocketHandler();
	int getSocketType();
	void changeLoadStatus(int i);
	int getLoadStatus();

	int sendCmd(char *buff);
	int recvCmd(char *buff);

	void close();

public:
	typedef enum SOCKET_TYPE
	{
		SOCKET_TYPE_NULL,
		SOCKET_TYPE_CLIENT,
		SOCKET_TYPE_SERVER,
		SOCKET_TYPE_ACCEPT,
	} enumSOCKET_TYPE;

	typedef enum LOAD_STATUS
	{
		LOAD_STATUS_NULL,			//刚连接或者还未连接
		LOAD_STATUS_USER,           //已输入用户名
		LOAD_STATUS_LOADED,		//已登录
	}enumLOAD_STATUS;

protected :
	void initCtrlSocket();

protected:
	SOCKET s;
	char		readBuff[BUFF_SIZE];
	int		readCount;
	char     *readCurPtr;

	int		socketType;
	int		loadStatus;

};

#endif __CTRLSOCKET_H