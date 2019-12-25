#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <pthread.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include "ftpfunc.h"

using namespace std;

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * mutex;
	int * clnt_sock_ptr;  // 套接字
	int this_port, serv_data_port;  // PASV模式下FTP服务器的数据TCP连接的端口
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

void * ftp_control_std_local(void * clnt_sock)
{
	for_std * sock = (for_std *)clnt_sock;
	struct tcp_info info;
	int len = sizeof(info);
	char buff[1024];
	int recv_len = 0;
	memset(buff, 0, 1024);
	while(1)
	{
		if(*sock->mutex)
		{
			continue;
		}
		getsockopt(*sock->clnt_sock_ptr, IPPROTO_TCP, TCP_INFO, (void *)&info, (socklen_t *)&len);
		if(!(info.tcpi_state==TCP_ESTABLISHED))  // 连接已经断开
		{
			pthread_cancel(sock->other_pid);
			break;
		}
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		//fputs(stdout, 256 - 1, buff);
		//fputs(buff, stdout);
		if(recv_len > 0)
		{
			fwrite(buff, recv_len, 1, stdout);
			memset(buff, 0, 1024);
		}
		//cout << buff;
	}
	return NULL;
}


void * ftp_control_std_remote(void * clnt_sock)
{
	char buff[1024] = {0};
	char * temp = buff;
	for_std * sock = (for_std *)clnt_sock;
	
	/*
	首先使用匿名用户测试能不能登录，如果不能登录等待用户输入
	*/
	int recv_len = 0;

	// 获取服务器Banner
	recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
	fwrite(buff, recv_len, 1, stdout);
	memset(buff, 0, 1024);

	// 默认用户名
	//buff = "user angie\xd\xa";
	strcpy(buff, "user anonymous\xd\xa");
	write(*sock->clnt_sock_ptr, buff, strlen(buff));
	recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
	memset(buff, 0, 1024);

	// 默认密码
	strcpy(buff, "pass \xd\xa");
	write(*sock->clnt_sock_ptr, buff, strlen(buff));
	memset(buff, 0, 1024);
	recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
	//fwrite(buff_in, recv_len, 1, stdout);

	// 判断默认用户名密码是否登录成功
	char code[4] = {0};
	for(int i = 0; i < 3; i++)
	{
		code[i] = buff[i];
	}
	memset(buff, 0, 1024);
	if(strcmp("230", code))
	{
		strcpy(buff, "login with default user and pass failed...\n");
		fwrite(buff, strlen(buff), 1, stdout);
		memset(buff, 0, 1024);
	}
	else
	{
		cout << "login success\n";
		memset(buff, 0, 1024);
		goto login_success;
	}

	// 判断是否登录成功
	while(1)
	{
		char user[1024] = {0}, pass[1024] = {0};
		cout << "user: ";
		cin >> user;
		cout << "pass: ";
		cin >> pass;

		sprintf(buff, "user %s\xd\xa", user);
		write(*sock->clnt_sock_ptr, buff, strlen(buff));
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		memset(buff, 0, 1024);
		
		sprintf(buff, "pass %s\xd\xa", pass);
		write(*sock->clnt_sock_ptr, buff, strlen(buff));
		memset(buff, 0, 1024);
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		for(int i = 0; i < 3; i++)
		{
			code[i] = buff[i];
		}
		if(strcmp("230", code))  // 登录失败
		{
			//strcpy(buff, "login with default user and pass failed...\nuser: ");
			strcpy(buff, "login failed...\nuser: ");
			fwrite(buff, strlen(buff), 1, stdout);
			memset(buff, 0, 1024);
		}
		else  // 登录成功
		{
			getchar();  // 获取cin后的回车，防止讲回车发送给服务器
			cout << "login success\n";
			memset(buff, 0, 1024);
			break;
		}
		/*
		memset(buff, 0, 1024);
		char user[1024] = {0}, pass[1024] = {0};
		cin >> user;
		//fread(user, 1024, 1, stdin);
		cout << "pass: ";
		cin >> pass;
		//fread(pass, 1024, 1, stdin);
		sprintf(buff, "user %s\xd\xa", user);
		write(*sock->clnt_sock_ptr, buff, strlen(buff));
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		memset(buff, 0, 1024);
		
		sprintf(buff, "pass %s\xd\xa", pass);
		write(*sock->clnt_sock_ptr, buff, strlen(buff));
		memset(buff, 0, 1024);
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		for(int i = 0; i < 3; i++)
		{
			code[i] = buff[i];
		}
		if(strcmp("230", code))  // 登录失败
		{
			//strcpy(buff, "login with default user and pass failed...\nuser: ");
			strcpy(buff, "login failed...\nuser: ");
			fwrite(buff, strlen(buff), 1, stdout);
			memset(buff, 0, 1024);
		}
		else  // 登录成功
		{
			getchar();  // 获取cin后的回车，防止讲回车发送给服务器
			cout << "login success\n";
			memset(buff, 0, 1024);
			break;
		}
		*/
	}

login_success:
    strcpy(buff, "pasv\xd\xa");
    write(*sock->clnt_sock_ptr, buff, strlen(buff));
    memset(buff, 0, 1024);
	recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
    int addr[6] = {0};
    sscanf(buff,"%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
    sock->serv_data_port = addr[4] * 256 + addr[5];
	memset(buff, 0, 1024);
	*sock->mutex = 0;  // 完成登录后释放互斥量


	// 用户输入命令
	while(1)
	{
		//cin >> buff;
		//gets(buff);
		//fgets(buff, 256 - 1, stdin);
		if(fread(temp, 1, 1, stdin) == 0)
		{
			if(temp == buff)
			{
				// 没有东西
				continue;
			}
			else
			{
				// 这种情况为读到文件尾了
				write(*sock->clnt_sock_ptr, buff, temp - buff);
				temp = buff;
				memset(buff, 0, 1024);
				continue;
			}
		}
		/*
		else if(fread(temp, 1, 1, stdin) == 0 && temp != buff)
		{
			// 这种情况为读到文件尾了
			write(*sock->clnt_sock_ptr, buff, temp - buff + 1);
			temp = buff;
			memset(buff, 0, 1024);
		}
		*/
		if(*temp == 0x0a)
		{
			// 读到回车
			write(*sock->clnt_sock_ptr, buff, temp - buff + 1);
			temp = buff;
			memset(buff, 0, 1024);
		}
		else if(temp - buff == 1022)
		{
			// 缓冲区满了
			temp = buff;
			write(*sock->clnt_sock_ptr, buff, 1023);
			memset(buff, 0, 1024);
		}
		else
		{
			temp++;
		}
		
		/*
		if(buff[0] == 0x0a)
		{
			continue;
		}
		*/

		/*
		if(strlen(buff) == 0)
		{
			continue;
		}
		*/
		//sprintf(buff, "%s", buff);
		
	}
	return NULL;
}

void * ftp_data_std_local(void * clnt_sock)
{
	for_std * sock = (for_std *)clnt_sock;
	struct tcp_info info;
	int len = sizeof(info);
	char buff[1024];
	int recv_len = 0;
	memset(buff, 0, 1024);
	/*
	while(1)
	{
		getsockopt(*sock->clnt_sock_ptr, IPPROTO_TCP, TCP_INFO, (void *)&info, (socklen_t *)&len);
		if(!(info.tcpi_state==TCP_ESTABLISHED))  // 连接已经断开
		{
			pthread_cancel(sock->other_pid);
			break;
		}
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		//fputs(stdout, 256 - 1, buff);
		//fputs(buff, stdout);
		if(recv_len > 0)
		{
			fwrite(buff, recv_len, 1, stdout);
			memset(buff, 0, 1024);
		}
		//cout << buff;
	}
	*/
	recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
	//close(*sock->clnt_sock_ptr);
	//fputs(stdout, 256 - 1, buff);
		//fputs(buff, stdout);
	/*
	if(recv_len > 0)
	{
		//fwrite(buff, recv_len, 1, stdout);
		cout << buff;
		memset(buff, 0, 1024);
	}
	*/
	cout << buff;
	memset(buff, 0, 1024);
	return NULL;
}


void * ftp_data_std_remote(void * clnt_sock)
{
	char buff[1024];
	char * temp = buff;
	memset(buff, 0, 1024);
	for_std * sock = (for_std *)clnt_sock;
	while(1)
	{
		//cin >> buff;
		//gets(buff);
		//fgets(buff, 256 - 1, stdin);
		if(fread(temp, 1, 1, stdin) == 0)
		{
			if(temp == buff)
			{
				// 没有东西
				continue;
			}
			else
			{
				// 这种情况为读到文件尾了
				write(*sock->clnt_sock_ptr, buff, temp - buff);
				temp = buff;
				memset(buff, 0, 1024);
				continue;
			}
		}
		/*
		else if(fread(temp, 1, 1, stdin) == 0 && temp != buff)
		{
			// 这种情况为读到文件尾了
			write(*sock->clnt_sock_ptr, buff, temp - buff + 1);
			temp = buff;
			memset(buff, 0, 1024);
		}
		*/
		if(*temp == 0x0a)
		{
			// 读到回车
			write(*sock->clnt_sock_ptr, buff, temp - buff + 1);
			temp = buff;
			memset(buff, 0, 1024);
		}
		else if(temp - buff == 1022)
		{
			// 缓冲区满了
			temp = buff;
			write(*sock->clnt_sock_ptr, buff, 1023);
			memset(buff, 0, 1024);
		}
		else
		{
			temp++;
		}
		
		/*
		if(buff[0] == 0x0a)
		{
			continue;
		}
		*/

		/*
		if(strlen(buff) == 0)
		{
			continue;
		}
		*/
		//sprintf(buff, "%s", buff);
		
	}
	return NULL;
}