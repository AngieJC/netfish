/*
 filename:		netfish.cpp
 author:		AngieJC
 date:			不记得了
 description:	用于集成其他的所有模块，形成类似nc的小工具
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

#include "getopt.h"			// 负责获取参数
#include "global.h"			// 定义了全局变量
#include "listen.h"			// 监听模式需要用到的函数
#include "help.h"			// 打印帮助信息
#include "connect.h"		// 连接服务器
#include "scan.h"			// 端口扫描
#include "exec.h"			// 交互式运行程序
#include "getip.h"			// 将域名翻译为IP
#include "traceroute.h"		// 路由追踪
#include "ftp.h"			// FTP客户端
#include "arpfind.h"		// 主机发现

// flag[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
//		l, p, e, h, z,  , w, t, f, a
//
// l:	listen			listen mode, for inbound connects
// p:	port			local or remote port number
// e:	prog			inbound program to exec [dangerous!]
// h:	help			this cruft
// z:	zero-I/O		zero-I/O mode [used for scanning]
// w:   wait            break after waiting some time
// f:   ftp             connect a FTP Server
// a:   arp             find live hosts in LAN by ARP packet

using namespace std;

int main(int argc, char ** argv)
{
	char * ipOrHostname = NULL, * file_name = NULL, * interface = NULL;
	int port_start = 0, port_end = 0, mod = 0, time = 0;
	bool flag[10] = {false};
	get_opt(argc, argv, &ipOrHostname, &port_start, &port_end, flag, &file_name, &time, &interface);

	// 帮助模式
	if(flag[HELP])
	{
goto_help:
		help();
		return 0;
	}

	// 主机发现
	if(flag[ARP])
	{
		find_host(interface);
		return 0;
	}

	char ip[16] = {0};
	hostname_to_ip(ipOrHostname, ip);

	//路由追踪
	if(flag[TRACEROUTE])
	{
		traceroute(ip);
		return 0;
	}

	// ftp
	if(flag[FTP])
	{
		ftp(ip);
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
		if(flag[TIME])
		{
			nf_scan(ip, port_start, port_end, time);
		}
		else
		{
			nf_scan(ip, port_start, port_end);
		}
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