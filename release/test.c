#include "mydisk.h"
#include <string.h>

extern MyDisk diskEntity;
extern MyCache cacheEntity;
extern const int BLOCK_SIZE;
extern int Disk_Latency;

//constants for measuring performance
const int BLOCK_SIZE = 5;

const int HDD_SEEK = 20;
const int HDD_READ_LATENCY = 100;
const int HDD_WRITE_LATENCY = 120;
const int SSD_READ_LATENCY = 100;
const int SSD_WRITE_LATENCY = 100;

const int CACHE_SIZE = 5;
const int CACHE_WRITE_LATENCY = 10;
const int CACHE_READ_LATENCY = 10;

int CACHE_SWITCH = 0;

int test_latency_value(int nDiskSeek, int nBlockWrite, int nBlockRead, int nCacheWrite, int nCacheRead) {
    int ret = 0;
    if (0 == diskEntity.disk_type) {
        if (Disk_Latency != HDD_SEEK * nDiskSeek + nBlockRead * HDD_READ_LATENCY + nBlockWrite * HDD_WRITE_LATENCY + CACHE_READ_LATENCY * nCacheRead + CACHE_WRITE_LATENCY * nCacheWrite) {
            ret = 1;
            printf("disk latency is wrong:%d, The expected values are (%i,%i,%i,%i,%i). Should be (%i)\n", Disk_Latency, nDiskSeek, nBlockWrite, nBlockRead, nCacheWrite, nCacheRead, (HDD_SEEK * nDiskSeek + nBlockRead * HDD_READ_LATENCY + nBlockWrite * HDD_WRITE_LATENCY + CACHE_READ_LATENCY * nCacheRead + CACHE_WRITE_LATENCY * nCacheWrite));
        }
    } else {
        if (Disk_Latency != nBlockRead * SSD_READ_LATENCY + nBlockWrite * SSD_WRITE_LATENCY + CACHE_READ_LATENCY * nCacheRead + CACHE_WRITE_LATENCY * nCacheWrite) ret = 1;
    }
    Disk_Latency = 0;
    return ret;
}

//test case 1:
//initialize disk with zero

int test_case_1() {
    int i = 0;
    int ret = init_disk("disk1", 100, 0);
    if (ret != 1) {
        char* buffer = (char *) malloc(sizeof (char) * 100);
        fseek(diskEntity.pBlockStore, 0, SEEK_SET);
        fread(buffer, 100, 1, diskEntity.pBlockStore);
        for (i = 0; i < 100; i++) {
            if (buffer[i] != '0') {
                ret = 1;
                break;
            }
        }
        free(buffer);
        close_disk();
    }
    return ret;
}

//test case 2:
//write data into emulated disk

int test_case_2() {
    int ret = init_disk("disk1", 100, 0);
    int i = 0;
    if (1 == ret) return 1;
    mydisk_write(0, 8, "11111111");
    mydisk_write(20, 5, "11111");
    char* buffer = (char *) malloc(sizeof (char) * diskEntity.max_blocks * BLOCK_SIZE);
    fseek(diskEntity.pBlockStore, 0, SEEK_SET);
    fread(buffer, diskEntity.max_blocks * BLOCK_SIZE, 1, diskEntity.pBlockStore);
    for (; i < 500; i++) {
        char c = '0';
        if ((i >= 0 && i <= 7) || (i >= 20 && i <= 24)) c = '1';
        if (buffer[i] != c) {
            ret = 1;
            break;
        }
    }
    free(buffer);
    close_disk();
    if (ret != 1) {
        ret = test_latency_value(2, 3, 0, 0, 0);
    }
    return ret;
}

//test case 3:
//read data from emulated disk

int test_case_3() {
    int i = 0;
    int ret = init_disk("disk1", 100, 0);
    if (1 == ret) return 1;


    mydisk_write(0, 8, "11111111");
    char* buffer = (char *) malloc(sizeof (char) * 10);
    mydisk_read(4, 10, buffer);
    for (; i < 10; i++) {
        char c = '0';
        if (i >= 0 && i <= 3) c = '1';
        if (buffer[i] != c) {
            ret = 1;
            break;
        }
    }
    free(buffer);
    close_disk();
    if (ret != 1) ret = test_latency_value(2, 2, 3, 0, 0);
    return ret;
}

//test case 4:
//write data to a cache enable disk

int test_case_4() {
    int i = 0;
    int ret = init_disk("disk1", 100, 0);
    if (1 == ret) return 1;

    //initialize cache
    init_cache(CACHE_SIZE);
    if (1 == CACHE_SWITCH && NULL == cacheEntity.blockqueue) return 1;

    ret = mydisk_write(0, 8, "11111111");
    ret = mydisk_write(20, 5, "11111");
    ret = mydisk_write(20, 5, "22222");
    close_disk();
    char* buffer = (char *) malloc(sizeof (char) * diskEntity.max_blocks * BLOCK_SIZE);
    FILE *fp = fopen("disk1", "rb");
    fread(buffer, BLOCK_SIZE * diskEntity.max_blocks, 1, fp);
    for (; i < BLOCK_SIZE * diskEntity.max_blocks; i++) {
        char c = '0';
        if (i >= 0 && i <= 7) c = '1';
        if (i >= 20 && i <= 24) c = '2';
        if (buffer[i] != c) {
            ret = 1;
            break;
        }
    }
    free(buffer);
    fclose(fp);
    if (ret != 1) ret = test_latency_value(5, 3, 3, 4, 0);
    return ret;
}

//test case 5:
//read data from cache-enabled disk

int test_case_5() {
    int i = 0;
    int ret = init_disk("disk1", 100, 0);

    if (1 == ret) return 1;
    //initialize cache
    CACHE_SWITCH = 1;
    init_cache(CACHE_SIZE);
    if (1 == CACHE_SWITCH && NULL == cacheEntity.blockqueue) return 1;

    ret = mydisk_write(0, 8, "11111111");
    ret = mydisk_write(20, 5, "11111");
    ret = mydisk_write(20, 5, "22222");
    char* buffer = (char *) malloc(sizeof (char) * 20);
    mydisk_read(4, 20, buffer);
    for (; i < 20; i++) {
        char c = '0';
        if (i >= 0 && i <= 3) c = '1';
        if (i <= 19 && i >= 16) c = '2';
        if (buffer[i] != c) {
            ret = 1;
            break;
        }
    }
    free(buffer);
    close_disk();
    if (ret != 1) ret = test_latency_value(6, 3, 5, 6, 3);
    return ret;
}

//test case 6:
//read data from emulated disk, test LRU

int test_case_6() {
    int i = 0;
    int ret = init_disk("disk1", 100, 0);

    if (1 == ret) return 1;
    //initialize cache
    init_cache(CACHE_SIZE);
    if (1 == CACHE_SWITCH && NULL == cacheEntity.blockqueue) return 1;

    ret = mydisk_write(0, 8, "11111111");
    ret = mydisk_write(20, 10, "2222222222");
    char* buffer = (char *) malloc(sizeof (char) * 30);
    mydisk_read(0, 30, buffer);
    //dumpQueue(cacheEntity.blockqueue);
    for (; i < 30; i++) {
        char c = '0';
        if (i >= 0 && i <= 7) c = '1';
        if (i <= 29 && i >= 20) c = '2';
        if (buffer[i] != c) {
            ret = 1;
            break;
        }
    }
    free(buffer);
    close_disk();
    if (ret != 1) ret = test_latency_value(7, 4, 8, 8, 2);
    return ret;
}

//test case 7:
//read data from emulated disk, test LRU and dirty cache block

int test_case_7() {
    int i = 0;
    int ret = init_disk("disk1", 100, 0);

    if (1 == ret) return 1;
    //initialize cache
    init_cache(CACHE_SIZE);
    if (1 == CACHE_SWITCH && NULL == cacheEntity.blockqueue) return 1;

    ret = mydisk_write(0, 8, "11111111");
    ret = mydisk_write(20, 10, "2222222222");
    ret = mydisk_write(20, 10, "3333333333");
    char* buffer = (char *) malloc(sizeof (char) * 30);
    mydisk_read(0, 30, buffer);
    for (; i < 30; i++) {
        char c = '0';
        if (i >= 0 && i <= 7) c = '1';
        if (i <= 29 && i >= 20) c = '3';
        if (buffer[i] != c) {
            ret = 1;
            break;
        }
    }
    free(buffer);
    close_disk();
    if (ret != 1) ret = test_latency_value(9, 6, 8, 10, 2);
    return ret;
}

int main(int argc, char **argv) {
    char *result[2];
    result[0] = "PASS";
    result[1] = "FAILED";
    //test case 1:
    //initialize disk with zero
    printf("TEST CASE 1:%s\n", result[test_case_1()]);
    //test case 2:
    //write data into emulated disk
    printf("TEST CASE 2:%s\n", result[test_case_2()]);
    //test case 3:
    //read data from emulated disk
    printf("TEST CASE 3:%s\n", result[test_case_3()]);
    //test case 4:
    //write data to a cache enable disk
    printf("TEST CASE 4:%s\n", result[test_case_4()]);
    //test case 5:
    //write data to a cache enable disk
    printf("TEST CASE 5:%s\n", result[test_case_5()]);
    //test case 6:
    //write data to a cache enable disk, test LRU
    printf("TEST CASE 6:%s\n", result[test_case_6()]);
    //test case 7:
    //write data to a cache enable disk, test LRU and dirty cache block
    printf("TEST CASE 7:%s\n", result[test_case_7()]);
    return 0;
}
