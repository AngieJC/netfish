#include "../global.h"

typedef struct IP_LIST {
    struct in_addr value;
    struct IP_LIST *next;
} ip_list;

void icmpinit();

int Socket(int family, int type, int protocol);
void Bind(int fd, const struct sockaddr_in *sa, socklen_t salen);
ssize_t Recvfrom (int fd, void *ptr, size_t nbytes, int flags, struct sockaddr_in *sa, socklen_t *salenptr);
void Sendto(int fd, const void *ptr, int nbytes, int flags, const struct sockaddr_in *sa, socklen_t salen);
void Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);

int Inet_pton(int family, const char *src, void *dst);

ip_list *createIpList();
void destroyIpList(ip_list *node);
int insert(ip_list *root, struct in_addr x);
void printIpList(ip_list *root);
uint16_t in_cksum(uint16_t *addr, int len, int csum);

// 发送ping包
int ping(char * ip);