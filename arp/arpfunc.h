#include "../global.h"

// arp头
struct _ARP_HEAD
{
	uint16_t     hardware_type;          // 硬件类型  0x0001
	uint16_t     protocal_type;          // 协议类型  0x0800
	uint8_t      hardware_addr_len;      // 硬件地址长度  06
	uint8_t      protocal_addr_len;      // 协议地址长度  04
	uint16_t     operation_field;        // 操作字段 01 request ,  02 response
	uint8_t      source_mac_addr[6];     // 源mac地址 will be filled in runtime
	uint32_t      source_ip_addr;         // 源ip地址 localhost
	uint8_t      dest_mac_addr[6];       // 目的max地址 00:00:00:00:00:00
	uint32_t      dest_ip_addr;           // 目的ip地址 
};

// 以太头
struct _ETHER_HEAD
{
	uint8_t      dest_mac_addr[6];       //目的 mac 地址
	uint8_t      source_mac_addr[6];     //源 mac 地址
	uint16_t     type;                   //帧类型
};

// arp数据报
struct _ARP_PACKET
{
    _ETHER_HEAD     eh;                 // 以太头
    _ARP_HEAD       ah;                 // ARP头
    uint8_t          padding[18];        // 数据填充，保证数据长度大于60
};

void init();

// 获取mac地址
int getmac(_ARP_PACKET * arp_packet);

// 为ARP数据报(请求)填充数据
void make_request(_ARP_PACKET * arp_packet, unsigned char* source_mac_addr, unsigned long source_ip_addr, unsigned long dest_ip_addr);