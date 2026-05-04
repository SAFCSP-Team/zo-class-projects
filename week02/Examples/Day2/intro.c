#include <stdio.h>

// char sum(int a, int b); // prototype | function header | interface | sig

typedef struct location{
    float lng;
    float lat;
    char type; 
}location;

// Google Map API 1.0 <--
// Google Map API 1.1

// name = value
location getCurrentLocation(float lng, float lat, char type){
    location loc;
    loc.lat = lat;
    loc.lng = lng;
    loc.type = type;
    return loc;
}

int sum(int value, int value2){
    return value + value2;
}

int sub(int value, int value2){
    int r = sum(4, 5);
    return value - value2;
}

int op1(int input) /* interface */
{
    /* implementation */
    /* statements */
    return 0;
}

int op2(int input) /* interface */
{
    /* implementation */
    /* statements */
    return 0;
}
int main(){
    int r = sum(100, 200);

    // 1. seq
    int x = 10;
    int y = x * 2;
    x++;
    y++;
    // 2. selection
    if(x > 10){
        x--;
        y = y + 2;
    }else{
        x += 20;
        y--;
    }

    switch(y){
        case 5:
        case 7:
          printf("y=5\n");
        break;

        case 10:
          printf("y=10\n");
          break;
        
        default:
           printf("y=?\n");
           break;
    }
    // 3. loop
    while(x < y){
        x++;
        printf("x\n");
        /* statements */
    }

    do{
        x = x + 1;
        x++;
        printf("x\n");
        /* statements */
    }while(x < y);
    
    for(/* part 1*/; /* part 2 */; /* part 3*/){

    }
    for(int i = 0; i < 10 ; i++){

    }

    // function: f(x)

    int result = sum(1, 2) + sum(5, 5) + sub(1, 4); // call
    x++;
    y++;
    
    return 0;
}
