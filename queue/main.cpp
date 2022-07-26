#include <QCoreApplication>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include"Queue.h"
#define MAX 5

void* enter(void* queue)
{
    Queue*q=(Queue*)queue;
    for(int i=0;;i++)
        {
            enqueue(q,createNode((void*)i));
            fprintf(stderr,"%d was enqueued",i);
        }
}
void* remove(void* queue)
{
    Queue*q=(Queue*)queue;
    for(int i=0;;i++)
    {
        dequeue(q);
    }
}
int main()
{
    pthread_t thread1;
    pthread_t thread2;
    Queue* queue=createQueue(MAX);
    pthread_create(&thread1,NULL,enter,(void*)queue);
    pthread_create(&thread2,NULL,remove,(void*)queue);
    pthread_join(thread1,NULL);
    pthread_join(thread2,NULL);
    return 0;
}
