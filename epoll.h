#ifndef EPOLL_H
#define EPOLL_H
#include "utility.h"
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#define UDP SOCK_DGRAM
#define TCP SOCK_STREAM

//set socket nonbloking mode
void SetNonBlock(int sock);

//new socket
int CreateSocket(int type);

//new epoll
int CreateEpoll(int count);

//bind socket to ip & port
void BindSocketAddr(struct sockaddr_in *address, int sockfd);

//translate ip string into sockaddr_in format
int Ip2SocketAddr(struct sockaddr_in *address, const char *ip, int port);


//add a new epoll event to watch
int EpollAdd(int epfd, int sockfd, int events);

//remove a epoll event form set
int EpollDel(int epfd, int sockfd);

//modify epoll event's type
int EpollMod(int epfd, int sockfd, int events);

//wait for an epoll event

int EpollWait(int epfd, struct epoll_event *events, int maxEvents, int timeOut);

//is fd readable
int IsRecvIn(struct epoll_event *event);

//is fd writable
int IsSendOut(struct epoll_event *event);


#endif //EPOLL_H
