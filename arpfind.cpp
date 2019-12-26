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
    char ip_string[16] = {0};
    uint32_t localip, localipsegment;
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
    arparg send_arp_packet, recv_arp_packet;
    send_arp_packet.interface = interface;
    send_arp_packet.arppacket = &arpsend;
    send_arp_packet.localipsegment = localipsegment;


    pthread_create(&send_pid, NULL, sendarp, (void *)&send_arp_packet);

    pthread_join(send_pid, NULL);
    /*
    char error[PCAP_ERRBUF_SIZE + 1] = { 0 };
    pcap_t * pcap = pcap_open_live(interface, 65536, 1, 1, error);
    for(int i = 0; i < 256; i++)
    {
        arpsend.ah.dest_ip_addr = localipsegment + i * 256 * 256 * 256;
        if(arpsend.ah.dest_ip_addr == arpsend.ah.source_ip_addr)  // 跳过自己
        {
            continue;
        }
        pcap_sendpacket(pcap, (u_char *)&arpsend, sizeof(_ARP_PACKET));
        sleep(0.001);
    }
    pcap_close(pcap);
    */

    /*
    for_std local, remote;
    
    // 由于两个线程都对同一个套接字进行操作，因此local.clnt_sock_ptr与remote.clnt_sock_ptr一样
	local.clnt_sock_ptr = &clnt_sock;
	remote.clnt_sock_ptr = &clnt_sock;

    // 创建两个线程
	pthread_create(&local.pid, NULL, std_local, (void *)&local);
	pthread_create(&remote.pid, NULL, std_remote, (void *)&remote);

    // 告诉当前线程另一个线程的ID是多少
	remote.other_pid = local.pid;
	local.other_pid = remote.pid;

    // 等待线程退出
	pthread_join(local.pid,NULL);
	pthread_join(remote.pid,NULL);

    // 关闭套接字，释放系统资源
	close(serv_sock);
	close(clnt_sock);
    */
}