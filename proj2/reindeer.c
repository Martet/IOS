#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>
#include "res.h"

int reindeer(sharedRes_t *shared, int id, int RT){
    srand(time(NULL) + id + 1234);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: RD %d: rstarted\n", shared->count++, id);
    sem_post(&shared->mutex);

    usleep(((rand() % (RT / 2 + 1)) + RT / 2)  * 1000);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: RD %d: return home\n", shared->count++, id);
    shared->reindeers++;
    if(shared->reindeers == shared->NR){
        sem_post(&shared->santa_sem);
    }
    sem_post(&shared->mutex);

    sem_wait(&shared->reind_sem);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: RD %d: get hitched\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->reind_sem);
    sem_post(&shared->main_wait);
    exit(0);
}
