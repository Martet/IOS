#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "res.h"

int santa(sharedRes_t *shared){
    sem_wait(shared->mutex);
    printf("%d: Santa: going to sleep\n", shared->count);
    shared->count++;
    sem_post(shared->mutex);
    exit(0);
}
