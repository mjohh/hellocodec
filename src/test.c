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
void bittest() {
    char a = 5;
    char b = -5;
    char c = ~(b-1);
    
    int sz = 0;
    for (int i=7; i>=0; i--) {
        if (1<<i & a){
            sz = i+1;
            break;
        }
    }
    int mask = 0;
    for (int i=0; i<sz; i++) {
        mask += 1<<i;
    }
    int d = (b - 1) & mask;
    printf("sz of 5 is %d\n", sz);
    printf("%d,%d\n", a, b);  
    printf("%d\n", c); 
    printf("%d\n", d);
}

extern void test_yuv_to_blocks();
extern void test_yuv_to_blocks8x8();
extern void test_yuv_to_blocks8x8_2();
extern void test_codec_pipeline();
extern void test_vli_encode();
extern void test_ac_rle_encode();
extern void test_tbl();

int main() {
    // test_rgb_yuv();
    // test_load_rgb();
    // test_yuv_to_blocks();
    // test_yuv_to_blocks8x8();
    // test_yuv_to_blocks8x8_2();
    // test_codec_pipeline();
    // bittest();
    // test_vli_encode();
    // test_ac_rle_encode();
    test_tbl();
}

