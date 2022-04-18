#include<stdio.h>
#include<math.h>
#include "def.h"

//typedef unsigned char uint8;


uint8 bound(int v, uint8 min, uint8 max){
    if (v < min)
        return min;
    if (v > max)
        return max;
    return v;
}

//#define BOUND(v, min, max)\
//    (v)<=(min) ? (min) : ((v)>=(max) ? (max) :(v))


void _rgb_to_yuv(uint8 r, uint8 g, uint8 b, uint8* y, uint8* u, uint8* v) {
    *y = round(0.299*r + 0.587*g + 0.114*b);
    *u = round(-0.169*r - 0.331*g + 0.500*b + 128);
    *v = round(0.500*r - 0.419*g - 0.081*b + 128);
    *y = BOUND(*y, 0, 255);
    *u = BOUND(*u, 0, 255);
    *v = BOUND(*v, 0, 255);
}

void rgb_to_yuv(uint8* r, uint8* g, uint8* b, uint8* y, uint8* u, uint8* v, int w, int h) {
    for (int row=0; row<h; row++) {
        for (int col=0; col<w; col++) {
            int i = row*w + col;
            _rgb_to_yuv(r[i], g[i], b[i], &y[i], &u[i], &v[i]);
        }
    }
}

void yuv_to_blocks(uint8* y, uint8* u, uint8* v, int w, int h, 
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64]) {
    
    int nwblock = w/8 + (w%8==0 ? 0 : 1);
    int nhblock = h/8 + (h%8==0 ? 0 : 1);
    
    for (int blockrow = 0; blockrow < nhblock; blockrow++) {
        for (int blockcol = 0; blockcol < nwblock; blockcol++) {
            int blockidx = blockrow * nwblock + blockcol;
            uint8* yblock = yblocks[blockidx];
            uint8* ublock = ublocks[blockidx];
            uint8* vblock = vblocks[blockidx];

            for (int row = 0; row < 8; row++) {
                for (int col = 0; col < 8; col++) {
                    int idxinblock = row * 8 + col;
                    int realrow = blockrow * 8 + row;
                    int realcol = blockcol * 8 + col;
                    int realidx = realrow * w + realcol;

                    if (realrow > h || realcol > w) {
                        yblock[idxinblock] = 0;
                        ublock[idxinblock] = 0;
                        vblock[idxinblock] = 0;
                    } else {
                        yblock[idxinblock] = y[realidx];
                        ublock[idxinblock] = u[realidx];
                        vblock[idxinblock] = v[realidx];
                    }
                }
            }
        }
    }
}

void test_rgb_yuv() {
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
    _rgb_to_yuv(255, 255, 255, &y, &u, &v);
    printf("\ny,u,v=%d,%d,%d", y, u, v);
    
    //_rgb_to_yuv(256, 355, 255, &y, &u, &v);
    //printf("\ny,u,v=%d,%d,%d", y, u, v);
    }while(0);
}

void test_yuv_to_blocks() {
    uint8 y[64];
    int w = 8;
    int h = 8;
    for (int i = 0; i < 64; i++) {
        y[i] = i;
    }

    uint8 yblocks[64];
    uint8 ublocks[64];
    uint8 vblocks[64];

    yuv_to_blocks(y, y, y, w, h, &yblocks, &ublocks, &vblocks);
    
    for (int i = 0; i < 64; i++) {
        printf("%d\n", yblocks[i]);
    }
    printf("-------------------\n");
    for (int i = 0; i < 64; i++) {
        printf("%d\n", ublocks[i]);
    }
    printf("-------------------\n");
    for (int i = 0; i < 64; i++) {
        printf("%d\n", vblocks[i]);
    }
    
}
