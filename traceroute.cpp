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

#include "icmp/icmpfunc.h"

using namespace std;

void traceroute(char * ip)
{
    icmpinit();
    
    ping(ip);

    return;
}