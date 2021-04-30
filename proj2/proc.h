#ifndef __PROC_H__
#define __PROC_H__

#include "res.h"

//elf code
int elf(sharedRes_t *shared, const int id, const int ET);

//reindeer code
int reindeer(sharedRes_t *shared, const int id, const int NR, const int RT);

//santa code
int santa(sharedRes_t *shared, const int NR, const int NE);

#endif
