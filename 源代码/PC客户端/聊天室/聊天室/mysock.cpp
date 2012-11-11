#include "mysock.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <winsock2.h>
#include "stdafx.h"

bool InitSocklib()
{
	WSADATA wsaData;
	int ret;
	if ((ret=WSAStartup(MAKEWORD(2,2),&wsaData))!=0)
	{
		printf("WSAStartup failed with error %d\n", ret);
		return false;
	}
	return true ;
}

void DestorySocklib()
{
	WSACleanup();
}

void sockError(const char *format,...)
{
	int errno;
	va_list	args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	errno = WSAGetLastError();

	switch (errno)
	{
		case WSAEADDRINUSE:
			fprintf(stderr,"WSAEADDRINUSE:The specified address is already in use.");
			break;
		case WSAEADDRNOTAVAIL:
			fprintf(stderr,"WSAEADDRNOTAVAIL:The specified address is not available from the local machine.");
			break;
		case WSAEAFNOSUPPORT:
			fprintf(stderr,"WSAEAFNOSUPPORT:Addresses in the specified family cannot be used with this socket.");
			break;
		case WSAECONNREFUSED:
			fprintf(stderr,"WSAECONNREFUSED:The attempt to connect was forcefully rejected.");
			break;
		case WSAEDESTADDRREQ:
			fprintf(stderr,"WSAEDESTADDRREQ:A destination address is required.");
			break;
		case WSAEFAULT:
			fprintf(stderr,"WSAEFAULT:The lpSockAddrLen argument is incorrect.");
			break;
		case WSAEINVAL:
			fprintf(stderr,"WSAEINVAL:The socket is already bound to an address.");
			break;
		case WSAEISCONN:
			fprintf(stderr,"WSAEISCONN:The socket is already connected.");
			break;
		case WSAEMFILE:
			fprintf(stderr,"WSAEMFILE:No more file descriptors are available.");
			break;
		case WSAENETUNREACH:
			fprintf(stderr,"WSAENETUNREACH:The network cannot be reached from this host at this time.");
			break;
		case WSAENOBUFS:
			fprintf(stderr,"WSAENOBUFS:No buffer space is available. The socket cannot be connected.");
			break;
		case WSAENOTCONN:
			fprintf(stderr,"WSAENOTCONN:The socket is not connected.");
			break;
		case WSAENOTSOCK:
			fprintf(stderr,"WSAENOTSOCK:The descriptor is a file, not a socket.");
			break;
		case WSAETIMEDOUT:
			fprintf(stderr,"WSAETIMEDOUT:The attempt to connect timed out without establishing a connection. ");
			break;
		default:
			fprintf(stderr,"WSAEError: Unknown! ");
			break;	
	}
	fprintf(stderr,"\n");
}

SOCKET connectSock(const char *host, const char *service , const char *transType)
{
	SOCKET s;
	struct sockaddr_in sockAddr;
	int type,protocol;
	struct hostent	*phe;	
	struct servent	*pse;	

	memset(&sockAddr,0,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;

	if (pse = getservbyname(service,transType))				//如果service输入是服务名称
	{
		sockAddr.sin_port = pse->s_port;
	}

	else if( (sockAddr.sin_port = htons((u_short)atoi(service)) )== 0)			//如果service输入是端口号
	{
		fprintf(stderr,"Can't get \"%s\" port number or service entry\n", service);
		return INVALID_SOCKET;
	}

	if (phe = gethostbyname(host))													//如果host输入是域名
	{
		memcpy(&sockAddr.sin_addr , phe->h_addr, phe->h_length);
	}
	
	else if ( (sockAddr.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)						//host是IP地址
	{
		fprintf(stderr,"Can't get \"%s\" IP address or host entry \n", host);
		return INVALID_SOCKET;
	}

	if (strcmp(transType, "udp") == 0)
	{
		type = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
	}
	else
	{
		type = SOCK_STREAM;
		protocol = IPPROTO_TCP;
	}

	if (	(s = socket(AF_INET , type ,protocol)) == INVALID_SOCKET)           //初始化s
	{
		sockError("Can't create socket!");
		return INVALID_SOCKET;
	}
	
	if (connect(s, (struct sockaddr *)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR)
	{
		fprintf(stderr,"Can't connect to %s.%s:\n",  host, service);
		sockError("");
		return INVALID_SOCKET;
	}
	return s;

}

SOCKET connectTCP(const char *host , const char *service)										//连接TCP套接字
{
	return connectSock(host, service, "tcp");
}

SOCKET connectUDP(const char *host , const char *service)										//连接UDP套接字
{
	return connectSock(host ,service ,"udp");
}

SOCKET passiveSock(const char *service , const char *transType , int qlen)						//建立被动套接字
{
	SOCKET s;
	struct sockaddr_in sockAddr;
	int type,protocol;
	struct servent	*pse;

	memset(&sockAddr,0 ,sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;
	sockAddr.sin_addr.s_addr = INADDR_ANY;

	if ( pse =getservbyname(service ,transType))					//如果是服务名称
	{
		sockAddr.sin_port = pse->s_port ;
	}

	else if ( (sockAddr.sin_port = htons((u_short)atoi(service))) ==0)
	{
		fprintf(stderr,"Can't get \"%s\" port number or service entry\n", service);
		return INVALID_SOCKET;
	}
	
	if (strcmp(transType, "udp") == 0)
	{
		type = SOCK_DGRAM;
		protocol = IPPROTO_UDP;
	}
	else
	{
		type = SOCK_STREAM;
		protocol = IPPROTO_TCP;
	}

	if ((s = socket(AF_INET,type,protocol)) == INVALID_SOCKET)   //初始化
	{
		sockError("Can't create socket!");
		return INVALID_SOCKET;
	}

	if (bind(s,(struct sockaddr *)&sockAddr,sizeof(sockAddr)) == SOCKET_ERROR) //绑定
	{
		fprintf(stderr,"Can't bind to port: %d \n",  service);
		sockError("");
		return INVALID_SOCKET;
	}

	if (type == SOCK_STREAM && listen(s,qlen) == SOCKET_ERROR)				//如果是TCP监听
	{
		fprintf(stderr,"Can't listen to port: %d \n",  service);
		sockError("");
		return INVALID_SOCKET;
	}
	return s;

}

SOCKET passiveTCP(const char *service ,int qlen)														//建立被动TCP套接字并监听
{
	return passiveSock(service,"tcp",qlen);
}

SOCKET passiveUDP(const char *service)																//建立被动UDP套接字
{
	return passiveSock(service, "udp", 0);
}

int readChar(SOCKET s,char *ptr)
{
	static int readCount = 0;
	static char *readPtr;
	static char readBuff[BUFF_SIZE];

	if (readCount <= 0)
	{
		readCount = recv(s , readBuff , BUFF_SIZE, 0 );
		if (readCount <=0)			//eof或者错误
		{
			return readCount;							//返回小于0的值
		}

		readPtr =readBuff;
	}

	readCount --;
	*ptr = *readPtr++;
	return 1;
}

int readLine(SOCKET s ,char FAR *buff, int maxSize)
{
	char c,*ptr;
	int n,rc;

	ptr = buff;

	for( n = 1 ; n <= maxSize ; n++)
	{
		rc = readChar(s ,&c);
		if (rc = 1)
		{
			if (c =='\n')
				break;
			*ptr++ = c ;
		}

		else 
		{
			break;
		}
	}

	*ptr = 0;
	return n;
}

int writeSocket(SOCKET s , char FAR *buff ,int n)
{
	int count = 0;
	int sc;
	while(count < n)
	{
		sc = send(s ,buff+count ,n-count,0);
		if (sc < 0)
			return sc;
		if (sc == 0)
			Sleep(100);
		count += sc;
	}
	return count;
}

int readSocket(SOCKET s , char FAR *buff , int n)
{
	int count = 0 ;
	int rc ;
	while (count <n)
	{
		rc = recv(s ,buff+count ,n-count ,0);
		if (rc <0)
			return rc;
		if (rc ==100)
			Sleep(100);
		count += rc;
	}
	return count;
}