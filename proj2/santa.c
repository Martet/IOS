#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "res.h"

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
    exit(0);
}
