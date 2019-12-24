/*
 filename:		scan.cpp
 author:		AngieJC
 date:			2019/12/23
 description:	用于扫描端口
*/

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

#include "scan.h"

using namespace std;

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * clnt_sock_ptr;  // 套接字
    struct sockaddr_in * serv_addr;
    int size_serv_addr;
	pthread_t pid, other_pid;  // 两个线程ID
    char * ip;
    int port;
    int flag;
}for_std;

typedef struct two_thread  // 结构体，用于保存线程信息，方便通知线程退出
{
	pthread_t pid, other_pid;  // 两个线程ID
    int time;
}two_thread;

void * nf_scanner(void * scanner);
void * nf_timmer(void * timmer);

void nf_scan(char * ip, int port_start, int port_end)
{
    /*
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // 向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
    serv_addr.sin_family = AF_INET;  // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  // 具体的IP地址
    */

    // 开始扫描
    for(int i = port_start; i <= port_end; i++)
    {
        // 创建套接字
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        // 向服务器（特定的IP和端口）发起请求
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
        serv_addr.sin_family = AF_INET;  // 使用IPv4地址
        serv_addr.sin_addr.s_addr = inet_addr(ip);  // 具体的IP地址
        serv_addr.sin_port = htons(i);  //端口
        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
        {  // 如果套接字创建成功，则端口打开
            cout << "IP: " << ip << "'s [" << i << "] port is *open*." << endl;
        }
        else
        {  // 否则为关闭状态
            cout << "IP: " << ip << "'s [" << i << "] port is *closed*." << endl;
        }
    }

    return;
}

void nf_scan(char * ip, int port_start, int port_end, int time)
{
    /*
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // 向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
    serv_addr.sin_family = AF_INET;  // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  // 具体的IP地址
    */

    // 开始扫描
    for(int i = port_start; i <= port_end; i++)
    {
        // 创建套接字
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        // 向服务器（特定的IP和端口）发起请求
        struct sockaddr_in serv_addr;
        memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
        serv_addr.sin_family = AF_INET;  // 使用IPv4地址
        serv_addr.sin_addr.s_addr = inet_addr(ip);  // 具体的IP地址
        serv_addr.sin_port = htons(i);  //端口
        for_std scanner;
        two_thread timmer;
        scanner.clnt_sock_ptr = &sock;
        scanner.serv_addr = &serv_addr;
        scanner.size_serv_addr = sizeof(serv_addr);
        scanner.ip = ip;
        scanner.port = i;
        scanner.flag = 0;
        timmer.time = time;

        // 创建两个线程
        pthread_create(&scanner.pid, NULL, nf_scanner, (void *)&scanner);
        pthread_create(&timmer.pid, NULL, nf_timmer, (void *)&timmer);
        /*
        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
        {  // 如果套接字创建成功，则端口打开
            cout << "IP: " << ip << "'s [" << i << "] port is *open*." << endl;
        }
        else
        {  // 否则为关闭状态
            cout << "IP: " << ip << "'s [" << i << "] port is *closed*." << endl;
        }
        */

        // 告诉当前线程另一个线程的ID是多少
        scanner.other_pid = timmer.pid;
        timmer.other_pid = scanner.pid;

        // 等待线程退出
        pthread_join(scanner.pid,NULL);
        pthread_join(timmer.pid,NULL);

        if(scanner.flag)
        {
            cout << "IP: " << ip << "'s [" << i << "] port is *open*." << endl;
        }
        else
        {
            cout << "IP: " << ip << "'s [" << i << "] port is *closed*." << endl;
        }

        close(sock);
    }

    return;
    /*
    connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    for_std local, remote;

    // 由于两个线程都对同一个套接字进行操作，因此local.clnt_sock_ptr与remote.clnt_sock_ptr一样
	local.clnt_sock_ptr = &clnt_sock;
	remote.clnt_sock_ptr = &clnt_sock;

    // 创建两个线程
	pthread_create(&local.pid, NULL, std_local, (void *)&local);
	pthread_create(&remote.pid, NULL, std_remote, (void *)&remote);

    // 告诉当前线程另一个线程的ID是多少
	remote.other_pid = local.pid;
	local.other_pid = remote.pid;

    // 等待线程退出
	pthread_join(local.pid,NULL);
	pthread_join(remote.pid,NULL);

    // 关闭套接字，释放系统资源
	close(clnt_sock);
    */
}

void * nf_scanner(void * scanner)
{
    //scanner = (for_std *)scanner;
    for_std * this_scanner = (for_std *)scanner;;
    if(connect(*this_scanner->clnt_sock_ptr, (struct sockaddr*)this_scanner->serv_addr, this_scanner->size_serv_addr) == 0)
    {  // 如果套接字创建成功，则端口打开
        //cout << "IP: " << this_scanner->ip << "'s [" << this_scanner->port << "] port is *open*." << endl;
        this_scanner->flag = 1;
    }
    else
    {  // 否则为关闭状态
        //cout << "IP: " << this_scanner->ip << "'s [" << this_scanner->port << "] port is *closed*." << endl;
        this_scanner->flag = 0;
    }
    pthread_cancel(this_scanner->other_pid);
}

void * nf_timmer(void * timmer)
{
    //timmer = (two_thread *)scanner;
    two_thread * this_timmer = (two_thread *)timmer;
    sleep(this_timmer->time);
    pthread_cancel(this_timmer->other_pid);
}