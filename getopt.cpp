#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "getopt.h"

// flag[0, 1, 2, 3, 4]
//		l, p, e, h, z
//
// l:	listen			listen mode, for inbound connects
// p:	port			local port number
// e:	prog			inbound program to exec [dangerous!]
// h:	help			this cruft
// z:	zero-I/O		zero-I/O mode [used for scanning]

using namespace std;

void get_opt(int argc, char ** argv, char ** ip, int * port_start, int * port_end, bool * flag, char ** file)
{
	if(argc == 1)
	{
		flag[HELP] = true;
		return;
	}
	//本文件用于获取用户输入的命令
	argv[1]++;  // 跳过'-'
	while(*argv[1] != 0)
	{
		switch(*argv[1])
		{
			case 'l':
				flag[LISTEN] = true;
				break;
			case 'p':
				flag[PORT] = true;
				break;
			case 'e':
				flag[PROG] = true;
				break;
			case 'h':  // 如果有-h参数，则直接退出该函数，显示帮助信息
				flag[HELP] = true;
				return;
			case 'z':
				flag[ZERO] = true;
				break;
			default:  // 如果有非法参数，则直接退出该函数，显示帮助信息
				flag[HELP] = true;
				return;
		}
		argv[1]++;
	}
	for(int i = 2; i < argc; i++)
	{
		// 监听模式， nf -lp port
		if(flag[LISTEN] && flag[PORT])
		{
			*ip = (char *)"127.0.0.1";
			*port_start = atoi(argv[i]);
			*port_end = atoi(argv[i]);
			*file = NULL;
		}
	}
}
