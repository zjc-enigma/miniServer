#include "threadpool.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

void InitPool(int threadNum)
{
    pool = (threadPool *)malloc(sizeof(threadPool));
    pthread_mutex_init(&(pool->queueLock), NULL);
    pthread_cond_init(&(pool->queueReady), NULL);

    pool->queueHead = NULL;
    pool->maxThreadNum = threadNum;
    pool->curQueueSize = 0;
    pool->destroy = 0;
    //array of tids
    pool->threadId = (pthread_t *)malloc(threadNum * sizeof(pthread_t));
    //create threads
    for(int i = 0; i < threadNum; i++)
	{
	    pthread_create(&(pool->threadId[i]), NULL, ThreadCell, NULL);
	}
}

int AddTask2Pool(void *(*process)(void *arg), void *arg)
{
    //create new node
    threadTask *newTask = (threadTask *)malloc(sizeof(threadTask));
    newTask->process = process;
    newTask->arg = arg;
    newTask->next = NULL;

    //insert the node to pool queue
    pthread_mutex_lock(&(pool->queueLock));
    threadTask *node;
    if((node = pool->queueHead) != NULL)
	{
	    while(node->next != NULL)
		node = node->next;
	    node->next = newTask;
	}
    else//queue is empty
	{
	    pool->queueHead = newTask;
	}

    assert(pool->queueHead != NULL);
    pool->curQueueSize++;
    pthread_mutex_unlock(&(pool->queueLock));//mutex_lock
    pthread_cond_signal(&(pool->queueReady));
    return 0;
}

int DestroyPool()
{
    if(pool->destroy)
	return -1;
    pool->destroy = 1;
    pthread_cond_broadcast(&(pool->queueReady));
    //waiting for
    for(int i = 0; i < pool->maxThreadNum; i++)
	pthread_join(pool->threadId[i], NULL);
    free(pool->threadId);

    //free queue nodes from head 
    threadTask *head = NULL;
    while(pool->queueHead != NULL)
	{
	    head = pool->queueHead;
	    pool->queueHead = pool->queueHead->next;
	    free(head);
	}
    //free mutex & cond
    pthread_mutex_destroy(&(pool->queueLock));
    pthread_cond_destroy(&(pool->queueReady));
    free(pool);
    //freed point should be NULL
    pool = NULL;
    return 0;
}

void *ThreadCell(void *arg)
{
    printf("thread 0x%x started\n", pthread_self());
    while(1)
    {
	pthread_mutex_lock(&(pool->queueLock));
	while(pool->curQueueSize == 0 && !pool->destroy)
	{
	    printf("thread 0x%x slept\n", pthread_self());
	    pthread_cond_wait(&(pool->queueReady), &(pool->queueLock));
	}
	if(pool->destroy)
	{
	    pthread_mutex_unlock(&(pool->queueLock));
	    printf("thread 0x%x will exit\n", pthread_self());
	    pthread_exit(NULL);
	}

	printf("thread 0x%x started\n", pthread_self());

	assert(pool->curQueueSize != 0);
	assert(pool->queueHead != NULL);

	//execute a task from queue head
	pool->curQueueSize--;
	threadTask *task = pool->queueHead;
	pool->queueHead = task->next;
	pthread_mutex_unlock(&(pool->queueLock));
	
	(*(task->process))(task->arg);
	free(task);
	task = NULL;
    }
    //unreachable
    pthread_exit(NULL);
}



