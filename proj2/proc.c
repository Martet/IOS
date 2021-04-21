#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "res.h"

int elf(sharedRes_t *shared, int id, int ET){
    srand(time(NULL) + id + 4321);

    while(1){
        sem_wait(&shared->mutex);
        print_log(shared, "%d: Elf %d: started\n", shared->count++, id);
        sem_post(&shared->mutex);

        usleep((rand() % (ET + 1)) * 1000);
    }
    

    sem_wait(&shared->mutex);
    print_log(shared, "%d: Elf %d: taking holidays\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    return 0;
}

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
    return 0;
}

int santa(sharedRes_t *shared){
    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: going to sleep\n", shared->count++);
    sem_post(&shared->mutex);

    while(1){
        sem_wait(&shared->santa_sem);
        if(shared->reindeers == shared->NR)
            break;
        
    }
    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: closing workshop\n", shared->count++);
    shared->shop_closed = 1;
    sem_post(&shared->mutex);

    for(int i = 0; i < shared->NR; i++){
        sem_post(&shared->reind_sem);
    }
    for(int i = 0; i < shared->NR; i++){
        sem_wait(&shared->reind_sem);
    }

    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: Christmas started\n", shared->count++);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    return 0;
}