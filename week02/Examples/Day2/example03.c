#include <stdio.h>

int main(){
    unsigned char byte = 0x7e; // 0111 1110
    unsigned short word = 0x54f2; // 0101 0100 1111 0010

    int byte_bit_count = 0; // 6
    int word_bit_count = 0; // 8

    unsigned char bmask = 1; // 0000 0001
    unsigned char wmask = 1; // 0000 0000 0000 0001

    for(int i = 0; i < 8; i++){
        if(byte & (bmask << i)){
            byte_bit_count++;
        }
    }

    printf("byte_bit_count: %d\n", byte_bit_count);

    for(int i = 0; i < 16; i++){
        if(word & (wmask << i)){
            word_bit_count++;
        }
    }

    printf("word_bit_count: %d\n", word_bit_count);

    return 0; //exit status | termination status
}