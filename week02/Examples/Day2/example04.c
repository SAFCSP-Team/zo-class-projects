#include <stdio.h>

int main(int argc, char **argv){
    if(argc != 2){
        printf("usage: %s value\n", argv[0]);
        return 0;
    }

    printf("language: %s\n", argv[1]);
    return 0;
}