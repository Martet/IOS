#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <semaphore.h>

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
    

    return 0;
}
