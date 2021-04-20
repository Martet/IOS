#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include "res.h"

int santa(sharedRes_t *shared){
    sem_wait(&shared->mutex);
    print_log(shared, "%d: Santa: going to sleep\n", shared->count++);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    exit(0);
}
