/*
 filename:		ftp.cpp
 author:		AngieJC
 date:			2019/12/24
 description:	实现ftp客户端功能
 refer:         https://blog.csdn.net/sun_wangdong/article/details/45868615
*/

#include "ftp.h"
#include "ftp/getsock.h"
#include "ftp/ftpfunc.h"
#include "global.h"

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
    int this_port, serv_data_port;  // PASV模式下FTP服务器的数据TCP连接的端口
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

using namespace std;

void ftp(char * ip)
{
    //cout << "FTP功能暂未实现，敬请期待" << endl;
    int control = 0;  // 控连接的TCP连接号
    int data = 0;  // 数据连接的连接号
    int local_port = 0;
    control = getsock(ip, 21, &local_port);
    int control_mutex = 1;


    for_std control_local, control_remote;

    // 由于两个线程都对同一个套接字进行操作，因此local.clnt_sock_ptr与remote.clnt_sock_ptr一样
	control_local.clnt_sock_ptr = &control;
    control_local.mutex = &control_mutex;
	control_remote.clnt_sock_ptr = &control;
    control_remote.mutex = &control_mutex;

    // 创建两个线程
	pthread_create(&control_local.pid, NULL, ftp_control_std_local, (void *)&control_local);
	pthread_create(&control_remote.pid, NULL, ftp_control_std_remote, (void *)&control_remote);

    // 告诉当前线程另一个线程的ID是多少
	control_remote.other_pid = control_local.pid;
	control_local.other_pid = control_remote.pid;

    while(control_mutex)
    {
        sleep(0.1);
    }

    data = getsock(ip, control_remote.serv_data_port, local_port + 1);  // 最后一个参数没有用上，按照FTP协议，最后一个参数是数据连接的客户端的端口

    for_std data_local, data_remote;

    // 由于两个线程都对同一个套接字进行操作，因此local.clnt_sock_ptr与remote.clnt_sock_ptr一样
	data_local.clnt_sock_ptr = &control;
    data_local.mutex = &control_mutex;
	data_remote.clnt_sock_ptr = &control;
    data_remote.mutex = &control_mutex;

    // 创建两个线程
	pthread_create(&data_local.pid, NULL, ftp_data_std_local, (void *)&data_local);
	pthread_create(&data_remote.pid, NULL, ftp_data_std_remote, (void *)&data_remote);

    // 告诉当前线程另一个线程的ID是多少
	data_remote.other_pid = data_local.pid;
	data_local.other_pid = data_remote.pid;

    // 等待线程退出
	pthread_join(control_local.pid,NULL);
	pthread_join(control_remote.pid,NULL);
    pthread_join(data_local.pid,NULL);
    pthread_join(data_remote.pid,NULL);

    // 关闭套接字，释放系统资源
	close(control);
    return;
}