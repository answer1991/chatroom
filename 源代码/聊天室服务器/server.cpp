#include <winsock2.h>
#include <stdio.h>
#include <conio.h>
#include <process.h>
#include <windows.h>
#pragma   comment(lib, "Ws2_32.lib ") 

#define BUFFER_SIZE    2048
#define MAX_CONNECTION_COUNT	FD_SETSIZE

typedef struct SocketUsername
{
	char				username[20];
	SOCKET*	s;
}SocketUsername;



int  MainServer(SOCKET* s);
SOCKET     SocketArr[MAX_CONNECTION_COUNT];											
//SocketUsername  SocketUsernameArr[MAX_CONNECTION_COUNT];
char				username[MAX_CONNECTION_COUNT][50];
char userlist[1024];																											//用户名列表
unsigned 	serverCount = 0 ;

int main(int argc,char *argv[])
{
	WORD wVersionRequested= MAKEWORD(2,2);
	WSADATA									wsaData;
	int												PORT=3204; 
	SOCKET										server_socket;
	SOCKADDR_IN							server_addr;
	fd_set											 fdread;
	int												Ret;

	if (WSAStartup(wVersionRequested,&wsaData) != NO_ERROR)
	{
		printf("error at WSAStartup()\n");
		return 1;
	}
	
	//创建一个SOCKET，并绑定和监听；
	server_socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server_socket == INVALID_SOCKET)
	{
		printf("不能创建服务套接字! \n");
		WSACleanup();
		return 1;
	}
	
	server_addr.sin_family=AF_INET;
	if (argc==1)
	{
		server_addr.sin_addr.s_addr=INADDR_ANY;
	}
	else if (argc==2)
	{
		server_addr.sin_addr.s_addr=inet_addr(argv[1]);
	}

	else
	{
		printf ("参数输入有误，请检查输入");
		WSACleanup();
		return 1;
	}

	server_addr.sin_port=htons(PORT);
	
	if (bind(server_socket,(SOCKADDR *)&server_addr,sizeof(server_addr)) == SOCKET_ERROR)
	{
		printf("IP地址绑定有误，请检查输入  \n");
		WSACleanup();
		return 1;
	}
	
	if( listen(server_socket,5) ==SOCKET_ERROR)
	{
		printf("error at listen the socket! \n") ;
		WSACleanup();
		return 1;
	}

	else
	{
		printf("服务器已开启端口并监听！  \n");
	}

	//初始化操作
	for ( int i = 0 ; i <MAX_CONNECTION_COUNT ; i++ )
	{
		SocketArr[i] = -1;
		//SocketUsernameArr[i].s = &(SocketArr[i]);
		//memset( SocketUsernameArr[i].username , 0, sizeof( SocketUsernameArr[i].username ) );
	}
	//memset( userlist , 0, sizeof( userlist ) );

	while(!kbhit())
	{
		FD_ZERO(&fdread);
		FD_SET(server_socket , &fdread);
		for( i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
			if(SocketArr[i] != -1)
				FD_SET(SocketArr[i] , &fdread);

		if((Ret = select(0,&fdread,NULL,NULL,NULL)) == SOCKET_ERROR)
		{
			printf("select errror");
			break;
		}

		if( Ret > 0)
		{	

			if (FD_ISSET(server_socket , &fdread))
			{

				for(i = 0;i < MAX_CONNECTION_COUNT; i++)
					if(SocketArr[i] == -1)
						break;

				if ((SocketArr[i] = accept(server_socket, NULL , NULL )) == INVALID_SOCKET)
				{
					printf("Accept socket failed! ");
					break;
				}
				serverCount++;
				printf("%d.    有用户连接到服务器！\n" , serverCount);
			}

			for(i = 0;i < MAX_CONNECTION_COUNT; i++)
				if((SocketArr[i] != -1) && (FD_ISSET(SocketArr[i] , &fdread )))
				{
				  MainServer(&SocketArr[i]);						
				}
		}
	}
	return 0;
}

int  MainServer(SOCKET* s)
{
	SOCKET client_socket = *s;
	char receive_buff[BUFFER_SIZE];														//接收缓冲区
	char send_buff[BUFFER_SIZE];															//发送缓冲区
	int Ret;
	int i;

		
    Ret=recv(client_socket , receive_buff , sizeof(receive_buff) , 0);


	//退出聊天室
	if (Ret <= 0)
	{
		closesocket(*s);
		*s = -1;
		
		//通知所有用户某个用户离开聊天室
		for ( i = 0 ; i < MAX_CONNECTION_COUNT ; i++ )
		{
			if(&(SocketArr[i]) == s)
			{
				sprintf(send_buff , "%s\n%s\n%s\n%s%s", "13" ,  "SERVER" , "0"  , username[i] , " 离开了聊天室!");
				serverCount++;
				printf("%d.    %s 离开了聊天室!\n" , serverCount , username[i]);
				break;
			}
		}
		for(i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
		{
			if (SocketArr[i] != -1)
			{
				send(SocketArr[i] , send_buff , strlen(send_buff)+1, 0);
			}
		}
		
		//清空用户名
		for ( i = 0 ; i < MAX_CONNECTION_COUNT ; i++ )
		{
			if(&(SocketArr[i]) == s)
			{
				memset( username[i] , 0, sizeof( username[i] ) );
				break;
			}
		}

		//更新用户列表
		memset( userlist , 0, sizeof( userlist ) );
		for ( i = 0 ; i < MAX_CONNECTION_COUNT ; i++ )
		{
			if (SocketArr[i] != -1)
			{
				sprintf(userlist , "%s%s\n" , userlist , username[i]);
			}
		}
		
		//发送用户列表
		sprintf(send_buff , "%s\n%s\n%s\n%s" , "12" ,  "SERVER" , "0" , userlist);
		serverCount++;
		printf("%d.    用户列表有更新，新用户列表为：\n%s\n" , serverCount ,userlist);
		for(i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
		{
			if (SocketArr[i] != -1)
			{
				send(SocketArr[i] , send_buff , strlen(send_buff)+1 , 0);
			}
		}

		return Ret;
	}

	
	receive_buff[Ret] = '\0';
	int type=1000;
	char fromuser[20];
	char touser[20];
	char data[1024];
	sscanf(receive_buff  , "%d\n%[^\n]\n%[^\n]\n%[^\0]", &type , fromuser , touser , data);
	
	//type == 0 为请求注册
	if (type == 0)
	{
		//首先判断请求注册的用户名是否被注册
		bool isRegistered = false;
		for ( i = 0 ; i < MAX_CONNECTION_COUNT ; i++ )
		{
			if (strcmp(username[i] , fromuser) == 0)
			{
				isRegistered = true;
				break;
			}
		}
		if (isRegistered == false)
		{
			//写入用户名
			for ( i = 0 ; i < MAX_CONNECTION_COUNT ; i++ )
			{
				if( SocketArr[i] == *s )
				{
					memset( username[i] , 0, sizeof( username[i]) );
					sprintf(username[i] , "%s" , fromuser );
				}
			}
		
			//更新用户列表
			memset( userlist , 0, sizeof( userlist ) );
			for ( i = 0 ; i < MAX_CONNECTION_COUNT ; i++ )
			{
				if (SocketArr[i] != -1)
				{
					sprintf(userlist , "%s%s\n" , userlist , username[i]);
				}
			}

			//type ==10 为注册成功
			memset( send_buff , 0 , sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s%s", "10" ,  "SERVER" , touser , "注册用户成功，欢迎您 " , fromuser);
			send(client_socket , send_buff, strlen(send_buff)+1 , 0);
			
			//type == 12 为发送用户列表
			//更新用户列表
			memset( send_buff , 0, sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s" , "12" ,  "SERVER" , "0" , userlist);
			serverCount++;
			printf("%d.    用户列表有更新，新用户列表为：\n%s\n" , serverCount , userlist);
			for(i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
			{
				if (SocketArr[i] != -1)
				{
					send(SocketArr[i] , send_buff , strlen(send_buff)+1, 0);
				}
			}

			//type == 13 通知所有用户，有新用户加入或者离开
			memset( send_buff , 0, sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s%s", "13" ,  "SERVER" , "0"  , fromuser , " 加入了聊天室");
			serverCount++;
			printf("%d.    %s 加入了聊天室!\n" , serverCount , fromuser );
			
			for(i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
			{
				if (SocketArr[i] != -1)
				{
					send(SocketArr[i] , send_buff , strlen(send_buff)+1 , 0);
				}
			}
		}

		if(isRegistered == true)
		{
			//type ==11 为注册失败
			memset( send_buff , 0, sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s%s", "11" ,  "SERVER" , touser  , fromuser , "   已存在，请选用其它用户名！");
			send(client_socket , send_buff, strlen(send_buff)+1,0);
			serverCount++;
			printf("%d.    %s  用户名存在，拒绝它的注册并关闭连接！\n" , serverCount , fromuser);

			closesocket(*s);
			*s = -1;
		}
	}
	
	//广播
	 if (type == 1 )
	{
		for(i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
		{
			if (SocketArr[i] != -1)
			{
				memset( send_buff , 0, sizeof(send_buff) );
				sprintf(send_buff , "%s\n%s\n%s\n%s", "1" ,  fromuser , touser , data);
				send(SocketArr[i] , send_buff , strlen(receive_buff)+1 , 0);
			}
		}
		serverCount++;
		printf("%d.    %s 对所有人说 ： %s \n" , serverCount ,fromuser  , data);
	}

	//私聊，单播
	if (type == 2 )
	{
		//用户对自己发送私聊信息
		if (strcmp(fromuser , touser) == 0)
		{
			memset( send_buff , 0, sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s%s", "14" ,  "SERVER" , fromuser  ,"您给自己发送了一条私聊信息：",data);
			send(*s , send_buff , strlen(send_buff)+1, 0);
			serverCount++;
			printf("%d.    %s 对自己发送了一条私聊信息 ： %s \n" , serverCount , fromuser  , data );
			return Ret;
		}
		bool isExist = false;
		//判断私聊对象是否存在
		for(i = 0 ; i < MAX_CONNECTION_COUNT ; i++)
		{
			if (SocketArr[i] != -1 && strcmp(username[i] , touser) == 0)
			{
				//发送私聊信息给私聊对象
				memset( send_buff , 0, sizeof(send_buff) );
				sprintf(send_buff , "%s\n%s\n%s\n%s", "2" ,  fromuser , touser , data);
				send(SocketArr[i] , receive_buff , strlen(send_buff)+1 , 0);
				isExist = true;
				break;
			}
		}

		//私聊对象存在
		if (isExist)
		{
			//返回给发送者
			memset( send_buff , 0, sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s", "2" ,  fromuser , touser , data);
			send(*s , send_buff , strlen(send_buff)+1 , 0);
			serverCount++;
			printf("%d.    %s 对 %s 说 ： %s \n" , serverCount , fromuser , touser , data );
		}

		//私聊对象不存在
		else
		{
			//返回给发送者出错信息
			memset( send_buff , 0, sizeof(send_buff) );
			sprintf(send_buff , "%s\n%s\n%s\n%s%s", "14" ,  "SERVER" , fromuser  , touser , "不存在，请核对在线的用户列表，再发送私聊信息！");
			send(*s , send_buff , strlen(send_buff)+1 , 0);
			serverCount++;
			printf("%d.    %s 对不在线的 %s 发送一条内容为 的私聊信息 :  %s\n" , serverCount , fromuser , touser , data );
		}
	}
	
    return Ret;
}