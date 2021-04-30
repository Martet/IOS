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
#include <time.h>
#include "proc.h"
#include "res.h"

pid_t *pid_arr; //global array of processes for signal handler
unsigned int arr_pos;
sharedRes_t *shared; //global shared memory
int NE, TE; //handler also needs number and wait time of elves

//free everything
void freeRes(){
    fclose(shared->file);
    sem_destroy(&shared->mutex);
    sem_destroy(&shared->main_wait);
    sem_destroy(&shared->santa_sem);
    sem_destroy(&shared->reind_sem);
    sem_destroy(&shared->elf_sem);
    sem_destroy(&shared->elfHelp_sem);
    sem_destroy(&shared->elfDone_sem);
    sem_destroy(&shared->reindHitch_sem);
    sem_destroy(&shared->santaDone_sem);
    munmap(shared, sizeof(sharedRes_t));
}

//kill all forked processes
void killAll(){
    for(unsigned int i = 0; i < arr_pos; i++)
        kill(pid_arr[i], SIGKILL);

    free(pid_arr);
}

//SIGQUIT signal handler
void quit_handler(int sig){
    (void)sig;
    killAll(pid_arr, arr_pos);
    freeRes();
    exit(0);
}

//SIGUSR1 signal handler
void usr_handler(int sig){
    (void)sig;
    int new_NE = (rand() % NE) + 1;
    pid_t *new_pid_arr = realloc(pid_arr, sizeof(pid_t) * (arr_pos + new_NE));
    if(new_pid_arr == NULL){
        fprintf(stderr, "Memory allocation error\n");
        killAll();
        freeRes();
        exit(1);
    }
    pid_arr = new_pid_arr;

    for(int i = 1; i <= new_NE; i++){
        pid_arr[arr_pos] = fork(); //fork elves
        if(pid_arr[arr_pos] < 0){ //check if forked correctly
            fprintf(stderr, "Forking error\n");
            killAll();
            freeRes();
            exit(1);
        }
        else if(pid_arr[arr_pos] == 0){
            free(pid_arr);
            elf(shared, i, TE); //run elf
            exit(0);
        }
        arr_pos++;
    }
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
    if(argc != 5 && argc != 6){ //must have 4 or 5 arguments
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }

    char flag = strcmp(argv[1], "-b") ? 0 : 1;
    if(flag && argc != 6){
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }

    NE = parseArg(argv[1 + flag], 1, 999);
    int NR = parseArg(argv[2 + flag], 1, 19);
    TE = parseArg(argv[3 + flag], 0, 1000);
    int TR = parseArg(argv[4 + flag], 0, 1000);
    if(NE == -1 || NR == -1 || TE == -1 || TR == -1){   //all arguments must be in correct range
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }
    
    //map shared memory
    shared = mmap(NULL, sizeof(sharedRes_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
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
    sem_init(&shared->santaDone_sem, 1, 0);

    //initialize pid array
    arr_pos = 0;
    pid_arr = malloc(sizeof(pid_t) * (1 + NE + NR));
    if(pid_arr == NULL){
        fprintf(stderr, "Memory allocation error\n");
        freeRes();
        return 1;
    }

    signal(SIGQUIT, quit_handler);
 
    //fork santa
    pid_arr[arr_pos++] = fork();
    if(pid_arr[arr_pos - 1] < 0){
        fprintf(stderr, "Forking error\n");
        free(pid_arr);
        freeRes();
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
            killAll();
            freeRes();
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
            killAll();
            freeRes();
            return 1;
        }
        else if(pid_arr[arr_pos] == 0){
            free(pid_arr);
            return reindeer(shared, i, NR, TR); //run reindeer
        }
        arr_pos++;
    }
    
    if(flag){ //if flag is set, handle SIGUSR1 and wait for signal from santa
        srand(time(NULL) + 666);
        signal(SIGUSR1, usr_handler);
        sem_wait(&shared->santaDone_sem);
    }
    
    //wait for all proccesses to finish
    for(int i = 0; i < 1 + NE + NR; i++)
        sem_wait(&shared->main_wait);
    
    free(pid_arr);
    freeRes(); //free everything and exit main process
    return 0;
}
