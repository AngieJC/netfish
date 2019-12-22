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

#include "getopt.h"
#include "global.h"
#include "listen.h"

using namespace std;

int main(int argc, char ** argv)
{
	char * ip = NULL, * file = NULL;
	int port_start = 0, port_end = 0, mod = 0;
	get_opt(argc, argv, &ip, &port_start, &port_end, &mod, &file);
	switch(mod)
	{
		case NOTHING:
			printf("Type \"nf -h\" for help\n");
			return 0;

		case LISTEN:
			listen(ip, port_start);
			break;
	}

	return 0;
}
