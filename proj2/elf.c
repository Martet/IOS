#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include "res.h"

int elf(sharedRes_t *shared, int id, int ET){
    sem_wait(shared->mutex);
    printf("%d: Elf %d: started\n", shared->count, id);
    shared->count++;
    sem_post(shared->mutex);

    nanosleep(&(struct timespec){.tv_nsec = (rand() % (ET + 1)) * 1000}, NULL);

    sem_wait(shared->mutex);
    printf("%d: Elf %d: taking holidays\n", shared->count, id);
    shared->count++;
    sem_post(shared->mutex);
    exit(0);
}
