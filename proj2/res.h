#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>

#ifndef _RES_DEF
#define _RES_DEF

typedef struct sharedRes{
    FILE *file;
    sem_t mutex;
    sem_t main_wait;
    sem_t santa_sem;
    sem_t reind_sem;
    sem_t elf_sem;
    unsigned count;
    int reindeers;
    int elves;
    int NR;
    char shop_closed;
} sharedRes_t;

void print_log(sharedRes_t *shared, const char* fmt, ...);

#endif