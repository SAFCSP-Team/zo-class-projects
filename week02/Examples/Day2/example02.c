#include <stdio.h>

int main(){
    unsigned char byte = 0x4e; // 0100 1110
    unsigned short word = 0x3cfa; // 0011 1100 1111 1010
    unsigned int dword = 0x00000000; // 0000 0000 0000 0000 0000 0000 0000 0000

    // dword = 0x043cafe0

    // byte
    dword = dword | byte; // 0x0000004e
    dword = dword << 20;  // 0x04e00000
    dword = dword & 0x04000000; // 0x04000000
    dword = dword | ((byte & 0x0f)<< 4); // 0x040000e0

    // word            4 + (3 * 5) - 1         
    dword = dword | ((word & 0xff00) << 8); // 0x043c00e0
    //...


    return 0; //exit status | termination status
}