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

void nf_scan(char * ip, int port_start, int port_end)
{
    // 创建套接字
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    // 向服务器（特定的IP和端口）发起请求
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  // 每个字节都用0填充
    serv_addr.sin_family = AF_INET;  // 使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  // 具体的IP地址

    // 开始扫描
    for(int i = port_start; i <= port_end; i++)
    {
        serv_addr.sin_port = htons(i);  //端口
        if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == 0)
        {  // 如果套接字创建成功，则端口打开
            cout << "IP: " << ip << "'s [" << i << "] port is *open*." << endl;
        }
        else
        {  // 否则为关闭状态
            cout << "IP: " << ip << "'s [" << i << "] port is *closed*." << endl;
        }
        close(sock);
    }

    return;
}