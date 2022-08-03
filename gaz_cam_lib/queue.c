#include<stdio.h>
#include<stdlib.h>
#include"queue.h"

int enqueue(Queue* queue,Node* node)
{
    if(!queue)return 1;
    sem_wait(&queue->sema);
    if(queue->counter==queue->capacity){
        sem_post(&queue->sema);
        return 1;
    }
    else{
        if(queue->head==NULL)
            (queue)->head=node;
        else
            queue->tail->next=node;
        queue->counter++;
        queue->tail=node;
    }
    sem_post(&queue->sema);
    return 0;
}
Node* dequeue(Queue* queue)
{
    if(!queue)return NULL;
    if(queue->counter==0)
        return NULL;
    sem_wait(&queue->sema);
    queue->counter--;
    Node *n=queue->head;
    queue->head=queue->head->next;
    sem_post(&queue->sema);
    return n;
}
Node* createNode(void* data)
{
    Node* node=(Node*)malloc(sizeof(Node));
    if(!node)exit(1);
    node->data=data;
    node->next=NULL;
    return node;
}
Queue* createQueue(int max)
{
    Queue* queue=(Queue*)malloc(sizeof(Queue));
    if(!queue)exit(1);
    queue->capacity=max;
    queue->counter=0;
    queue->head=NULL;
    queue->tail=NULL;
    sem_init(&queue->sema,0,1);
    return queue;
}
void freeNode(Node* node)
{
    if(!node)
        free(node);
}
void freeQueue(Queue* queue)
{
    if(queue){
        while(queue->head)
            freeNode(dequeue(queue));
        free(queue);
    }
}
int isEmpty(Queue* queue){
    if(!queue)
        return 1;
    return !(queue->counter);
}
Node* top(Queue* queue)
{
    if(!queue)return NULL;
    sem_wait(&queue->sema);
    if(!isEmpty(queue))
    {
        sem_post(&queue->sema);
        return queue->head;
    }
    sem_post(&queue->sema);
    return NULL;
}
