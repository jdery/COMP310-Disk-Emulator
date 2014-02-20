/* NAME: JEAN-SEBASTIEN DERY
 * STUDENT ID: 260 430 688
 * ASSIGNMENT #1
 */

#ifndef MYCACHE_H
#define MYCACHE_H

#include <stdio.h>
#include <stdlib.h>
#include "mydisk.h"
#include "myqueue.h"

const int CACHE_SIZE;
const int CACHE_WRITE_LATENCY;
const int CACHE_READ_LATENCY;

int CACHE_SWITCH;
//the structure of cache

typedef struct _cache_ {
    MyQueue* blockqueue; //the queue to store blocks
    int hitCount; //counter of cache-hit
    int totalCount; //counter of total cache reference
} MyCache;

/**
 * initialize the cache
 * nblocks: the maximum number of blocks in the cache
 * cacheEntity: the cache to be initialized
 */
int init_cache(int nblocks);

/**
 * query if certain block is in the cache
 * blockIdx: the index of the block to be checked*/
int query_cache(int blockIdx);

/**
 * query cache to see if block v is cached
 * v: the index of the block
 * return NULL if block v is not there, otherwise, return the content of the block
 */
char *read_cache(int v);


/**
 * update the content of a block in the cache 
 * v: the index of the block
 * block_data: the data to be written in the cache
 * return 1 if block v is not there, otherwise, return 0
 */
int write_cache(int v, char *block_data, size_t blkOffset, size_t contentSize);

/**
 * the operation performed when a block is referred 
 * block: the accessed block
 * cacheEntity: the cache storing blocks
 */
void block_referred(QueueElement *block);
/**
 * add new entry to cache
 * v: the index of the new block
 * block_to_cached: the content of the new block
 * cacheEntity: the cache storing blocks
 */
int add_cache_entry(int v, char *block_to_cached);
/**
 * clean the cache
 * cacheEntity:the cache to be cleared
 */
void close_cache();
/**
 * mark the block to be dirty
 * blkIdx: index of the block to be touched
 * cacheEntity: the cache storing blocks
 */
void mark_as_dirty(int blkIdx);
#endif
