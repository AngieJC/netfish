/*
 filename:		ftp.cpp
 author:		AngieJC
 date:			2019/12/24
 description:	实现ftp客户端功能
*/

#include "ftp.h"
#include "ftp/getsock.h"
#include "global.h"
//include "listen.h"

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

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
    int * mutex;
	int * clnt_sock_ptr;  // 套接字
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

using namespace std;

void ftp(char * ip)
{
    //cout << "FTP功能暂未实现，敬请期待" << endl;
    int control = 0;  // 控连接的TCP连接号
    int data = 0;  // 数据连接的连接号
    control = getsock(ip, 21);
    int control_mutex = 1;


    for_std local, remote;

    // 由于两个线程都对同一个套接字进行操作，因此local.clnt_sock_ptr与remote.clnt_sock_ptr一样
	local.clnt_sock_ptr = &control;
    local.mutex = &control_mutex;
	remote.clnt_sock_ptr = &control;
    remote.mutex = &control_mutex;

    // 创建两个线程
	pthread_create(&local.pid, NULL, ftp_std_local, (void *)&local);
	pthread_create(&remote.pid, NULL, ftp_std_remote, (void *)&remote);

    // 告诉当前线程另一个线程的ID是多少
	remote.other_pid = local.pid;
	local.other_pid = remote.pid;

    // 等待线程退出
	pthread_join(local.pid,NULL);
	pthread_join(remote.pid,NULL);

    // 关闭套接字，释放系统资源
	close(control);
    return;
}