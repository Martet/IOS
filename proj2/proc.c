#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#include "res.h"

int elf(sharedRes_t *shared, const int id, const int ET){
    srand(time(NULL) + id + 4321);

    sem_wait(&shared->mutex);
    print_log("%d: Elf %d: started\n", shared->count++, id);
    sem_post(&shared->mutex);

    while(1){ //elf loop
        usleep((rand() % (ET + 1)) * 1000); //do work

        sem_wait(&shared->mutex);
        print_log("%d: Elf %d: need help\n", shared->count++, id);
        sem_post(&shared->mutex);

        sem_wait(&shared->elf_sem); //wait to be only elf

        if(shared->shop_closed) //check if workshop isnt closed, else leave loop
            break;

        sem_wait(&shared->mutex);
        shared->elves++;
        if(shared->elves == 3) //if third elf in line, wake up Santa
            sem_post(&shared->santa_sem);
        else //else signal another elf
            sem_post(&shared->elf_sem); 
        sem_post(&shared->mutex);

        sem_wait(&shared->elfHelp_sem); //wait until santa starts helping

        if(shared->shop_closed) //check if workshop isnt closed, else leave loop
            break;

        sem_wait(&shared->mutex);
        print_log("%d: Elf %d: get help\n", shared->count++, id);
        shared->elves--;
        if(shared->elves == 0){ //if last elf to get help
            sem_post(&shared->elfDone_sem); //signal to santa that all elves are helped
            sem_post(&shared->elf_sem); //signal to another elf
        }
        sem_post(&shared->mutex);
    }
    
    sem_wait(&shared->mutex);
    print_log("%d: Elf %d: taking holidays\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->elf_sem);
    sem_post(&shared->elfHelp_sem);
    sem_post(&shared->main_wait);
    return 0;
}

int reindeer(sharedRes_t *shared, const int id, const int NR, const int RT){
    srand(time(NULL) + id + 1234);

    sem_wait(&shared->mutex);
    print_log("%d: RD %d: rstarted\n", shared->count++, id);
    sem_post(&shared->mutex);

    usleep(((rand() % (RT / 2 + 1)) + RT / 2)  * 1000);
    //do work and return home
    sem_wait(&shared->mutex);
    print_log("%d: RD %d: return home\n", shared->count++, id);
    shared->reindeers++;
    if(shared->reindeers == NR){ //if last reindeer to return, wake up santa
        sem_post(&shared->santa_sem);
    }
    sem_post(&shared->mutex);

    sem_wait(&shared->reindHitch_sem); //wait for santa to start hitching

    sem_wait(&shared->mutex);
    print_log("%d: RD %d: get hitched\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->reind_sem); //let santa know that succesfully hitched 
    sem_post(&shared->main_wait);
    return 0;
}

int santa(sharedRes_t *shared, const int NR){
    sem_wait(&shared->mutex);
    print_log("%d: Santa: going to sleep\n", shared->count++);
    sem_post(&shared->mutex);

    while(1){ //santa loop
        sem_wait(&shared->santa_sem); //get woken up by elf or reindeer
        if(shared->reindeers == NR) //if all reindeers returned, stop helping and hitch them
            break;
        else if(shared->elves == 3){ //if three elves waiting, help them
            sem_wait(&shared->mutex);
            print_log("%d: Santa: helping elves\n", shared->count++);
            sem_post(&shared->mutex);

            for(int i = 0; i < 3; i++) //let elves know they got helped
                sem_post(&shared->elfHelp_sem);

            sem_wait(&shared->elfDone_sem); //wait until are elves are out of workshop
            sem_wait(&shared->mutex);
            print_log("%d: Santa: going to sleep\n", shared->count++);
            sem_post(&shared->mutex);
        }
    }

    sem_wait(&shared->mutex);
    print_log("%d: Santa: closing workshop\n", shared->count++);
    shared->shop_closed = 1; //close workshop
    sem_post(&shared->mutex);

    sem_post(&shared->elf_sem);
    sem_post(&shared->elfHelp_sem);

    for(int i = 0; i < NR; i++){ //let all reindeers know to get hitched
        sem_post(&shared->reindHitch_sem);
    }
    for(int i = 0; i < NR; i++){ //wait for all reindeers to get hitched
        sem_wait(&shared->reind_sem);
    }

    sem_wait(&shared->mutex);
    print_log("%d: Santa: Christmas started\n", shared->count++);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    return 0;
}