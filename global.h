/*
 filename:		global.h
 author:		AngieJC
 date:			不记得了
 description:	用于标记参数的全局变量
*/

// flag[0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
//		l, p, e, h, z,  , w, t, f, a
//
// l:	listen			listen mode, for inbound connects
// p:	port			local or remote port number
// e:	prog			inbound program to exec [dangerous!]
// h:	help			this cruft
// z:	zero-I/O		zero-I/O mode [used for scanning]
// w:   wait            break after waiting some time
// f:   ftp             connect a FTP Server
// a:   arp             find live hosts in LAN by ARP packet

#pragma pack(push) // 保持对齐方式
#pragma pack(1) // 设定1字节对齐

#define LISTEN		0
#define PORT		1
#define PROG		2
#define HELP		3
#define ZERO        4
#define CONNECT     5
#define TIME        6
#define TRACEROUTE  7
#define FTP         8
#define ARP         9