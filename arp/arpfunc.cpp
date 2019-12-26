/*
 filename:		arpfunc.cpp
 author:		AngieJC
 date:			2019/12/26
 description:	主机发现中需要用到的函数
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

/*
typedef u_int32_t in_addr_t;
struct in_addr
{
    in_addr_t s_addr;
};
*/

void init()
{
    cout << "This func used to find live hosts in LAN" << endl;
    /*
    int sock_mac;
	struct ifreq ifr_mac;
	char mac_addr[30];
	sock_mac = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_mac == -1)
	{
		perror("create socket falise...mac\n");
		return;
	}
 
	memset(&ifr_mac, 0, sizeof(ifr_mac));
	strncpy(ifr_mac.ifr_name, "ens33", sizeof(ifr_mac.ifr_name) - 1);
 
	if ((ioctl(sock_mac, SIOCGIFHWADDR, &ifr_mac)) < 0)
	{
		printf("mac ioctl error\n");
		close(sock_mac);
		return;
	}
 
	sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]
	);
 
	close(sock_mac);
    //cout << mac_addr << endl;
    */
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
		perror("create socket falise...mac\n");
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
 
    /*
	sprintf(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[0],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[1],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[2],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[3],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[4],
		(unsigned char)ifr_mac.ifr_hwaddr.sa_data[5]
	);
    cout << mac_addr << endl;
    */

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
    char error[PCAP_ERRBUF_SIZE + 1] = { 0 };
    pcap_t * pcap = pcap_open_live(args->interface, 65536, 1, 1, error);
    for(int i = 0; i < 256; i++)
    {
        args->arppacket->ah.dest_ip_addr = args->localipsegment + i * 256 * 256 * 256;
        if(args->arppacket->ah.dest_ip_addr == args->arppacket->ah.source_ip_addr)  // 跳过自己
        {
            continue;
        }
        pcap_sendpacket(pcap, (u_char *)args->arppacket, sizeof(_ARP_PACKET));
        sleep(0.001);
    }
    pcap_close(pcap);
}