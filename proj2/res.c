#include <semaphore.h>
#include <stdarg.h>
#include <stdio.h>
#include "res.h"

//prints a formatted string to the file in shared->file, must be run inside mutex
void print_log(sharedRes_t *shared, const char* fmt, ...){
    va_list args;
    va_start(args, fmt);
 
    vfprintf(shared->file, fmt, args);
    fflush(shared->file);
 
    va_end(args);
}
