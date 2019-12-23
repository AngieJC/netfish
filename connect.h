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

#include "listen.h"
#include "global.h"

int nf_connect(char * ip, int port_start);
//void * std_local(void * clnt_sock);
//void * std_remote(void * clnt_sock);