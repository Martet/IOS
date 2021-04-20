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

    usleep((rand() % (ET + 1)) * 1000);

    sem_wait(&shared->mutex);
    print_log(shared, "%d: Elf %d: taking holidays\n", shared->count++, id);
    sem_post(&shared->mutex);

    sem_post(&shared->main_wait);
    exit(0);
}
