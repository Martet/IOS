#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <time.h>
#include "res.h"

int reindeer(sharedRes_t *shared, int id, int RT){
    sem_wait(shared->mutex);
    printf("%d: RD %d: rstarted\n", shared->count, id);
    shared->count++;
    sem_post(shared->mutex);

    nanosleep(&(struct timespec){.tv_nsec = ((rand() % (RT / 2 + 1)) + RT / 2)  * 1000}, NULL);

    sem_wait(shared->mutex);
    printf("%d: RD %d: return home\n", shared->count, id);
    shared->count++;
    sem_post(shared->mutex);

    exit(0);
}
