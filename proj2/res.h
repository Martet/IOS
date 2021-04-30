#ifndef __RES_H__
#define __RES_H__

#include <semaphore.h>
#include <stdio.h>

//shared structure
typedef struct sharedRes{
    sem_t mutex;
    sem_t main_wait;
    sem_t santa_sem;
    sem_t reind_sem;
    sem_t elf_sem;
    sem_t elfHelp_sem;
    sem_t reindHitch_sem;
    sem_t elfDone_sem;
    FILE *file;
    unsigned int count;
    int reindeers;
    int elves;
    char shop_closed;
} sharedRes_t;

//prints formatted string to file in shared structure
#define print_log(...) do{                  \
    fprintf(shared->file, __VA_ARGS__);    \
    fflush(shared->file);                   \
} while(0)

#endif
