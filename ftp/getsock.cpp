/*
 filename:		getsock.cpp
 author:		AngieJC
 date:			2019/12/24
 description:	获取sock
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

#include "getsock.h"

using namespace std;

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * mutex;
	int * clnt_sock_ptr;  // 套接字
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

int getsock(char * ip, int port)
{
	// cout << "这个函数用来获取套接字链接\n";
	int control_sock;
    struct hostent *ht = NULL;
	struct sockaddr_in servaddr;
    control_sock = socket(AF_INET,SOCK_STREAM,0);
    if(control_sock < 0)
    {
       // printf("socket error\n");
       return -1;
    }
    ht = gethostbyname(ip);
    if(!ht)
    { 
        return -1;
    }
   
    memset(&servaddr,0,sizeof(struct sockaddr_in));
    memcpy(&servaddr.sin_addr.s_addr,ht->h_addr,ht->h_length);
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    
    if(connect(control_sock,(struct sockaddr*)&servaddr,sizeof(struct sockaddr)) == -1)
    {
        return -1;
    }
    return control_sock;
}