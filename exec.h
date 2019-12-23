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

void * std_local_exec(void * clnt_sock);
void * std_remote_exec(void * clnt_sock);

void nf_exec_serv(char * ip, int port_start, char * filename);
void nf_exec_clen(char * ip, int port_start, char * filename);