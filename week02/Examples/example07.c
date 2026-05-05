#include <stdio.h>

unsigned int byte_pack(unsigned char a,
                       unsigned char b,
                       unsigned char c,
                       unsigned char d)
{
    unsigned int result = 0;
    /* pack */
    return result;
}

int compare_nybble(unsigned char value){
    /*
       low nybble == high nybble -> 0
       low nybble > high nybble -> 1
       low nybble < high nybble -> -1
    */

    return 0;
}

int is_odd(unsigned char value){
    return value & 1;
}

int main(){
    unsigned char value = 99;
    int result = is_odd(value);
    if(result){
        printf("ODD\n");
    }else{
        printf("EVEN\n");
    }

    /* compare nybble */
    result = compare_nybble(0x33);
    result = compare_nybble(0xf2);
    result = compare_nybble(0xc8);

    /* pack */
    unsigned int result = byte_pack(25, 16, 33, 22);
    // print
    return 0;
}