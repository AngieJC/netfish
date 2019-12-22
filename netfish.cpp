#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "getopt.h"
#include "global.h"

using namespace std;

int listen(char * ip, int port_start);
void * std_local(void * clnt_sock);
void * std_remote(void * clnt_sock);

int stop = 0;

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
	int * clnt_sock_ptr = &clnt_sock;
	pthread_t local_pid, remote_pid;  // 定义两个线程的pid
	pthread_create(&local_pid, NULL, std_local, (void *)clnt_sock_ptr);
	pthread_create(&remote_pid, NULL, std_remote, (void *)clnt_sock_ptr);
	pthread_join(local_pid,NULL);
	pthread_join(remote_pid,NULL);
	close(serv_sock);
	return 0;

	/*

    //接收客户端请求
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

	//向客户端发送数据
    char str[1024] = {0};
	while(1)
	{
		read(clnt_sock, str, sizeof(str));
		cout << str;
		memset(str, 0, 1024);
	}
   
    //关闭套接字
    close(clnt_sock);
    close(serv_sock);
	*/
}


void * std_local(void * clnt_sock)
{
	char buff[1024];
	memset(buff, 0, 1024);
	int * sock = (int *)clnt_sock;
	while(1)
	{
		read(*sock, buff, sizeof(buff));
		cout << buff;
		memset(buff, 0, 1024);
	}
	return NULL;
}


void * std_remote(void * clnt_sock)
{
	char buff[1024];
	memset(buff, 0, 1024);
	int * sock = (int *)clnt_sock;
	while(1)
	{
		cin >> buff;
		sprintf(buff, "%s\r\n", buff);
		write(*sock, buff, sizeof(buff));
		memset(buff, 0, 1024);
	}
	return NULL;
}