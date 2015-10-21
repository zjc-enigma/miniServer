#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>

#define MAX_BUF 5000
#define MAX_POOL_SIZE 50

//task in the pool   
typedef struct task
{
    void *arg;
    void * (*process)(void *arg);//call back fuction  
    struct task *next;
}threadTask;

typedef struct
{
    pthread_mutex_t queueLock;
    pthread_cond_t queueReady;
    //task queue head
    threadTask * queueHead;
    //whether destroy the pool
    int destroy;
    pthread_t *threadId;
    //num of thread in pool
    int maxThreadNum;
    //current waiting queue size
    int curQueueSize;
}threadPool;

static threadPool *pool = NULL;

int AddTask2Pool(void *(*process)(void *arg), void *arg);

void *ThreadCell(void *arg);

//create thread pool
void InitPool(int threadNum);

//destroy
int DestroyPool();

//void *MessageHandlerThread(int *sockfd);

#endif //THREADPOOL_H
