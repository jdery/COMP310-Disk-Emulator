/* NAME: JEAN-SEBASTIEN DERY
 * STUDENT ID: 260 430 688
 * ASSIGNMENT #1
 */

#include "myqueue.h"

int init_queue(int queue_capacity, MyQueue* myqueue) {
    myqueue->max_queue_size = queue_capacity;
    myqueue->queue_len = 0;
    myqueue->head = NULL;
    myqueue->tail = NULL;
    return 0;
}

// Starts at the "tail" and go through the queue using "next".

QueueElement* contains_in_queue(int v, MyQueue* myqueue) {
    if (myqueue->queue_len == 0) {
        return (NULL);
    } else {
        QueueElement *p = myqueue->tail;
        for (; p != NULL; p = p->next) {
            if (p->blkidx == v) {
                return p;
            }
        }
        return NULL;
    }
}

// Adds "newblock" at the end of the queue "tail".

int push(QueueElement* newblock, MyQueue* myqueue) {
    //TODO: if the queue is full, return 1

    QueueElement *tempQE;
    int iSize = myqueue->queue_len;

    // Validate that the queue is not full.
    if (myqueue->max_queue_size == myqueue->queue_len) {
        return (1);
    }

    //TODO: push the newblock into the queue
    //and update the fields of myqueue

    // If queue is empty, the first in is the head, else add the new block at the end of the queue.
    if (iSize == 0) {
        newblock->next = NULL;
        newblock->prev = NULL;
        myqueue->head = newblock;
        myqueue->tail = newblock;
    } else {
        // When the second element is inserted.
        if (iSize == 1) {
            newblock->next = myqueue->head;
            newblock->prev = NULL;
            myqueue->head->prev = newblock;
            myqueue->tail = newblock;
        } else {
            // The general case.
            newblock->prev = NULL;
            newblock->next = myqueue->tail;
            newblock->next->prev = newblock;
            myqueue->tail = newblock;
        }
    }

    // Increment the length of the queue by one.
    iSize++;
    myqueue->queue_len = iSize;
    return 0;
}

//remove the "head" of the queue.

QueueElement* pop(MyQueue* myqueue) {
    QueueElement *p = myqueue->head;
    //TODO: remove the head from the queue

    // Validate that the queue contains elements.
    if (myqueue->queue_len == 0) {
        return (NULL);
    } else {
        // When there is one element in the queue.
        if (myqueue->head->prev == NULL) {
            clean_queue(myqueue);
        } else {
            // The general case.
            myqueue->head = myqueue->head->prev;
            myqueue->head->next = NULL;
            myqueue->queue_len--;
        }
    }
    return p;
}

// Removes the elements in the queue.

void clean_queue(MyQueue* myqueue) {
    //printf("---------- (queue) The queue is been cleaned.\n");
    //TODO:clean the queue
    //be careful to release the memory

    myqueue->head = NULL;
    myqueue->tail = NULL;
    myqueue->queue_len = 0;

    // DOES NOT WORK*****************************
    //free(myqueue);
}

// Print the queue.

void dumpQueue(MyQueue* myqueue) {
    printf("DUMP:");
    QueueElement *p = myqueue->tail;
    for (; p != NULL; p = p->next) {
        printf("BLOCK %d FLAG:%d CONTENT:%s\n", p->blkidx, p->dirtyflag, p->content);
    }
    printf("\n");
}
