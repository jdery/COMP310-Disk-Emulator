/* NAME: JEAN-SEBASTIEN DERY
 * STUDENT ID: 260 430 688
 * ASSIGNMENT #1
 */

#ifndef MYDISK_H
#define MYDISK_H

#include <stdio.h>
#include <stdlib.h>

#include "mycache.h"

const int BLOCK_SIZE;

const int HDD_SEEK;
const int HDD_READ_LATENCY;
const int HDD_WRITE_LATENCY;
const int SSD_READ_LATENCY;
const int SSD_WRITE_LATENCY;

typedef struct _my_disk {
    FILE *pBlockStore; //the file to store the data blocks
    int max_blocks; //maximum allowed blocks
    int disk_type; //0-HDD, 1-SSD, -1-uninitialized
} MyDisk;

/**
 * initialize the disk
 * file_name: the name of the file you want to store the file blocks
 * max_blocks: the maximum number of blocks stored in disk
 * disk_type: 0-HDD, 1-SSD
 * cache_Switch:0 - off, 1 - on
 */
int init_disk(char const *file_name, size_t max_blocks, int disk_type);

/**
 * read sequential data from the disk to buffer
 * start_address: start address of the data you read
 * nbytes: the length of data you read
 * buffer: the destination of the reading operation
 */
int mydisk_read(int start_address, int nbytes, void *buffer);

/**
 * write data to the continuous area of the disk
 * start_address: start address of the destination of writing operation
 * size: the size of buffer in bytes
 * buffer: the source of the data
 */
int mydisk_write(int start_address, int size, void *buffer);

/**
 * close the disk
 */
void close_disk();

#endif
