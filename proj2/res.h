#include <semaphore.h>

#ifndef _RES_DEF
#define _RES_DEF

typedef struct sharedRes{
    sem_t *mutex;
    unsigned count;
} sharedRes_t;

#endif
