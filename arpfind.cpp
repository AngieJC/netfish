/*
 filename:		arpfind.cpp
 author:		AngieJC
 date:			2019/12/26
 description:	发现局域网内存活主机
 */

#pragma pack(push) // 保持对齐方式
#pragma pack(1) // 设定1字节对齐

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

#include "arp/arpfunc.h"

using namespace std;

/*
// arp头
struct _ARP_HEAD
{
	u_short     hardware_type;          // 硬件类型  0x0001
	u_short     protocal_type;          // 协议类型  0x0800
	u_char      hardware_addr_len;      // 硬件地址长度  06
	u_char      protocal_addr_len;      // 协议地址长度  04
	u_short     operation_field;        // 操作字段 01 request ,  02 response
	u_char      source_mac_addr[6];     // 源mac地址 will be filled in runtime
	u_long      source_ip_addr;         // 源ip地址 localhost
	u_char      dest_mac_addr[6];       // 目的max地址 00:00:00:00:00:00
	u_long      dest_ip_addr;           // 目的ip地址 
};

// 以太头
struct _ETHER_HEAD
{
	u_char      dest_mac_addr[6];       //目的 mac 地址
	u_char      source_mac_addr[6];     //源 mac 地址
	u_short     type;                   //帧类型
};

// arp数据报
struct _ARP_PACKET
{
    _ETHER_HEAD     eh;                 // 以太头
    _ARP_HEAD       ah;                 // ARP头
    u_char          padding[18];        // 数据填充，保证数据长度大于60
};
*/

void find_host(char * interface)
{
    init();

    // 申请ARP数据报
    _ARP_PACKET arpsend, arprecv;

    // 以太头填充
    // 类型
    arpsend.eh.type = htons(0x0806);  // ARP代号

    // 目的地址，广播，全f
    for(int i = 0; i < 6; i++)
    {
        arpsend.eh.dest_mac_addr[i] = 0xff;
    }

    // 源地址
    if(!getmac(&arpsend))
    {
        cout << "Can't get MAC" << endl;
        return;
    }



    // ARP头填充
    arpsend.ah.hardware_type = htons(0x0001);
    arpsend.ah.protocal_type = htons(0x0800);
    arpsend.ah.hardware_addr_len = 0x0006;
    arpsend.ah.protocal_addr_len = 0x0004;
    arpsend.ah.operation_field = htons(0x00001);
    memcpy(arpsend.ah.source_mac_addr, arpsend.eh.source_mac_addr, 6);
    //memset((char*)&arpsend.ah.source_ip_addr, 0, 4);  // 源ip
    arpsend.ah.source_ip_addr = inet_addr("192.168.139.18");
    memset(arpsend.ah.dest_mac_addr, 0, 6);
    //memset((char*)&arpsend.ah.dest_ip_addr, 0, 4);  // 目的ip
    arpsend.ah.dest_ip_addr = inet_addr("192.168.18.2");



    // ARP数据填充
    memset(arpsend.padding, 0, 18);



    // 佛送ARP数据报
    char error[PCAP_ERRBUF_SIZE + 1] = { 0 };
    pcap_t * pcap = pcap_open_live(interface, 65536, 1, 0, error);
    for(int i = 0; i < 5; i++)
    {
        sleep(1);
        pcap_sendpacket(pcap, (u_char *)&arpsend, sizeof(_ARP_PACKET));
    }

}