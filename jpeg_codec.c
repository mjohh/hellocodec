#include<stdio.h>
#include<math.h>

typedef unsigned char uint8;


uint8 bound(int v, uint8 min, uint8 max){
    if (v < min)
        return min;
    if (v > max)
        return max;
    return v;
}

#define BOUND(v, min, max)\
    (v)<=(min) ? (min) : ((v)>=(max) ? (max) :(v))


void rgb_to_yuv(uint8 r, uint8 g, uint8 b, uint8* y, uint8* u, uint8* v) {
    *y = round(0.299*r + 0.587*g + 0.114*b);
    *u = round(-0.169*r - 0.331*g + 0.500*b + 128);
    *v = round(0.500*r - 0.419*g - 0.081*b + 128);
    *y = BOUND(*y, 0, 255);
    *u = BOUND(*u, 0, 255);
    *v = BOUND(*v, 0, 255);
}


int main() {
    uint8 v = BOUND(-1, 0, 2);
    printf("v=%d", v);

    v = BOUND(1, 0, 2);
    printf("v=%d", v);

    v = BOUND(3, 0, 2);
    printf("v=%d", v);

    do{
    uint8 y=0;
    uint8 u=0;
    uint8 v=0;
    rgb_to_yuv(255, 255, 255, &y, &u, &v);
    printf("\ny,u,v=%d,%d,%d", y, u, v);
    
    //rgb_to_yuv(256, 355, 255, &y, &u, &v);
    //printf("\ny,u,v=%d,%d,%d", y, u, v);
    }while(0);
    return 0;
}

