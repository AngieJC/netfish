#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "getopt.h"
#include "global.h"

using namespace std;

typedef struct for_std
{
	int * clnt_sock_ptr;
	pthread_t pid, other_pid;
}for_std;

int listen(char * ip, int port_start);
void * std_local(void * clnt_sock);
void * std_remote(void * clnt_sock);

int main(int argc, char ** argv)
{
	char * ip = NULL, * file = NULL;
	int port_start = 0, port_end = 0, mod = 0;
	get_opt(argc, argv, &ip, &port_start, &port_end, &mod, &file);
	switch(mod)
	{
		case NOTHING:
			printf("Type \"nf -h\" for help\n");
			return 0;

		case LISTEN:
			listen(ip, port_start);
			break;
	}

	return 0;
}

int listen(char * ip, int port_start)
{
	int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
	serv_addr.sin_port = htons(port_start);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//进入监听状态，等待用户发起请求
    listen(serv_sock, 20);
	struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	//int * clnt_sock_ptr = &clnt_sock;
	//pthread_t local_pid, remote_pid;  // 定义两个线程的pid
	for_std local, remote;
	local.clnt_sock_ptr = &clnt_sock;
	remote.clnt_sock_ptr = &clnt_sock;
	pthread_create(&local.pid, NULL, std_local, (void *)&local);
	pthread_create(&remote.pid, NULL, std_remote, (void *)&remote);
	remote.other_pid = local.pid;
	local.other_pid = remote.pid;
	pthread_join(local.pid,NULL);
	pthread_join(remote.pid,NULL);
	close(serv_sock);
	close(clnt_sock);
	return 0;
}


void * std_local(void * clnt_sock)
{
	for_std * sock = (for_std *)clnt_sock;
	struct tcp_info info;
	int len = sizeof(info);
	char buff[1024];
	memset(buff, 0, 1024);
	while(1)
	{
		getsockopt(*sock->clnt_sock_ptr, IPPROTO_TCP, TCP_INFO, (void *)&info, (socklen_t *)&len);
		if(!(info.tcpi_state==TCP_ESTABLISHED))  // 连接已经断开
		{
			pthread_cancel(sock->other_pid);
			break;
		}
		read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		cout << buff;
		memset(buff, 0, 1024);
	}
	return NULL;
}


void * std_remote(void * clnt_sock)
{
	char buff[1024];
	memset(buff, 0, 1024);
	for_std * sock = (for_std *)clnt_sock;
	while(1)
	{
		cin >> buff;
		sprintf(buff, "%s\r\n", buff);
		write(*sock->clnt_sock_ptr, buff, sizeof(buff));
		memset(buff, 0, 1024);
	}
	return NULL;
}