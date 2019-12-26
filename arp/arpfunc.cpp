/*
 filename:		arpfunc.cpp
 author:		AngieJC
 date:			2019/12/26
 description:	主机发现中需要用到的函数
 refer:         https://www.cnblogs.com/wd1001/p/4596945.html
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
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <net/if_arp.h>
#include <errno.h>
#include <linux/if.h>

#include "arpfunc.h"

using namespace std;


void arpinit()
{
    cout << "\033[31mThis func used to find hosts in LAN...\033[0m" << endl;
    cout << endl << endl << endl;
    cout << "\033[36m<:)))><" << "\t\t" << "<。)#)))≤" << endl;
    cout << "\t" << "<()>+++<" << "\t" << "<・ )))><<\033[0m" << endl;
    cout << endl << endl << endl;
}

int get_local_ip(const char *eth_inf, char *ip)
{
	int sd;
	struct sockaddr_in sin;
	struct ifreq ifr;
 
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	if (-1 == sd)
	{
		printf("socket error: %s\n", strerror(errno));
		return -1;
	}
 
	strncpy(ifr.ifr_name, eth_inf, IFNAMSIZ);
	ifr.ifr_name[IFNAMSIZ - 1] = 0;
 
	// if error: No such device  
	if (ioctl(sd, SIOCGIFADDR, &ifr) < 0)
	{
		printf("ioctl error: %s\n", strerror(errno));
		close(sd);
		return -1;
	}
 
	memcpy(&sin, &ifr.ifr_addr, sizeof(sin));
	snprintf(ip, 16, "%s", inet_ntoa(sin.sin_addr));
 
	close(sd);
	return 0;
}

int getmac(_ARP_PACKET * arp_packet)
{
    int sock_mac;
	struct ifreq ifr_mac;
	char mac_addr[30];
	sock_mac = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_mac == -1)
	{
		perror("create socket false...mac\n");
		return 0;
	}
 
	memset(&ifr_mac, 0, sizeof(ifr_mac));
	strncpy(ifr_mac.ifr_name, "ens33", sizeof(ifr_mac.ifr_name) - 1);
 
	if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
	{
		printf("mac ioctl error\n");
		close(sock_mac);
		return 0;
	}

    for(int i = 0; i < 6; i++)
    {
        arp_packet->eh.source_mac_addr[i] = (unsigned char)ifr_mac.ifr_hwaddr.sa_data[i];
    }
 
	close(sock_mac);
    return 1;
}

void * sendarp(void * arp_packet)
{
    arparg * args = (arparg *)arp_packet;
    for(int i = 0; i < 256; i++)
    {
        args->arppacket->ah.dest_ip_addr = args->localipsegment + i * 256 * 256 * 256;
        if(args->arppacket->ah.dest_ip_addr == args->arppacket->ah.source_ip_addr)  // 跳过自己
        {
            continue;
        }
        pcap_sendpacket(args->pcap, (u_char *)args->arppacket, sizeof(_ARP_PACKET));
        sleep(0.001);
    }

    return NULL;
}

void * recvarp(void * arp_packet)
{
    arparg * args = (arparg *)arp_packet;
    char packet_filter[]="ether proto \\arp";
    struct bpf_program fcode;
    struct pcap_pkthdr * header;
    const u_char * pkt_data;

    pcap_compile(args->pcap, &fcode, packet_filter, 1, args->mask);
    pcap_setfilter(args->pcap, &fcode);
    int result;
    struct in_addr ip_address;
    uint32_t liveip[256] = {0};
    while((result=pcap_next_ex(args->pcap,&header,&pkt_data))>=0)
    {
        if(result == 0)
        {
            continue;
        }

        _ARP_HEAD * arph = (_ARP_HEAD *)(pkt_data +14);
        if(!checkip(arph->source_ip_addr, liveip))
        {
            ip_address.s_addr = arph->source_ip_addr;
            cout << inet_ntoa(ip_address) << "\tis alive..." << endl;
        }
    }

    return NULL;
}

bool checkip(uint32_t source_ip_addr, uint32_t * liveip)
{
    while(*liveip)
    {
        if(*liveip == source_ip_addr)
        {
            return true;
        }
        liveip++;
    }
    *liveip = source_ip_addr;
    return false;
}