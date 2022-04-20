#include<stdio.h>
#include<math.h>
#include<assert.h>
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
    
    int nwblock = w/8 + (w%8 ? 1 : 0);
    int nhblock = h/8 + (h%8 ? 1 : 0);
    
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


void yuv_to_blocks8x8(uint8* y, uint8* u, uint8* v, int w, int h, 
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64], int nwblocks, int nhblocks) {
    
    int nw = w/8 + (w%8 ? 1 : 0);
    int nh = h/8 + (h%8 ? 1 : 0);
    assert(nw == nwblocks);
    assert(nh == nhblocks);
 
    int ww = nwblocks * 8;
    int hh = nhblocks * 8;
    uint8* pyblocks = (uint8*)yblocks;
    uint8* publocks = (uint8*)ublocks;
    uint8* pvblocks = (uint8*)vblocks;
    

    for (int r = 0; r < hh; r++) {
        for (int c = 0; c < ww; c++) {
            int i = r * w + c;
            int ii = r * ww + c;
            if (r < h && c < w) {
                pyblocks[ii] = y[i];
                publocks[ii] = u[i];
                pvblocks[ii] = v[i];
            } else {
                pyblocks[ii] = 0;
                publocks[ii] = 0;
                pvblocks[ii] = 0;  
            }
        }
    }
}


void yuv_to_blocks8x8_2(uint8* y, uint8* u, uint8* v, int w, int h, 
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64]) {

    int ww = w / 8 + (w % 8 ? 1 : 0);
    int hh = h / 8 + (h % 8 ? 1 : 0);

    for (int rowb = 0; rowb < hh; rowb++) {
        for (int colb = 0; colb < ww; colb++) {
            // index of block in yblocks
            int ib =  rowb * ww + colb;

            // visit within block
            for (int r = 0;  r < 8; r++) {
                for (int c = 0; c < 8; c++) {

                    // map to idx of extended uint8 matrix
                    int i = rowb * 8 + r;
                    int j = colb * 8 + c;
                    if (i < h && j < w) {
                        yblocks[ib][r*8+c] = y[i*w+j];
                        ublocks[ib][r*8+c] = u[i*w+j];
                        vblocks[ib][r*8+c] = v[i*w+j];
                    } else {
                        yblocks[ib][r*8+c] = 0;
                        ublocks[ib][r*8+c] = 0;
                        vblocks[ib][r*8+c] = 0;
                    }
                }
            }
 
        }
    }
    
}
