#include <assert.h>
#include "CtrlSocket.h"


CtrlSocket::CtrlSocket()
{
	initCtrlSocket();
}

CtrlSocket::~CtrlSocket()
{
	if (s != INVALID_SOCKET)
	{
		closesocket(s);
		s = INVALID_SOCKET;
	}
}

void CtrlSocket::initCtrlSocket()
{
	s = INVALID_SOCKET;
	readCount = 0;
	readCurPtr = readBuff;
	socketType = SOCKET_TYPE_NULL;
	loadStatus = LOAD_STATUS_NULL;
}

bool CtrlSocket::isOpen()
{
	return (s != INVALID_SOCKET);
}

bool CtrlSocket::clientTCP(const char *host, const char *port)
{
	assert(!isOpen());
	s = ::connectTCP(host ,port);
	socketType = SOCKET_TYPE_CLIENT;
	return (s != INVALID_SOCKET);
}

bool CtrlSocket::serverTCP(const char *port, int qlen)
{
	assert(!isOpen());
	s = ::passiveTCP(port ,qlen);
	socketType = SOCKET_TYPE_SERVER;
	return (s != INVALID_SOCKET);
}

bool CtrlSocket::acceptTCP(SOCKET servS)
{
	s = accept(servS ,NULL,NULL);
	socketType = SOCKET_TYPE_ACCEPT;
	return (s != INVALID_SOCKET);
}

int CtrlSocket::sendCmd(char *buff)
{
	assert(isOpen());
	int sendCount = 0;
	
	strcat(buff, "\r\n");
	sendCount = writeSocket(s,buff,strlen(buff));
	return sendCount;
}

int CtrlSocket::recvCmd(char *buff)
{
	assert(isOpen());

	char *readPtr;
	int n = 0;
	readPtr = buff;
	while (1)
	{
		if (readCount ==0)
		{
			readCount = recv(s ,readBuff,sizeof(readBuff),0);
			if (readCount <= 0)
			{
				this ->close();
			    //break;
				return readCount;
			}
			else 
			{
				readCurPtr = readBuff;
			}
		}

		else if (readCount <0)
		{
			return -1;
		}

		else 
		{
			if (*readCurPtr == '\n')
			{
				*readPtr++ = *readCurPtr++;
				readCount --;
				n++;
				break;
			}
			
			else
			{
				*readPtr++ =*readCurPtr++;
				readCount --;
				n++;
			}
		}
	}
	*readPtr = '\0';
	return n;
}


void CtrlSocket::close()
{
	assert(isOpen());
	closesocket(s);
	initCtrlSocket();
}

SOCKET CtrlSocket::getSocketHandler()
{
	return this->s;
}

void CtrlSocket::changeLoadStatus(int i)
{
	this ->loadStatus = i;
}

int CtrlSocket::getLoadStatus()
{
	return this ->loadStatus;
}

int CtrlSocket::getSocketType()
{
	return this ->socketType;
}