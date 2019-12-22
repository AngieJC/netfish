#include "getopt.h"

void get_opt(int argc, char ** argv, char ** ip, int * port_start, int * port_end, int * mod, char ** file)
{
	if(argc == 1)
	{
		*mod = NOTHING;
		return;
	}
	//本文件用于获取用户输入的命令
	//如果用户输入的是nf -lp [port]
	if(!strcmp(argv[1], "-lp"))
	{
		*ip = (char *)"127.0.0.1";
		*port_start = atoi(argv[2]);
		*port_end = atoi(argv[2]);
		*mod = LISTEN;
		*file = NULL;
	}
}
