#include <stdio.h>
#include "def.h"

/*
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
*/


extern void test_yuv_to_blocks();

int main() {
    test_rgb_yuv();
    test_load_rgb();
    test_yuv_to_blocks();
}

