/* NAME: JEAN-SEBASTIEN DERY
 * STUDENT ID: 260 430 688
 * ASSIGNMENT #1
 */

#ifndef MYSTACK_H
#define MYSTACK_H

#include <stdio.h>
#include <stdlib.h>

/**
 * we implement the LRU algorithm with Queue based on double link list,
 * when a block is referred, it's moved to the tail of the Queue,
 * and when Queue is full, we remove the block at the head
 */

//QueueElement describe the block in the cache
typedef struct _queue_ele_
{
	int blkidx;//block's index
	char *content;//the data of the block
	int dirtyflag;//0 - clean, 1 - dirty
	struct _queue_ele_ *prev;
	struct _queue_ele_ *next;
}QueueElement;

typedef struct _queue_
{
	int queue_len;
	int max_queue_size;
	QueueElement *head;
	QueueElement *tail;
}MyQueue;

/**
 * initialize the queue 
 * queueCapacity: the maximum number of elements in the queue
 * myqueue: the queue to be initilized
 */
int init_queue(int queueCapacity, MyQueue* myqueue);
/**
 * check the block v is in the queue
 * v: index of the interested block
 * myqueue: the queue storing elements
 */
QueueElement* contains_in_queue(int v, MyQueue* myqueue);
/**
 * pop a element from the queue
 * myqueue: the queue storing elements
 * return: return the element just be poped
 */
QueueElement* pop(MyQueue* myqueue);
/**
 * push a new element to the queue
 * newblock: the element to be pushed
 * myqueue: the queue storing blocks
 */
int push(QueueElement* newblock, MyQueue* myqueue);
/**
 * clean the queue
 * myqueue: the queue storing elements
 */
void clean_queue(MyQueue* myqueue);
/**
 * display the elements in the queue
 * myqueue: the queue storing elements
 */
void dumpQueue(MyQueue *myqueue);
#endif
