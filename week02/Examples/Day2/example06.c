#include <stdio.h>

int main(){
    // ASCII CODE
    unsigned char value = 't';

    // t = 116   0111 0100
    // r = 114   0111 0010
    value = value & 0xf0; // 0111 0000
    value = value | 0x02; // 0111 0010

    value = value & 0xDf; // 1101 1111
    // 01010010 R -> 82
    
    printf("value: %c, %d\n", value, value);
    return 0;
}