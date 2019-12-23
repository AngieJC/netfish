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

#include "getopt.h"			// 负责获取参数
#include "global.h"			// 定义了全局变量
#include "listen.h"			// 监听模式需要用到的函数
#include "help.h"			// 打印帮助信息
#include "connect.h"		// 连接服务器
#include "scan.h"			// 端口扫描
#include "exec.h"			// 交互式运行程序
#include "getip.h"

// flag[0, 1, 2, 3, 4]
//		l, p, e, h, z
//
// l:	listen			listen mode, for inbound connects
// p:	port			local port number
// e:	prog			inbound program to exec [dangerous!]
// h:	help			this cruft
// z:	zero-I/O		zero-I/O mode [used for scanning]

using namespace std;

int main(int argc, char ** argv)
{
	char * ipOrHostname = NULL, * file_name = NULL;
	int port_start = 0, port_end = 0, mod = 0;
	bool flag[6] = {false};
	get_opt(argc, argv, &ipOrHostname, &port_start, &port_end, flag, &file_name);
	char ip[16] = {0};
	hostname_to_ip(ipOrHostname, ip);

	// 帮助模式
	if(flag[HELP])
	{
goto_help:
		help();
		return 0;
	}

	// 监听模式
	if(flag[LISTEN] && !flag[PROG])
	{
		if(flag[PORT])
		{
			nf_listen(ip, port_start);
		}
		else
		{
			goto goto_help;
		}
		return 0;
	}

	// 客户端模式
	if(flag[CONNECT] && !flag[PROG])
	{
		nf_connect(ip, port_start);
	}

	// 扫描
	if(flag[ZERO])
	{
		nf_scan(ip, port_start, port_end);
	}

	// 交互运行程序
	if(flag[PROG])
	{
		if(flag[LISTEN])
		{
			nf_exec_serv(ip, port_start, file_name);
		}
		else if(flag[CONNECT])
		{
			nf_exec_clen(ip, port_start, file_name);
		}
		else
		{
			goto goto_help;
		}
	}

	return 0;
}