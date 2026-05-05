
#include<stdio.h>
#define NYBBLE  4

#define BEGIN_FOR(a, b, c)    for(a; b; c){
#define END_FOR }

#define IFSTMT(expr, v)       \
  if(expr){                   \
    int result = 10;          \
    printf("%d\n", result);   \
  }


int main(){
    unsigned short value = (unsigned short)(0xE21C << NYBBLE); // 0x21C0
    IFSTMT(value > 0, 10)

    BEGIN_FOR(int i = 0, i < 10, i++)
     printf("%d\n", i);
    END_FOR

    return 0;
}