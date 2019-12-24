/*
 filename:		traceroute.cpp
 author:		AngieJC
 date:			2019/12/24
 description:	用于路由追踪
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

using namespace std;

#define BUF_SIZE 64

void traceroute(char * ip)
{
    /*
    //创建套接字
    //SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
    int sock = socket(AF_INET, SOCK_DGRAM,0);
    //服务器地址信息
    sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));  //每个字节都用0填充
    servAddr.sin_family = PF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip);
    servAddr.sin_port = htons(15865);
    //不断获取用户输入并发送给服务器，然后接受服务器数据
    sockaddr fromAddr;
    int addrLen = sizeof(fromAddr);
    for(int i = 1; i <= 128; i++)
    {
        char buffer[BUF_SIZE] = {0};
        memset(buffer, 'a', BUF_SIZE - 1);
        setsockopt(sock, IPPROTO_IP, IP_TTL, (char *)&i, sizeof(i));
        sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr*)&servAddr, sizeof(servAddr));
        cout << "send\n";
        sleep(1);
    }
    close(sock);
    */
    cout << "路由追踪功能已经不太好使，没有实现" << endl;

    return;
}