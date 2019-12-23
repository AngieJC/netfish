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
	if(*argv[1] == '-')
	{
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
				/*  由于-e参数不会出现在第一个，所以这里注释掉
				case 'e':
					flag[PROG] = true;
					break;
				*/
				case 'h':  // 如果有-h参数，则直接退出该函数，显示帮助信息
					flag[HELP] = true;
					return;
				case 'z':
					flag[ZERO] = true;
					break;
				default:  // 如果有非法参数，则直接退出该函数，显示帮助信息
goto_help:
					flag[HELP] = true;
					return;
			}
			argv[1]++;
		}  // 到此，第一个参数，即-xxx全部整理完毕
		
		// 如果有-z，则为扫描模式
		if(flag[ZERO])
		{
			if(argc == 4)
			{
				int invide = 0;
				*ip = argv[2];
				// 获取起止端口
				for(int i = 0; i < sizeof(argv[3]); i++)
				{
					if(argv[3][i] == '-')
					{
						invide = i;
						break;
					}
				}
				if(invide)
				{
					// 有分隔符，即端口为[起始-终止]形式
					char str_port_start[6] = {0};
					char str_port_end[6] = {0};
					memcpy(str_port_start, argv[3], invide);
					argv[3] += (invide + 1);
					strcpy(str_port_end, argv[3]);
					*port_start = atoi(str_port_start);
					*port_end = atoi(str_port_end);
				}
				else
				{
					// 没有分隔符，即只扫描一个端口
					*port_start = atoi(argv[3]);
					*port_end = atoi(argv[3]);
				}
			}  // if(argc == 4)  有4个参数
			else  // 否则肯定是参数有错误，显示帮助信息
			{
				goto goto_help;
			}
		}  // if(flag[ZERO])  扫描模式的参数到此结束

		else if(flag[LISTEN])  // 监听模式
		{
			if(flag[PORT])  // 定义了端口
			{
				*ip = (char *)"127.0.0.1";
				*port_start = atoi(argv[2]);
				*port_end = atoi(argv[2]);
				*file = NULL;
				cout << "Listen at: " << *ip << " on: " << *port_start << endl;
			}
			else
			{
				goto goto_help;
			}
		}
	}  // if(*argv[1] == '-')
	else
	{
		/*
		for(int i = 1; i < argc; i++)
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
		*/
		
		// 如果第一各参数不是-xxx形式，那么一定是nf [ip] [port] ... 形式
		*ip = argv[1];
		*port_start = atoi(argv[2]);
		*port_end = atoi(argv[2]);
		cout << "Connect to " << *ip << " on " << *port_start << endl;
	}
}

/*
nc -xxx 
*/