/* NAME: JEAN-SEBASTIEN DERY
 * STUDENT ID: 260 430 688
 * ASSIGNMENT #1
 */

#include "mydisk.h"
#include "mycache.h"
#include <memory.h>
#include <stdio.h>

MyDisk diskEntity = {NULL, 0, -1};
MyCache cacheEntity = {NULL, 0, 0};

int Disk_Latency = 0;

int readLatency = 0;
int writeLatency = 0;

int iDiskRange = 0;

int init_disk(char const *file_name, size_t max_blocks, int disk_type) {

    // Create "cBlock" and allocate memory for "BLOCK_SIZE" size.
    char *cBlock = (char *) malloc(BLOCK_SIZE*max_blocks);

    //TODO:set values of fields of MyDisk with parameters

    // Validating that the inputs are meaningfull values.
    if ((file_name == "") || (file_name == NULL)) {
        printf("//---> The file MUST have a name.\n");
        return (1);
    } else if ((max_blocks <= 0)) {
        printf("//---> The file MUST have a size.\n");
        return (1);
    } else if ((disk_type != 0) && (disk_type != 1)) {
        printf("//---> The disc type is not valid.\n", disk_type);
        return (1);
    }

    // Create a variable for the maximum disk address range.
    iDiskRange = max_blocks * (BLOCK_SIZE - 1);
    
    // Initialise the new file with the input values.
    diskEntity.pBlockStore = fopen(file_name, "w+");
    diskEntity.max_blocks = max_blocks;
    diskEntity.disk_type = disk_type;

    //initialize the metrics
    Disk_Latency = 0;
    if (diskEntity.disk_type == 0) {
        readLatency = HDD_READ_LATENCY;
        writeLatency = HDD_WRITE_LATENCY;
    } else {
        readLatency = SSD_READ_LATENCY;
        writeLatency = SSD_WRITE_LATENCY;
    }
    //TODO: fill the disk with zeros

    // Filling "cBlock" with '0' for "BLOCK_SIZE" bits.
    memset(cBlock, '0', (BLOCK_SIZE*max_blocks));
    
    // Filling the file with "max_blocks*BLOCK_SIZE" bits.
    fwrite(cBlock, 1, (BLOCK_SIZE*max_blocks), diskEntity.pBlockStore);
    
    //TODO: after you finish this function, remember to modify the return value to 0
    return (0);
}

int mydisk_read(int start_address, int nbytes, void *buffer) {
    //TODO:
    //startBlockIdx and endBlockIdx indicate start and end block
    //of this writing operation
    //pls carefully calculate them
    int startBlockIdx = 0;
    int endBlockIdx = 0;
    int i = 0, j = 0;
    int iEndAddress = start_address + nbytes -1;
    int iCurrPosition = 0;
    int iNumberOfBlocks;
    char *cCache = (char *)malloc(BLOCK_SIZE);
    int bSeek = 0;

    startBlockIdx = start_address / BLOCK_SIZE;
    endBlockIdx = (start_address + nbytes - 1) / BLOCK_SIZE;

    iNumberOfBlocks = endBlockIdx - startBlockIdx + 1;

    // Validating that the parameters are good for the operation.
    if (start_address < 0) {
        return (1);
    } else if (nbytes < 0) {
        return (1);
    } else if ((start_address + nbytes) >= (diskEntity.max_blocks*BLOCK_SIZE) ) {
        return (1);
    }

    size_t bufferOffset = 0;

    for (i = startBlockIdx; i < endBlockIdx + 1; i++) {
        int blockInCacheFlag = 0; //0 - no, 1 - yes

        if (1 == CACHE_SWITCH) blockInCacheFlag = query_cache(i);

        // The time required to move between sector for the first block.
        if ((diskEntity.disk_type == 0) && !blockInCacheFlag && !bSeek) {
            Disk_Latency += HDD_SEEK;
            bSeek = 1;
        }
        
        if (0 == blockInCacheFlag) {
            //if cache is missed or disabled
            //TODO:read data from disk

            // Calculation for the disk latency for each block.
            if (diskEntity.disk_type == 0) {
                Disk_Latency += HDD_READ_LATENCY;
            } else if (diskEntity.disk_type == 1) {
                Disk_Latency += SSD_READ_LATENCY;
            }

            for (j = 0 ; j < BLOCK_SIZE; j++) {
                iCurrPosition = i*BLOCK_SIZE+j;
                if ((iCurrPosition >= start_address) && (iCurrPosition <= iEndAddress)) {
                    fseek(diskEntity.pBlockStore, iCurrPosition, SEEK_SET);
                    fread(buffer+bufferOffset, 1, 1, diskEntity.pBlockStore);
                    // Add the content in "cCache" to have a full block of information.
                    bufferOffset++;
                }
            }

            if (1 == CACHE_SWITCH) {
                // Add the latency for the caching.
                if (diskEntity.disk_type == 0) {
                    Disk_Latency += CACHE_WRITE_LATENCY;
                } else if (diskEntity.disk_type == 1) {
                    Disk_Latency += CACHE_WRITE_LATENCY;
                }

                // Reading the data on the disk to have the full block in the cache.
                for (j = 0 ; j < BLOCK_SIZE; j++) {
                    iCurrPosition = i*BLOCK_SIZE+j;
                    if ((iCurrPosition >= start_address) && (iCurrPosition <= iEndAddress)) {
                        // Add the content in "cCache" to have a full block of information.
                        fseek (diskEntity.pBlockStore, iCurrPosition, SEEK_SET);
                        fread(cCache+j, 1, 1, diskEntity.pBlockStore);
                    } else {
                        // Will read and store the part of the block which is not in the passed buffer
                        // to have a complete block in the cache.
                        fseek (diskEntity.pBlockStore, iCurrPosition, SEEK_SET);
                        fread(cCache+j, 1, 1, diskEntity.pBlockStore);
                    }
                }

                //TODO: if cache is enabled, cache this block
                add_cache_entry(i, cCache);
                cCache = (char *)malloc(BLOCK_SIZE);
            }
        } else {
            Disk_Latency += CACHE_READ_LATENCY;
            //if this block is cached, 
            //TODO:read this block from cache through read_cache(int)
            char *cCachedBlock = read_cache(i);
            for (j = 0 ; j < BLOCK_SIZE ; j++) {
                iCurrPosition = i*BLOCK_SIZE+j;
                if ((iCurrPosition >= start_address) && (iCurrPosition <= iEndAddress)) {
                    memset(buffer+bufferOffset, cCachedBlock[j], 1);
                    bufferOffset++;
                }
            }
        }
        //TODO:copy the data into the buffer[
        //take care about the offsets
    }

    return 0;
}

void printDisk() {
    //==========================================================================
    // Display the content of the file.
    printf("----- Displaying the content of the file: \n");
    char *cDisplay = (char *) malloc(BLOCK_SIZE*diskEntity.max_blocks);
    int m;
    fseek(diskEntity.pBlockStore, 0, SEEK_SET);
    for (m = 0 ; m < (BLOCK_SIZE*diskEntity.max_blocks) ; m++) {
        fread(cDisplay+m, 1, 1, diskEntity.pBlockStore);
        printf("(%c)", cDisplay[m]);
        if (m%20 == 1) {
            printf("\n");
        }
    }
    free(cDisplay);
    printf("\n");
    //==========================================================================
}

int mydisk_write(int start_address, int nbytes, void *buffer) {
    //TODO:
    //startBlockIdx and endBlockIdx indicate start and end block
    //of this writing operation
    //pls carefully calculate them
    int startBlockIdx = 0;
    int endBlockIdx = 0;
    int i = 0, iBufferTarget = 0, j = 0;
    int iEndAddress = start_address + nbytes -1;
    int iCurrPosition = 0;
    int iNumberOfBlocks;
    char *cCache = (char *)malloc(BLOCK_SIZE);
    int bSeek = 0;
    
    //printf("*** mydisk_write(%i, %i, %s), buffersize(%i)\n",start_address, nbytes, buffer, strlen(buffer));

    startBlockIdx = start_address / BLOCK_SIZE;
    endBlockIdx = (start_address + nbytes - 1) / BLOCK_SIZE;

    iNumberOfBlocks = endBlockIdx - startBlockIdx + 1;
    
    // Validating that the parameters are good for the operation.
    if (start_address < 0) {
        return (1);
    } else if (nbytes < 0) {
        return (1);
    } else if ((start_address + nbytes) >= (diskEntity.max_blocks*BLOCK_SIZE) ) {
        return (1);
    }

    for (i = startBlockIdx; i < endBlockIdx + 1; i++) {
        //TODO:write block by block
        //be careful for some extreme case
        //e.g. start_address and start_address are not start or end of a block
        
        // The code under is for when the cache is enabled.
        int blockInCacheFlag = 0; //0 - no, 1 - yes

        //check if the current block is cached
        if (1 == CACHE_SWITCH) blockInCacheFlag = query_cache(i);

        // The time required to move between sector for the first block.
        if ((diskEntity.disk_type == 0) && !blockInCacheFlag && !bSeek) {
            //printf("----- (Seek sector): 1S\n");
            Disk_Latency += HDD_SEEK;
            bSeek = 1;
        }

        if (0 == blockInCacheFlag) {
            
            // Calculation for the disk latency for each block.
            if (diskEntity.disk_type == 0) {
                Disk_Latency += HDD_WRITE_LATENCY;
            } else if (diskEntity.disk_type == 1) {
                Disk_Latency += SSD_WRITE_LATENCY;
            }
            
            //TODO:write data into the disk
            for (j = 0 ; j < BLOCK_SIZE; j++) {
                iCurrPosition = i*BLOCK_SIZE+j;
                if ((iCurrPosition >= start_address) && (iCurrPosition <= iEndAddress)) {
                    fseek (diskEntity.pBlockStore, iCurrPosition, SEEK_SET);
                    fwrite(buffer+iBufferTarget, 1, 1, diskEntity.pBlockStore);
                    iBufferTarget++;
                }
            }

            //TODO:if cache is enabled, cache current block
            if (1 == CACHE_SWITCH) {
                // Add the latency for the caching.
                if (diskEntity.disk_type == 0) {
                    Disk_Latency += HDD_SEEK + HDD_READ_LATENCY + CACHE_WRITE_LATENCY;
                } else if (diskEntity.disk_type == 1) {
                    Disk_Latency += SSD_READ_LATENCY + CACHE_WRITE_LATENCY;
                }

                // Reading the data on the disk to have the full block in the cache.
                for (j = 0 ; j < BLOCK_SIZE; j++) {
                    iCurrPosition = i*BLOCK_SIZE+j;
                    if ((iCurrPosition >= start_address) && (iCurrPosition <= iEndAddress)) {
                        // Add the content in "cCache" to have a full block of information.
                        fseek (diskEntity.pBlockStore, iCurrPosition, SEEK_SET);
                        fread(cCache+j, 1, 1, diskEntity.pBlockStore);
                    } else {
                        // Will read and store the part of the block which is not in the passed buffer
                        // to have a complete block in the cache.
                        fseek (diskEntity.pBlockStore, iCurrPosition, SEEK_SET);
                        fread(cCache+j, 1, 1, diskEntity.pBlockStore);
                    }
                }

                add_cache_entry(i, cCache);
                // Reset the memory.
                cCache = (char *)malloc(BLOCK_SIZE);
            }
        }//if yes, write the new content into the cache,
            //and make the dirty flag of cached blk to be 1
        else {
            //TODO: if current block is cached,
            //then modify the data in cache and mark it as dirty
            size_t blkOffset = 0;
            size_t contentSize = 0;
            char *block_data = (char *) malloc(BLOCK_SIZE);

            // Setting the offset from the start of the block.
            if (start_address > (i * BLOCK_SIZE)) {
                blkOffset = start_address - (i * BLOCK_SIZE);
            } else {
                blkOffset = 0;
            }
            // Setting the number of bits to update in the cache.
            if ((start_address + nbytes) < ((i + 1) * BLOCK_SIZE)) {
                contentSize = BLOCK_SIZE - blkOffset - (((i + 1) * BLOCK_SIZE) - (start_address + nbytes));
            } else {
                contentSize = BLOCK_SIZE - blkOffset;
            }

            // Determining "data_block" to pass it in the function.
            for (j = 0; j < BLOCK_SIZE; j++) {
                iCurrPosition = i * BLOCK_SIZE + j;
                if ((iCurrPosition >= start_address) && (iCurrPosition <= iEndAddress)) {
                    fwrite(buffer + iBufferTarget, 1, 1, diskEntity.pBlockStore);
                    block_data[j] = *((char*) buffer + iBufferTarget);
                    iBufferTarget++;
                }
            }

            write_cache(i, block_data, blkOffset, contentSize);

            Disk_Latency += CACHE_WRITE_LATENCY;
        }
    }

    return 0;
}

void close_disk() {
    //permanent caching data
    if (1 == CACHE_SWITCH) {
        int i = 0;
        //TODO: when cache is enabled
        //you need to clean cache,
        //if block is dirty, you need to write back the block data to disk
        //NOTICE: you don't need to update the performance metrics here
        while (cacheEntity.blockqueue->queue_len != 0) {
            QueueElement *qPop = pop(cacheEntity.blockqueue);
            // If dirty write it to the disk.
            if (qPop->dirtyflag == 1) {
                fseek(diskEntity.pBlockStore, (qPop->blkidx * BLOCK_SIZE), SEEK_SET);
                for (i = 0; i < BLOCK_SIZE; i++) {
                    fwrite(qPop->content + i, 1, 1, diskEntity.pBlockStore);
                }
            }
        }
        // Deleting the queue.
        clean_queue(cacheEntity.blockqueue);
    }
    fclose(diskEntity.pBlockStore);
    diskEntity.pBlockStore = NULL;
}

