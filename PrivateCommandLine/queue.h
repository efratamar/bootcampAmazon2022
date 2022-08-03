#ifndef QUEUE_H
#define QUEUE_H
#include<semaphore.h>

typedef struct node{
    void* data;
    struct node* next;
}Node;
typedef struct queue{
    Node*head;
    Node*tail;
    int capacity;
    int counter;
    sem_t sema;
}Queue,*p_queue;

int enqueue(Queue*,Node*);
Node* dequeue(Queue*);
Node* createNode(void*);
Queue* createQueue(int);
void freeNode(Node*);
void freeQueue(Queue*);
int isEmpty(Queue*);
Node* top(Queue*);
#endif // QUEUE_H
