#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "proc.h"
#include "res.h"

//parses string in arg as number between min and max (included), returns the number, -1 on error
int parseArg(char *arg, unsigned int min, unsigned int max){
    char *endPtr;
    long out = strtol(arg, &endPtr, 10);
    if(strcmp(endPtr, "") || out < min || out > max)
        return -1;
    return out;
}

int main(int argc, char* argv[]){
    if(argc != 5){
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }
    int NE = parseArg(argv[1], 1, 999);
    int NR = parseArg(argv[2], 1, 19);
    int TE = parseArg(argv[3], 0, 1000);
    int TR = parseArg(argv[4], 0, 1000);
    if(NE == -1 || NR == -1 || TE == -1 || TR == -1){
        fprintf(stderr, "Argument parsing error\n");
        return 1;
    }
    
    
    sharedRes_t *shared = mmap(NULL, sizeof(sharedRes_t), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(shared == MAP_FAILED){
        fprintf(stderr, "Failed to map shared memory\n");
        return 1;
    }

    shared->file = fopen("proj2.out", "w");
    if(shared->file == NULL){
        fprintf(stderr, "Error opening file\n");
        munmap(shared, sizeof(sharedRes_t));
        return 1;
    }
    
    shared->count = 1;
    shared->reindeers = 0;
    shared->elves = 0;
    shared->shop_closed = 0;
    shared->NR = NR;
    sem_init(&shared->mutex, 1, 1);
    sem_init(&shared->main_wait, 1, 0);
    sem_init(&shared->santa_sem, 1, 0);
    sem_init(&shared->reind_sem, 1, 0);
    sem_init(&shared->elf_sem, 1, 0);

    pid_t id = fork();
    if(id < 0){
        fprintf(stderr, "Forking error\n");
        return 1;
    } 
    else if(id == 0){
        return santa(shared);
    }
    else{
        for(int i = 1; i <= NE; i++){
            pid_t pid = fork();
            if(pid < 0){
                fprintf(stderr, "Forking error\n");
                return 1;
            }
            else if(pid == 0){
                return elf(shared, i, TE);
            }
                
        }
        for(int i = 1; i <= NR; i++){
            pid_t pid = fork();
            if(pid < 0){
                fprintf(stderr, "Forking error\n");
                return 1;
            }
            else if(pid == 0){
                return reindeer(shared, i, TR);
            }
        }
    }

    for(int i = 0; i < 1 + NE + NR; i++)
        sem_wait(&shared->main_wait);
    printf("%d: Kak\n", shared->count++);

    fclose(shared->file);
    sem_destroy(&shared->mutex);
    sem_destroy(&shared->main_wait);
    sem_destroy(&shared->santa_sem);
    munmap(shared, sizeof(sharedRes_t));

    return 0;
}
