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

void find_host(char * interface)
{
    char error[PCAP_ERRBUF_SIZE + 1] = { 0 };

    arpinit();

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
    char ip_string[16] = {0};
    uint32_t localip, localipsegment, mask;
    pcap_lookupnet(interface, &localip, &mask, error);
    get_local_ip(interface, ip_string);
    localip = inet_addr(ip_string);
    localipsegment = localip % (256 * 256 * 256);
    arpsend.ah.source_ip_addr = localip;
    //arpsend.ah.source_ip_addr = inet_addr("192.168.139.18");
    memset(arpsend.ah.dest_mac_addr, 0, 6);
    //arpsend.ah.dest_ip_addr = inet_addr("192.168.18.1");



    // ARP数据填充
    memset(arpsend.padding, 0, 18);



    // 发送ARP数据报
    pthread_t send_pid, recv_pid;

    pcap_t * pcap = pcap_open_live(interface, 65536, 1, 1, error);
    if(!pcap)
    {
        cout << "open network card error, maybe use \"sudo\" can solve this problem\n";
        return;
    }

    arparg send_arp_packet, recv_arp_packet;
    send_arp_packet.interface = interface;
    send_arp_packet.arppacket = &arpsend;
    send_arp_packet.localipsegment = localipsegment;
    send_arp_packet.pcap = pcap;
    send_arp_packet.mask = mask;
    recv_arp_packet.interface = interface;
    recv_arp_packet.arppacket = &arprecv;
    recv_arp_packet.localipsegment = localipsegment;
    recv_arp_packet.pcap = pcap;
    recv_arp_packet.mask = mask;
    pthread_create(&send_pid, NULL, sendarp, (void *)&send_arp_packet);
    pthread_create(&recv_pid, NULL, recvarp, (void *)&recv_arp_packet);

    pthread_join(send_pid, NULL);
    sleep(1);
    pthread_cancel(recv_pid);
    //pthread_join(recv_pid, NULL);

    pcap_close(pcap);
}
