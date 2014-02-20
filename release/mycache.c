/* NAME: JEAN-SEBASTIEN DERY
 * STUDENT ID: 260 430 688
 * ASSIGNMENT #1
 */

#include <string.h>

#include "mydisk.h"
#include "mycache.h"
#include "myqueue.h"

extern MyDisk diskEntity;
extern MyCache cacheEntity;

extern const int BLOCK_SIZE;
extern const int CACHE_SIZE;

extern int Disk_Latency;
extern int writeLatency;
extern int readLatency;

int init_cache(int nblocks) {
    if (nblocks <= 0) return 1;

    //initialize queue first
    //TODO: initialize the blockqueue
    CACHE_SWITCH = 1;

    // Allocates the memory
    cacheEntity.blockqueue = (MyQueue *) malloc(nblocks);

    // Initialize the queue.
    init_queue(nblocks, cacheEntity.blockqueue);

    return 0;
}

void mark_as_dirty(int blkIdx) {
    QueueElement *ele = contains_in_queue(blkIdx, cacheEntity.blockqueue);
    ele->dirtyflag = 1;
}

void close_cache() {
    clean_queue(cacheEntity.blockqueue);
    cacheEntity.hitCount = cacheEntity.totalCount = 0;
}

// It takes the referred block and move it at the end of the queue.

void block_referred(QueueElement *block) {
    if (NULL == block->prev) return; //first cached block
    //TODO: move the referred block to the tail of the queue

    QueueElement *qTemp = cacheEntity.blockqueue->tail;

    //dumpQueue(cacheEntity.blockqueue);
    // If the block exists in the queue.
    if (query_cache(block->blkidx)) {
        // Will find the position of the block in the queue.
        while (qTemp->blkidx != block->blkidx) {
            qTemp = qTemp->next;
        }
        // If the referred block is the Head.
        if (qTemp->next == NULL) {
            push(pop(cacheEntity.blockqueue), cacheEntity.blockqueue);
        } else {
            block->next->prev = block->prev;
            block->prev->next = block->next;
            cacheEntity.blockqueue->tail->prev = block;
            cacheEntity.blockqueue->tail = block;
        }
    }
}

// 0 if it is not in the cache and 1 if it is.

int query_cache(int blockIdx) {
    QueueElement* ele = contains_in_queue(blockIdx, cacheEntity.blockqueue);
    return NULL == ele ? 0 : 1;
}

char* read_cache(int v) {
    //printf("----- (cache) Reading cache.\n");
    //NOTICE:we implement LRU by double linked-list,
    //while in real word, we check if certain block is in the cache by hash-based method, whose latency can be ignored, so you DO NOT need to count in the latency for this operation
    //but when a block is hit and you read its data from the cache, you have to add Disk_Latency with CACHE_READ_LATENCY;
    int foundInCache = query_cache(v);
    cacheEntity.totalCount++;
    if (1 == foundInCache) {
        QueueElement* ele = contains_in_queue(v, cacheEntity.blockqueue);
        //contains in the cache
        cacheEntity.hitCount++;
        block_referred(ele);
        //Disk_Latency += CACHE_READ_LATENCY;
        return ele->content;
    }
    return NULL;
}

// I must update only the bits that have changed.

int write_cache(int v, char *block_data, size_t blkOffset, size_t contentSize) {
    QueueElement *ele;
    int i;
    int foundInCache = query_cache(v);
    char *cUpdate = (char *) malloc(contentSize);

    cacheEntity.totalCount++;
    if (1 == foundInCache) {
        //TODO:update the content of the block

        // I search the element in the queue.
        ele = contains_in_queue(v, cacheEntity.blockqueue);
        cUpdate = ele->content;

        // Update the content of the block.
        for (i = 0; i < contentSize; i++) {
            cUpdate[blkOffset + i] = block_data[i];
        }

        ele->content = cUpdate;

        // Mark it as dirty.
        mark_as_dirty(v);
        // If the block is there return 0.
        return 0;
    }

    // If the block is not there return 1.
    return 1;
}

int add_cache_entry(int v, char *block_buffer) {
    if (v < 0 || NULL == block_buffer) return 1;
    //TODO:construct a new queue element

    QueueElement *block_to_cache = (QueueElement *) malloc(sizeof (QueueElement));
    block_to_cache->content = block_buffer;
    block_to_cache->blkidx = v;
    block_to_cache->dirtyflag = 0;
    block_to_cache->next = NULL;
    block_to_cache->prev = NULL;

    //add new element
    // Returns (1) when the cache is full.
    if (1 == push(block_to_cache, cacheEntity.blockqueue)) {
        int i;
        //full cache
        //performing LRU algorithm
        //TODO:replace the least recent use block, and write back the
        //block data to disk

        // Remove the head.
        QueueElement *qPop = pop(cacheEntity.blockqueue);
        // If dirty write it to the disk.
        if (qPop->dirtyflag == 1) {
            fseek(diskEntity.pBlockStore, (qPop->blkidx * BLOCK_SIZE), SEEK_SET);
            for (i = 0; i < BLOCK_SIZE; i++) {
                fwrite(qPop->content + i, 1, 1, diskEntity.pBlockStore);
            }
            // Calculation for the disk latency for each block.
            if (diskEntity.disk_type == 0) {
                Disk_Latency += HDD_SEEK + HDD_WRITE_LATENCY;
            } else if (diskEntity.disk_type == 1) {
                Disk_Latency += SSD_WRITE_LATENCY;
            }
        }
        push(block_to_cache, cacheEntity.blockqueue);
    }
    return 0;
}
