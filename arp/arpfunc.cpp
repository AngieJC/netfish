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

void make_request(_ARP_PACKET * arp_packet, unsigned char* source_mac_addr, unsigned long source_ip_addr, unsigned long dest_ip_addr)
{
    //
}