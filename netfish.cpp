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

#include "getopt.h"			// 负责获取参数
#include "global.h"			// 定义了全局变量
#include "listen.h"			// 监听模式需要用到的函数
#include "help.h"			// 打印帮助信息

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
	char * ip = NULL, * file_name = NULL;
	int port_start = 0, port_end = 0, mod = 0;
	bool flag[5] = {false};
	get_opt(argc, argv, &ip, &port_start, &port_end, flag, &file_name);
	if(flag[HELP])
	{
goto_help:
		help();
		return 0;
	}
	if(flag[LISTEN])
	{
		if(flag[PORT])
		{
			listen(ip, port_start);
		}
		else
		{
			goto goto_help;
		}
		return 0;
	}

	return 0;
}
