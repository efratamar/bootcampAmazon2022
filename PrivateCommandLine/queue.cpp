#include<stdio.h>
#include<stdlib.h>
#include"queue.h"

void enqueue(Queue* queue,Node* node)
{
    if(queue->capacity==queue->counter&&node)
        free(node);
    else
        {
            sem_wait(&queue->sema);
            if(queue->tail==NULL)
            queue->tail=queue->head=node;
            queue->tail->next=node;
            queue->counter++;
            sem_post(&queue->sema);
        }
}
Node* dequeue(Queue* queue)
{
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
    while(queue->head)
        freeNode(dequeue(queue));
    free(queue);
}
int isEmpty(Queue* queue){
    return !(queue->counter);
}
