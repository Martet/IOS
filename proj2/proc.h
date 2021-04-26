#ifndef __RES_H_
#define __RES_H_

#include "res.h"

//elf code
int elf(sharedRes_t *shared, int id, int ET);

//reindeer code
int reindeer(sharedRes_t *shared, int id, int NR, int RT);

//santa code
int santa(sharedRes_t *shared, int NR, int NE);

#endif
