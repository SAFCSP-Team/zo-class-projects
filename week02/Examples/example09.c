
#include<stdio.h>

#define BYTE_SHL(value, o)   ((value) << (o))
#define BYTE_SHR(value, o)   ((value) >> (o))

#define BYTE_SH(value, l, r)  \
    BYTE_SHL(value, l);       \
    BYTE_SHR(value, r);

#define BYTE_SWAP_NYBBLE(value)
/*
   implementation
*/

int main(){
    unsigned char v = 0x2E;
    BYTE_SWAP_NYBBLE(v)
    printf("%x\n", v);
    BYTE_SH(v, 3, 2)
    return 0;
}