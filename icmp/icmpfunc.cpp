/*
 filename:		icmp.cpp
 author:		AngieJC
 date:			2019/12/27
 description:	路由追踪中需要用到的函数
 refer:         https://github.com/w-ku/Traceroute
 */

#include <pcap.h>
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
#include <sys/ioctl.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip_icmp.h>
#include <net/if_arp.h>
#include <errno.h>
#include <linux/if.h>

#include "icmpfunc.h"

using namespace std;

#define TIMEOUT 1000  // 超时时间
#define TTL_LIMIT 30
#define REQUESTS_PER_TTL 3
#define BUFFER_SIZE 128
#define ICMP_HEADER_LEN 8
#define ERROR(str) { fprintf(stderr, "%s: %s\n", str, strerror(errno)); exit(1); }
#define CUSTOM_ERROR_MESSAGE(str) { fprintf(stderr, "%s\n", str); exit(1); }
#define IP_LIST_ERROR(str) { fprintf(stderr, "%s\n", str); exit(1); }

double timeDifference(struct timeval start, struct timeval end) {
    return (end.tv_sec - start.tv_sec)*1000.0 + (end.tv_usec - start.tv_usec)/1000.0;
}

void icmpinit()
{
    cout << "\033[31mThis func used to trace route to target...\033[0m" << endl;
    cout << endl << endl << endl;
    cout << "\033[36m<:)))><" << "\t\t" << "<。)#)))≤" << endl;
    cout << "\t" << "<()>+++<" << "\t" << "<・ )))><<\033[0m" << endl;
    cout << endl << endl << endl;
}

int ping(char * ip) {
    struct sockaddr_in remoteAddr;
    bzero(&remoteAddr, sizeof(remoteAddr));
    remoteAddr.sin_family = AF_INET;
    Inet_pton(AF_INET, ip, &remoteAddr.sin_addr);  // 如果IP地址非法将会输出提示信息

    int pid = getpid();

    int sockId = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    struct timeval begin, current;
    begin.tv_sec = 0;
    begin.tv_usec = 1000;  // (= 1 ms)
    Setsockopt(sockId, SOL_SOCKET, SO_RCVTIMEO, &begin, sizeof(begin));  // 设置超时时间


    char icmpRequestBuffer[BUFFER_SIZE], replyBuffer[BUFFER_SIZE];  // 为发包与收包申请空间

    struct icmp *icmpRequest = (struct icmp *) icmpRequestBuffer;
    icmpRequest->icmp_type = ICMP_ECHO;
    icmpRequest->icmp_code = htons(0);
    icmpRequest->icmp_id = htons(pid);

    int ttl, sequence = 0, repliedPacketsCnt, i;
    bool stop = 0;  // 停止发包标识
    double elapsedTime;  // 用于计算回包的平均时长
    struct timeval sendTime[REQUESTS_PER_TTL];  // send time of a specific packet
    ip_list *ipsThatReplied;  // 回包的ip列表

    for(ttl=1; ttl<=TTL_LIMIT; ttl++)
    {
		repliedPacketsCnt = 0;
		elapsedTime = 0.0;
		ipsThatReplied = createIpList();

		for(i=1; i<=REQUESTS_PER_TTL; i++)
        {
			Setsockopt(sockId, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl));  // 设置TTL参数
			icmpRequest->icmp_seq = htons(++sequence);  // 设置序列号
			
			icmpRequest->icmp_cksum = 0;
			icmpRequest->icmp_cksum = in_cksum((uint16_t*) icmpRequest, ICMP_HEADER_LEN, 0);  // 校验和
			
			gettimeofday(&sendTime[(sequence-1) % REQUESTS_PER_TTL], NULL);
			Sendto(sockId, icmpRequestBuffer, ICMP_HEADER_LEN, 0, &remoteAddr, sizeof(remoteAddr));
		}

		gettimeofday(&begin, NULL);  // 发包后获取当前时间

		while(repliedPacketsCnt < REQUESTS_PER_TTL)
        {
			int RecvRetVal = Recvfrom(sockId, replyBuffer, BUFFER_SIZE, 0, 0, 0);  // 收包
			gettimeofday(&current, NULL);

			if(RecvRetVal < 0)
            {
				if(timeDifference(begin, current) > TIMEOUT) break;
				continue;
			}

			struct ip *reply = (struct ip *) replyBuffer;

			if(reply->ip_p != IPPROTO_ICMP) continue;  // 检查收到的包是不是ICMP数据报

			struct icmp *icmpHeader = (struct icmp *) (replyBuffer + reply->ip_hl*4);  // 获取ICMP头信息

			if(icmpHeader->icmp_type != ICMP_ECHOREPLY && 
                !(icmpHeader->icmp_type == ICMP_TIME_EXCEEDED && icmpHeader->icmp_code == ICMP_EXC_TTL))
            {  // 如果不是响应包，也不是超时包
                continue;
            }

			if(icmpHeader->icmp_type == ICMP_TIME_EXCEEDED)
            {
                icmpHeader = (struct icmp *) (icmpHeader->icmp_data + ((struct ip *) (icmpHeader->icmp_data))->ip_hl*4);
            }

			if(ntohs(icmpHeader->icmp_id) != pid || sequence - ntohs(icmpHeader->icmp_seq) >= REQUESTS_PER_TTL)
            {
                continue;
            }

			elapsedTime += timeDifference(sendTime[(ntohs(icmpHeader->icmp_seq)-1) % REQUESTS_PER_TTL], current);
			insert(ipsThatReplied, reply->ip_src);
			repliedPacketsCnt++;

			if(icmpHeader->icmp_type == ICMP_ECHOREPLY)
            {
                stop = 1;
            }
		}

		// 输出
		printf("%2d. ", ttl);
		if(repliedPacketsCnt == 0) { printf("\033[33mNo reply\033[0m\n"); continue; }
		printIpList(ipsThatReplied);
		destroyIpList(ipsThatReplied);

		if(repliedPacketsCnt == REQUESTS_PER_TTL) printf("%.1f ms", elapsedTime / repliedPacketsCnt);
		else printf("\033[36m???\033[0m");

		if(stop == 1)
        {
            cout << "\t\033[31mTarget\033[0m" << endl;
            break;
        }
        else
        {
            cout << endl;
        }
    }
    return 0;
}

int Socket(int family, int type, int protocol)
{
	int	n;
	if ( (n = socket(family, type, protocol)) < 0)
    {
		ERROR ("socket error");
    }
	return n;
}

void Bind(int fd, const struct sockaddr_in *sa, socklen_t salen)
{
	if (bind(fd, (struct sockaddr*)sa, salen) < 0)
    {
        ERROR ("bind error");
    }
}

ssize_t Recvfrom(int fd, void *ptr, size_t nbytes, int flags, struct sockaddr_in *sa, socklen_t *salenptr)
{
	ssize_t	n = recvfrom(fd, ptr, nbytes, flags, (struct sockaddr*)sa, salenptr);
	if(n < 0 && errno != EAGAIN)
    {
        ERROR ("recvfrom error");
    }
	return n;
}

void Sendto(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr_in *sa, socklen_t salen)
{
	if (sendto(fd, ptr, nbytes, flags, (struct sockaddr*)sa, salen) != nbytes)
    {
        ERROR ("sendto error");
    }
}

void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
{
	if (setsockopt(sockfd,level,optname,optval,optlen) < 0)
    {
        ERROR ("setsockopt error");
    }
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout)
{
	int n;
	if ((n = select(nfds,readfds,writefds,exceptfds,timeout)) < 0)
    {
            ERROR ("select error");
    }
	return n;	
}


int Inet_pton(int family, const char *src, void *dst)
{
    int n = inet_pton(family, src, dst);
    if(n == 0)
    {
        CUSTOM_ERROR_MESSAGE("inet_pton error (invalid IP address)");
    }
    if(n == -1)
    {
        CUSTOM_ERROR_MESSAGE("inet_pton error (invalid address family)");
    }
    return n;
}

ip_list *createIpList()
{
    ip_list *root = (ip_list *) malloc(sizeof(ip_list));
    if(root == NULL)
    {
        IP_LIST_ERROR("Malloc error: Could not create an IP address list.");
    }
    root->next = NULL;
    return root;
}


void destroyIpList(ip_list *node)
{
    if(node == NULL)
    {
        return;
    }
    destroyIpList(node->next);
    free(node);
}


void addAfterTheNode(ip_list *preceding, struct in_addr x)
{
    ip_list *newNode = (ip_list *) malloc(sizeof(ip_list));
    if(newNode == NULL)
    {
        IP_LIST_ERROR("Malloc error: Could not add an IP address to the list.");
    }
    newNode->value = x;
    newNode->next = preceding->next;
    preceding->next = newNode;
}


int insert(ip_list *root, struct in_addr x)
{
    ip_list *cur = root, *tmp;
    
    while(cur->next != NULL)
    {
		tmp = cur->next;
		if(x.s_addr == tmp->value.s_addr) return 0;
		if(x.s_addr < tmp->value.s_addr) break;
		cur = tmp;
    }
    addAfterTheNode(cur, x);
    return 1;
}


void printIpList(ip_list *root)
{
    ip_list *cur = root->next;
    
    while(cur != NULL)
    {
		printf("%-16s", inet_ntoa(cur->value));
		cur = cur->next;
    }
}

uint16_t in_cksum(uint16_t *addr, int len, int csum)
{
    int sum = csum;

    while(len > 1)
    {
        sum += *addr++;
        len -= 2;
    }

    if(len == 1)
    {
        sum += htons(*(uint8_t *)addr << 8);
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return ~sum;
}