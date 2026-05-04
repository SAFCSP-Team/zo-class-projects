#include <stdio.h>

int main(){
    unsigned char byte = 0x00; // 0000 0000
    unsigned short word = 0x0000; // 0000 0000 0000 0000
    unsigned int dword = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000

    unsigned char bmask = 0x20; // 0010 0000
    byte =  byte | bmask;
    byte = byte | 0x09; // 0000 1001 -> 0010 1001 -> 0x29
    byte = byte ^ 0x30; // 0011 0000 -> 0001 1001 -> 0x19
    byte = byte & 0x7e; // 0111 1110 -> 0001 1000 -> 0x18

    // word = 0x1818
    word = byte; // 0x0018
    word = word << 8; // 0x1800;
    word = word | byte; // 0x1818

    // dword = 0x 18 00 00 18
    dword = byte; // 0x00000018
    dword = dword | 0x18000000; // 0x18000018;




    return 0; //exit status | termination status
}
