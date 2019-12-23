//#include "listen.h"
#include "connect.h"
#include "global.h"

using namespace std;

typedef struct for_std  // 结构体，用于保存线程信息，方便通知线程退出
{
	int * clnt_sock_ptr;  // 套接字
	pthread_t pid, other_pid;  // 两个线程ID
}for_std;

int nf_connect(char * ip, int port_start)
{
    /*
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr(ip);  //具体的IP地址
	serv_addr.sin_port = htons(port_start);  //端口
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

	//进入监听状态，等待用户发起请求
    listen(serv_sock, 20);
	struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
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
	return 0;
    */
    int clnt_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port_start);
    connect(clnt_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    /*
    // 连接后创建两个线程
    struct sockaddr_in serv_addr;
    socklen_t serv_addr_size = sizeof(serv_addr);
    */
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
	close(clnt_sock);
	return 0;
}