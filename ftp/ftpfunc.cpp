/*
 filename:		ftpfunc.cpp
 author:		AngieJC
 date:			2019/12/24
 description:	ftp通信过程中需要用到的函数
 */

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
#include "getsock.h"

using namespace std;

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * mutex;
	int * clnt_sock_ptr;  // 套接字
	int this_port, serv_data_port;  // PASV模式下FTP服务器的数据TCP连接的端口
	char * ip;
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

typedef struct getfile  // 用于上传下载文件
{
	int * clnt_sock_ptr;  // 套接字
	char * filename;  // 文件名
}getfile;

int send_pasv(int sock);

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
			sleep(0.1);
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
	}

login_success:
	//*sock->mutex = 0;  // 完成登录后释放互斥量


	// 用户输入命令
	int data_sock = 0;
	int addr[6] = {0};
	char cmd[1024] = {0};
	char filename[1024] = {0};
	char path[1024] = {0};
	while(1)
	{
		/*
		strcpy(buff, "pasv\xd\xa");
		write(*sock->clnt_sock_ptr, buff, strlen(buff));
		memset(buff, 0, 1024);
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		memset(addr, 0, 6);
		sscanf(buff,"%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
		sock->serv_data_port = addr[4] * 256 + addr[5];
		memset(buff, 0, 1024);
		*/

		memset(cmd, 0, 1024);
		sleep(0.2);
		cin >> cmd;
		if(strcmp(cmd, "ls") == 0 || strcmp(cmd, "l") == 0)
		{
			// 查看当前文件夹下所有文件
			//getchar();
			sock->serv_data_port = send_pasv(*sock->clnt_sock_ptr);  // 端口号可能为0
			data_sock = getsock(sock->ip, sock->serv_data_port);
			memset(buff, 0, 1024);
			sprintf(buff, "list\xd\xa");
			write(*sock->clnt_sock_ptr, buff, strlen(buff));
			memset(buff, 0, 1024);
			recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
			memset(buff, 0, 1024);
			pthread_t pid;
			pthread_create(&pid, NULL, ftp_data_std_local, (void *)&data_sock);
		}
		else if(strcmp(cmd, "download") == 0 || strcmp(cmd, "d") == 0)
		{
			// 下载文件
			cin >> filename;
			sock->serv_data_port = send_pasv(*sock->clnt_sock_ptr);  // 端口号可能为0
			data_sock = getsock(sock->ip, sock->serv_data_port);
			getfile clnt_sock;
			clnt_sock.clnt_sock_ptr = &data_sock;
			clnt_sock.filename = filename;
			memset(buff, 0, 1024);
			sprintf(buff, "retr %s\xd\xa", filename);
			write(*sock->clnt_sock_ptr, buff, strlen(buff));
			memset(buff, 0, 1024);
			recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
			memset(buff, 0, 1024);
			pthread_t pid;
			pthread_create(&pid, NULL, ftp_file_std_local, (void *)&clnt_sock);
		}
		else if(strcmp(cmd, "upload") == 0 || strcmp(cmd, "u") == 0)
		{
			// 上传文件
			cin >> filename;
			sock->serv_data_port = send_pasv(*sock->clnt_sock_ptr);  // 端口号可能为0
			data_sock = getsock(sock->ip, sock->serv_data_port);
			getfile clnt_sock;
			clnt_sock.clnt_sock_ptr = &data_sock;
			clnt_sock.filename = filename;
			memset(buff, 0, 1024);
			sprintf(buff, "stor %s\xd\xa", filename);
			write(*sock->clnt_sock_ptr, buff, strlen(buff));
			memset(buff, 0, 1024);
			recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
			memset(buff, 0, 1024);
			pthread_t pid;
			pthread_create(&pid, NULL, ftp_file_std_remote, (void *)&clnt_sock);
		}
		else if(strcmp(cmd, "cd") == 0)
		{
			// 改变工作目录
			cin >> path;
			sprintf(buff, "cwd %s\xd\xa", path);
			write(*sock->clnt_sock_ptr, buff, strlen(buff));
			memset(buff, 0, 1024);
			recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
			memset(buff, 0, 1024);
		}
		else if(strcmp(cmd, "rm") == 0)
		{
			// 删除文件
			cin >> filename;
			sprintf(buff, "dele %s\xd\xa", filename);
			write(*sock->clnt_sock_ptr, buff, strlen(buff));
			memset(buff, 0, 1024);
			recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
			memset(buff, 0, 1024);
		}
		else if(strcmp(cmd, "clear") == 0)
		{
			// 清屏
			system("clear");
		}
		else if(strcmp(cmd, "quit") == 0 || strcmp(cmd, "q") == 0)
		{
			// 退出
			getchar();
			//sock->serv_data_port = send_pasv(*sock->clnt_sock_ptr);
			//data_sock = getsock(sock->ip, sock->serv_data_port);
			sprintf(buff, "quit\xd\xa");
			write(*sock->clnt_sock_ptr, buff, strlen(buff));
			memset(buff, 0, 1024);
			recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
			memset(buff, 0, 1024);
			break;
		}
		else if(strcmp(cmd, "help") == 0 || strcmp(cmd, "h") == 0)
		{
print_help:
			// 显示帮助
			cout << "ls or l:\t\tlist files on FTP Server" << endl;
			cout << "download or d:\t\tdownload file from FTP Server" << endl;
			cout << "upload or u:\t\tupload file to FTP Server" << endl;
			cout << "cd:\t\t\tchange directory" << endl;
			cout << "rm:\t\t\tremove file" << endl;
			cout << "clear:\t\t\tclean the screen" << endl;
			cout << "quit or q:\t\tquit this program" << endl;
			cout << "help or h:\t\tthis cruft" << endl;
			continue;
		}
		else
		{
			// 显示帮助
			goto print_help;
		}
	}
	return NULL;
}

void * ftp_data_std_local(void * clnt_sock)  // 用于接收不需要保存的东西，比如list命令的返回结果
{
	char buff[1024];
	int recv_len = 0;
	int sock = *(int *)clnt_sock;
	memset(buff, 0, 1024);
	recv_len = read(sock, buff, sizeof(buff));
	close(sock);
	cout << buff << endl;
	memset(buff, 0, 1024);
	return NULL;
}

void * ftp_file_std_local(void * clnt_sock)  // 用于下载文件并保存
{
	getfile * sock = (getfile *)clnt_sock;
	char buff[1024];
	int recv_len = 0;
	memset(buff, 0, 1024);
	FILE * downloca_file_fb;
	downloca_file_fb = fopen(sock->filename, "wb");
	while(1)
	{
		recv_len = read(*sock->clnt_sock_ptr, buff, sizeof(buff));
		if(recv_len > 0)
		{
			fwrite(buff, recv_len, 1, downloca_file_fb);
			memset(buff, 0, 1024);
		}
		else
		{
			fclose(downloca_file_fb);
			break;
		}
	}
	close(*sock->clnt_sock_ptr);
	cout << "download complete!\n";
	return NULL;
}


void * ftp_file_std_remote(void * clnt_sock)  // 用于下载文件并保存
{
	getfile * sock = (getfile *)clnt_sock;
	char buff[1024];
	int read_len = 0;
	char * temp = buff;
	memset(buff, 0, 1024);
	FILE * upload_file_fb = fopen(sock->filename, "rb");
	if(upload_file_fb == NULL)
	{
		cout << "open file error, maybe it does not exist..." << endl;
		return NULL;
	}
	while(1)
	{
		read_len = fread(buff, 1, 1024, upload_file_fb);
		if(read_len)
		{
			write(*sock->clnt_sock_ptr, buff, read_len);
			memset(buff, 0, 1024);
		}
		else
		{
			fclose(upload_file_fb);
			break;
		}
	}
	close(*sock->clnt_sock_ptr);
	cout << "upload complete!\n";
	return NULL;
}

int send_pasv(int sock)
{
	int recv_len;
	int addr[6] = {0};
	char buff[1024] = {0};
	strcpy(buff, "pasv\xd\xa");
	write(sock, buff, strlen(buff));
	memset(buff, 0, 1024);
	sleep(1);
	recv_len = read(sock, buff, sizeof(buff));
	sscanf(buff,"%*[^(](%d,%d,%d,%d,%d,%d)",&addr[0],&addr[1],&addr[2],&addr[3],&addr[4],&addr[5]);
	return addr[4] * 256 + addr[5];
}