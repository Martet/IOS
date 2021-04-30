/*
 * Author: Martin Zmitko (xzmitk01)
 * Date: 26-04-2021
 * Description: Second IOS project, process synchronization using unix semaphores
*/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include "proc.h"
#include "res.h"

//free everything
void freeRes(sharedRes_t *shared){
    fclose(shared->file);
    sem_destroy(&shared->mutex);
    sem_destroy(&shared->main_wait);
    sem_destroy(&shared->santa_sem);
    sem_destroy(&shared->reind_sem);
    sem_destroy(&shared->elf_sem);
    sem_destroy(&shared->elfHelp_sem);
    sem_destroy(&shared->elfDone_sem);
    sem_destroy(&shared->reindHitch_sem);
    munmap(shared, sizeof(sharedRes_t));
}

//kill all forked processes
void killAll(pid_t *pid_arr, const unsigned int arr_pos){
    for(unsigned int i = 0; i < arr_pos; i++)
        kill(pid_arr[i], SIGKILL);

    free(pid_arr);
}

//parses string in arg as number between min and max (included), returns the number, -1 on error
int parseArg(char *arg, const unsigned int min, const unsigned int max){
    char *endPtr;
    long out = strtol(arg, &endPtr, 10);
    if(strcmp(endPtr, "") || out < min || out > max)
        return -1;
    return out;
}

int main(int argc, char* argv[]){
    if(argc != 5){ //must have 4 arguments
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }
    int NE = parseArg(argv[1], 1, 999);
    int NR = parseArg(argv[2], 1, 19);
    int TE = parseArg(argv[3], 0, 1000);
    int TR = parseArg(argv[4], 0, 1000);
    if(NE == -1 || NR == -1 || TE == -1 || TR == -1){   //all arguments must be in correct range
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }
    
    //map shared memory
    sharedRes_t *shared = mmap(NULL, sizeof(sharedRes_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(shared == MAP_FAILED){
        fprintf(stderr, "Failed to map shared memory\n");
        return 1;
    }

    //open output file
    shared->file = fopen("proj2.out", "w");
    if(shared->file == NULL){
        fprintf(stderr, "Error opening file\n");
        munmap(shared, sizeof(sharedRes_t));
        return 1;
    }
    
    //initialize shared variables and semaphores
    shared->count = 1;
    shared->reindeers = 0;
    shared->elves = 0;
    shared->shop_closed = 0;
    sem_init(&shared->mutex, 1, 1);
    sem_init(&shared->main_wait, 1, 0);
    sem_init(&shared->santa_sem, 1, 0);
    sem_init(&shared->reind_sem, 1, 0);
    sem_init(&shared->elf_sem, 1, 1);
    sem_init(&shared->elfHelp_sem, 1, 0);
    sem_init(&shared->elfDone_sem, 1, 0);
    sem_init(&shared->reindHitch_sem, 1, 0);

    //initialize pid array
    unsigned int arr_pos = 0;
    pid_t *pid_arr = malloc(sizeof(pid_t) * (1 + NE + NR));
    if(pid_arr == NULL){
        fprintf(stderr, "Memory allocation error\n");
        freeRes(shared);
        return 1;
    }
 
    //fork santa
    pid_arr[arr_pos++] = fork();
    if(pid_arr[arr_pos - 1] < 0){
        fprintf(stderr, "Forking error\n");
        free(pid_arr);
        freeRes(shared);
        return 1;
    } 
    else if(pid_arr[arr_pos - 1] == 0){
        free(pid_arr);
        return santa(shared, NR, NE); //run santa
    }
    
    for(int i = 1; i <= NE; i++){
        pid_arr[arr_pos] = fork(); //fork elves
        if(pid_arr[arr_pos] < 0){ //check if forked correctly
            fprintf(stderr, "Forking error\n");
            killAll(pid_arr, arr_pos);
            freeRes(shared);
            return 1;
        }
        else if(pid_arr[arr_pos] == 0){
            free(pid_arr);
            return elf(shared, i, TE); //run elf
        }
        arr_pos++;
    }
    for(int i = 1; i <= NR; i++){
        pid_arr[arr_pos] = fork(); //fork reindeers
        if(pid_arr[arr_pos] < 0){ //check if forked correctly
            fprintf(stderr, "Forking error\n");
            killAll(pid_arr, arr_pos);
            freeRes(shared);
            return 1;
        }
        else if(pid_arr[arr_pos] == 0){
            free(pid_arr);
            return reindeer(shared, i, NR, TR); //run reindeer
        }
        arr_pos++;
    }
    
    
    //wait for all proccesses to finish
    for(int i = 0; i < 1 + NE + NR; i++)
        sem_wait(&shared->main_wait);
    
    free(pid_arr);
    freeRes(shared); //free everything and exit main process
    return 0;
}
