#include "sql.h"
#include <stdio.h> 
#include <errno.h> 
#include <assert.h>
#include <sys/types.h>
#include "epoll.h"
#include "process.h"
#include "jparse.h"
#include "threadpool.h"
#include "utility.h"

#define LISTEN_NUM 20
#define MAX_LINE 100
#define MAX_EVENTS 100000

MYSQL *conn;
int epfd;

//recv & send messages
void* MessageHandlerThread(int *sockfd)
{
    char recvBuf[MAX_BUF + 1];
    char sendBuf[MAX_BUF + 1];
    int newfd, ret;
    int totalSend = strlen(sendBuf);
    char *pSend = sendBuf;
    newfd = *sockfd;
    bzero(recvBuf, MAX_BUF + 1);
    bzero(sendBuf, MAX_BUF + 1);
    int loop = 1;
    while(loop)
    {
	ret = recv(newfd, recvBuf, sizeof(recvBuf), 0);
	if(ret < 0)
	{
	    if(errno == EAGAIN)
		break;
	    else 
	    {
		EpollDel(epfd, newfd);
		return NULL;		    
	    }

	}
	else if(ret == 0)
	{
	    perror("connection closed\n");
	    EpollDel(epfd, newfd);
	    return NULL;
	}
	else
	{
	    if(ret == sizeof(recvBuf))
		loop = 1;
	    else
		loop = 0;
	}

    }
//    fflush(stdout);
    //parse recvdata & handle
    printf("recvBuf : %s\n", recvBuf);
    if(ProcessRecvData(conn, recvBuf, sendBuf, newfd) < 0)
    {
	perror("process error\n");
	return NULL;
    }
    printf("sendBuf : %s\n", sendBuf);
    //in common it is writable
/*    while(1)
      {
      ret = send(newfd, pSend, totalSend, 0);
      if(ret < 0)
      {
      if(errno == EINTR)
      return NULL;
      if(errno == EAGAIN)
      {
      usleep(1000);
      continue;
      }
      return NULL;
      }
      if(ret == totalSend)
      return NULL;
	
      totalSend -= ret;
      pSend += ret;
      }
*/
    ret = send(newfd, sendBuf, sizeof(sendBuf), 0);
    if(ret < 0)
    {
	perror("send error\n");
    }
    EpollDel(epfd, newfd);
    close(newfd);
    return NULL;
}
int main()
{
    struct epoll_event ev, events[MAX_EVENTS];
    struct sockaddr_in client;
    struct sockaddr_in server;
//    const char* localhost = "127.0.0.1";
    const char* localhost = "0.0.0.0";
    int tcpfd, udpfd, nfds, confd;
    int servPort = 8888;
    socklen_t addrLen = sizeof(struct sockaddr);
    char line[MAX_LINE];
    ssize_t n;
    MYSQL connection;
    pthread_t thread;
    pthread_attr_t attr;

    //create socket
    tcpfd = CreateSocket(TCP);
    //translate ip & port into sockaddr_in format
    Ip2SocketAddr(&server, localhost, servPort);
    //bind ip & port to socket
    BindSocketAddr(&server, tcpfd);
    //begin to listen
    listen(tcpfd, LISTEN_NUM);
    //create epoll
    epfd = CreateEpoll(MAX_EVENTS);
    //set fd nonblocking mode
    SetNonBlock(tcpfd);
    //register fd event to epoll
    EpollAdd(epfd, tcpfd, EPOLLIN|EPOLLET);
    //create thread pool
    InitPool(MAX_POOL_SIZE);
    //init a connection to MySql
    conn = mysql_init(NULL);
    if(mysql_real_connect(conn, "localhost", "root", "", "test_server", 0, NULL, 0) == NULL){
	perror("no database connection...\n");
	exit(1);
    }

    while(1){
	if((nfds = EpollWait(epfd, events, 20, 500)) == -1){
	    perror("epoll_wait\n");
	    exit(-1);
	}

	for(int i = 0; i < nfds; i++)
	{
	    if(events[i].data.fd == tcpfd)
	    {
		confd = accept(tcpfd, (struct sockaddr *)&client, &addrLen);
		if(confd < 0)
		{
		    perror("accept error \n");
		    exit(1);
		}
		else
		    printf("connect %s:%d socket :%d\n", inet_ntoa(client.sin_addr), ntohs(client.sin_port), confd);
//		SetNonBlock(confd);
		EpollAdd(epfd, confd, EPOLLIN|EPOLLET);

		if(IsRecvIn(&events[i]))
		    AddTask2Pool(MessageHandlerThread, (void *)&confd);
	    }
	}//for(0 to nfds)
    }
}
