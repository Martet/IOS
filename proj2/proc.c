#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "res.h"

int elf(sharedRes_t *shared, int id, int ET){
    srand(time(NULL) + id + 4321);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: Elf %d: started\n", shared->count++, id);
    sem_post(&shared->mutex);

    while(1){
        usleep((rand() % (ET + 1)) * 1000);
        sem_wait(&shared->mutex);
        print_log(shared, "%d: Elf %d: need help\n", shared->count++, id);
        sem_post(&shared->mutex);

        sem_wait(&shared->elf_sem);

        sem_wait(&shared->mutex);
        shared->elves++;
        if(shared->elves == 3)
            sem_post(&shared->santa_sem);
        else
            sem_post(&shared->elf_sem);
        sem_post(&shared->mutex);

        sem_wait(&shared->elfHelp_sem);

        if(shared->shop_closed)
            break;

        sem_wait(&shared->mutex);
        print_log(shared, "%d: Elf %d: get help\n", shared->count++, id);
        shared->elves--;
        if(shared->elves == 0){
            sem_post(&shared->elfDone_sem);
            sem_post(&shared->elf_sem);
        }
        sem_post(&shared->mutex);
    }
    
    sem_wait(&shared->mutex);
    print_log(shared, "%d: Elf %d: taking holidays\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    return 0;
}

int reindeer(sharedRes_t *shared, int id, int NR, int RT){
    srand(time(NULL) + id + 1234);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: RD %d: rstarted\n", shared->count++, id);
    sem_post(&shared->mutex);

    usleep(((rand() % (RT / 2 + 1)) + RT / 2)  * 1000);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: RD %d: return home\n", shared->count++, id);
    shared->reindeers++;
    if(shared->reindeers == NR){
        sem_post(&shared->santa_sem);
    }
    sem_post(&shared->mutex);

    sem_wait(&shared->reindHitch_sem);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: RD %d: get hitched\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->reind_sem);
    sem_post(&shared->main_wait);
    return 0;
}

int santa(sharedRes_t *shared, int NR, int NE){
    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: going to sleep\n", shared->count++);
    sem_post(&shared->mutex);

    while(1){
        sem_wait(&shared->santa_sem);
        if(shared->reindeers == NR)
            break;
        else if(shared->elves == 3){
            sem_wait(&shared->mutex);
            print_log(shared, "%d: Santa: helping elves\n", shared->count++);
            sem_post(&shared->mutex);

            for(int i = 0; i < 3; i++)
                sem_post(&shared->elfHelp_sem);

            sem_wait(&shared->elfDone_sem);
            sem_wait(&shared->mutex);
            print_log(shared, "%d: Santa: going to sleep\n", shared->count++);
            sem_post(&shared->mutex);
        }
    }
    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: closing workshop\n", shared->count++);
    shared->shop_closed = 1;
    sem_post(&shared->mutex);

    for(int i = 0; i < NE; i++){
        sem_post(&shared->elfHelp_sem);
        sem_post(&shared->elf_sem);
    }

    for(int i = 0; i < NR; i++){
        sem_post(&shared->reindHitch_sem);
    }
    for(int i = 0; i < NR; i++){
        sem_wait(&shared->reind_sem);
    }

    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: Christmas started\n", shared->count++);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    return 0;
}