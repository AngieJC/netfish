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

int listen(char * ip, int port_start);
void * std_local(void * clnt_sock);
void * std_remote(void * clnt_sock);

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * clnt_sock_ptr;  // 套接字
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;