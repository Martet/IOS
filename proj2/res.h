#ifndef _RES_DEF
#define _RES_DEF

#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>

//shared structure
typedef struct sharedRes{
    FILE *file;
    sem_t mutex;
    sem_t main_wait;
    sem_t santa_sem;
    sem_t reind_sem;
    sem_t elf_sem;
    sem_t elfHelp_sem;
    sem_t reindHitch_sem;
    sem_t elfDone_sem;
    unsigned count;
    int reindeers;
    int elves;
    char shop_closed;
} sharedRes_t;

//prints formatted string to file in shared structure
void print_log(sharedRes_t *shared, const char* fmt, ...);

#endif
