#include "epoll.h"

//set socket nonbloking mode
void SetNonBlock(int sock)
{
    int opts;
    if((opts = fcntl(sock, F_GETFL)) < 0){
	perror("fcntl error : F_GETFL\n");
	exit(-1);
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(sock, F_SETFL, opts) < 0){
	perror("fcntl error : F_SETFL\n");
	exit(-1);
    }
}

//new socket
int CreateSocket(int type)
{
    int sockfd = socket(AF_INET, type, 0);
    if(sockfd < 0){
	perror("create socket error\n");
	exit(1);
    }
    return sockfd;
}

//new epoll
int CreateEpoll(int count)
{
    int fd = epoll_create(count);
    if(fd < 0){
	perror("create epoll error\n");
	exit(1);
    }
    return fd;
}

//bind socket to ip & port
void BindSocketAddr(struct sockaddr_in *address, int sockfd)
{
    if(bind(sockfd, (struct sockaddr *)address, sizeof(struct sockaddr)) < 0)
	{
	    perror("bind error\n");
	    exit(1);
	}
}

//translate ip string into sockaddr_in format
int Ip2SocketAddr(struct sockaddr_in *address, const char *ip, int port){
    if(address == NULL){
	perror("address is NULL\n");
	exit(1);
	}
    bzero(address, sizeof(struct sockaddr));
    address->sin_family = AF_INET;
    address->sin_port = htons(port);
//    address->sin_addr.s_addr = inet_addr(ip);
    address->sin_addr.s_addr = htonl(INADDR_ANY);
    return inet_aton(ip, &(address->sin_addr));
}

//add a new epoll event to watch
int EpollAdd(int epfd, int sockfd, int events)
{
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = events;
    return epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);
}
//remove a epoll event form set
int EpollDel(int epfd, int sockfd)
{
    int ret = epoll_ctl(epfd, EPOLL_CTL_DEL, sockfd, NULL);
    close(sockfd);
    return ret;
}

//modify epoll event's type
int EpollMod(int epfd, int sockfd, int events)
{
    struct epoll_event ev;
    ev.data.fd = sockfd;
    ev.events = events;
    return epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
}

//wait for an epoll event
int EpollWait(int epfd, struct epoll_event *events, int maxEvents, int timeOut)
{
    return epoll_wait(epfd, events, maxEvents, timeOut);
}

//is fd readable
int IsRecvIn(struct epoll_event *event)
{
    if(event == NULL)
	return FALSE;
    else
	return event->events & EPOLLIN;
}

//is fd writable
int IsSendOut(struct epoll_event *event)
{
        if(event == NULL)
	return FALSE;
    else
	return event->events & EPOLLOUT;
}
