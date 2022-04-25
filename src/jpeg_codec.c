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


#define DCT_SIZE 8
#define FIX_Q 11
static int s_factor[64];
static int s_factor_inited = 0;

const float AAN_DCT_FACTOR[DCT_SIZE] = {
    1.0f, 1.387039845f, 1.306562965f, 1.175875602f,
    1.0f, 0.785694958f, 0.541196100f, 0.275899379f,
};

int float_2_fixed(float v) {
    return (int)(v * (1 << FIX_Q));
}

void init_factor() {
    s_factor_inited = 1;
    for (int i = 0; i < DCT_SIZE; i++) {
        for (int j = 0; j < DCT_SIZE; j++) {
            float tmp = 1.0f / (AAN_DCT_FACTOR[i] * AAN_DCT_FACTOR[i] * 8); 
            s_factor[i*DCT_SIZE+j] = float_2_fixed(tmp);
        }
    }
}

int* get_factor_tab() {
    if (!s_factor_inited) {
        init_factor() ;
        s_factor_inited = 1;
    }
    return s_factor;
}

void fdct_2d_8x8(int *data8x8, int* ftab) {
    int tmp0, tmp1, tmp2, tmp3;
    int tmp4, tmp5, tmp6, tmp7;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5, z11, z13;
    int *dataptr;
    int ctr;

    /* Pass 1: process rows. */
    dataptr = data8x8;
    for (ctr = 0; ctr < DCT_SIZE; ctr++) {
        tmp0 = dataptr[0] + dataptr[7];
        tmp7 = dataptr[0] - dataptr[7];
        tmp1 = dataptr[1] + dataptr[6];
        tmp6 = dataptr[1] - dataptr[6];
        tmp2 = dataptr[2] + dataptr[5];
        tmp5 = dataptr[2] - dataptr[5];
        tmp3 = dataptr[3] + dataptr[4];
        tmp4 = dataptr[3] - dataptr[4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * float_2_fixed(0.707106781f) >> FIX_Q; /* c4 */
        dataptr[2] = tmp13 + z1;  /* phase 5 */
        dataptr[6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * float_2_fixed(0.382683433f) >> FIX_Q;  /* c6 */
        z2 = (float_2_fixed(0.541196100f) * tmp10 >> FIX_Q) + z5;     /* c2-c6 */
        z4 = (float_2_fixed(1.306562965f) * tmp12 >> FIX_Q) + z5;     /* c2+c6 */
        z3 = tmp11 * float_2_fixed(0.707106781f) >> FIX_Q;            /* c4 */

        z11 = tmp7 + z3;        /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[5] = z13 + z2;  /* phase 6 */
        dataptr[3] = z13 - z2;
        dataptr[1] = z11 + z4;
        dataptr[7] = z11 - z4;

        dataptr += DCT_SIZE;     /* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    dataptr = data8x8;
    for (ctr = 0; ctr < DCT_SIZE; ctr++) {
        tmp0 = dataptr[DCT_SIZE * 0] + dataptr[DCT_SIZE * 7];
        tmp7 = dataptr[DCT_SIZE * 0] - dataptr[DCT_SIZE * 7];
        tmp1 = dataptr[DCT_SIZE * 1] + dataptr[DCT_SIZE * 6];
        tmp6 = dataptr[DCT_SIZE * 1] - dataptr[DCT_SIZE * 6];
        tmp2 = dataptr[DCT_SIZE * 2] + dataptr[DCT_SIZE * 5];
        tmp5 = dataptr[DCT_SIZE * 2] - dataptr[DCT_SIZE * 5];
        tmp3 = dataptr[DCT_SIZE * 3] + dataptr[DCT_SIZE * 4];
        tmp4 = dataptr[DCT_SIZE * 3] - dataptr[DCT_SIZE * 4];

        /* Even part */
        tmp10 = tmp0 + tmp3;  /* phase 2 */
        tmp13 = tmp0 - tmp3;
        tmp11 = tmp1 + tmp2;
        tmp12 = tmp1 - tmp2;

        dataptr[DCT_SIZE * 0] = tmp10 + tmp11;  /* phase 3 */
        dataptr[DCT_SIZE * 4] = tmp10 - tmp11;

        z1 = (tmp12 + tmp13) * float_2_fixed(0.707106781f) >> FIX_Q; /* c4 */
        dataptr[DCT_SIZE * 2] = tmp13 + z1;  /* phase 5 */
        dataptr[DCT_SIZE * 6] = tmp13 - z1;

        /* Odd part */
        tmp10 = tmp4 + tmp5;  /* phase 2 */
        tmp11 = tmp5 + tmp6;
        tmp12 = tmp6 + tmp7;

        /* The rotator is modified from fig 4-8 to avoid extra negations. */
        z5 = (tmp10 - tmp12) * float_2_fixed(0.382683433f) >> FIX_Q;  /* c6 */
        z2 = (float_2_fixed(0.541196100f) * tmp10 >> FIX_Q) + z5;     /* c2-c6 */
        z4 = (float_2_fixed(1.306562965f) * tmp12 >> FIX_Q) + z5;     /* c2+c6 */
        z3 = tmp11 * float_2_fixed(0.707106781f) >> FIX_Q;            /* c4 */

        z11 = tmp7 + z3;  /* phase 5 */
        z13 = tmp7 - z3;

        dataptr[DCT_SIZE * 5] = z13 + z2; /* phase 6 */
        dataptr[DCT_SIZE * 3] = z13 - z2;
        dataptr[DCT_SIZE * 1] = z11 + z4;
        dataptr[DCT_SIZE * 7] = z11 - z4;

        dataptr++;  /* advance pointer to next column */
    }

    // ftab = ftab ? ftab : s_factor;
    if (ftab == NULL) {
        ftab = get_factor_tab();
    }
    for (ctr = 0; ctr < 64; ctr++) {
        data8x8[ctr] *= ftab[ctr];
        data8x8[ctr] >>= FIX_Q + 2;
    }
}

void blocks_fdct(const uint8 (*yblocks)[64], const uint8 (*ublocks)[64], const uint8 (*vblocks)[64],
    uint w, uint h, int (*ydct)[64], int (*udct)[64], int (*vdct)[64]) {
    
    int nwb = w / 8 + (w % 8 ? 1 : 0);
    int nhb = h / 8 + (h % 8 ? 1 : 0);
    int nb = nwb * nhb;

    for (int b = 0; b < nb; b++) {
        for (int i = 0; i < 64; i++) {
            ydct[b][i] = yblocks[b][i] - 128;
            udct[b][i] = ublocks[b][i] - 128;
            vdct[b][i] = vblocks[b][i] - 128;
            
            ydct[b][i] = ydct[b][i] << 2;
            udct[b][i] = udct[b][i] << 2;
            vdct[b][i] = vdct[b][i] << 2;
        }
        fdct_2d_8x8(ydct[b], NULL);
        fdct_2d_8x8(udct[b], NULL);
        fdct_2d_8x8(vdct[b], NULL);
    }
}

static int s_std_quant_tab_lumin[64] = {
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 36, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99,
};

static int s_std_quant_tab_chrom[64] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
};

void quant_encode8x8(int *data8x8, bool luminance) {
    for (int i = 0; i < 64; i++) {
        if (luminance) {
            data8x8[i] /= s_std_quant_tab_lumin[i];
        } else {
            data8x8[i] /= s_std_quant_tab_chrom[i];
        }
    }
}

void fdct_2_quant(int (*ydct)[64], int (*udct)[64], int (*vdct)[64], int w, int h) {

    int nwb = w / 8 + (w % 8 ? 1 : 0);
    int nhb = h / 8 + (h % 8 ? 1 : 0);
    int nb = nwb * nhb;
    
    for (int i = 0; i < nb; i++) {
        quant_encode8x8(ydct[i], 1);
        quant_encode8x8(udct[i], 0);
        quant_encode8x8(vdct[i], 0);
    }    
}

static int s_zigzag_index_8x8[] = {
    0,  1,  8,  16, 9,  2,  3,  10,
    17, 24, 32, 25, 18, 17, 4,  5, 
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6,  7,  14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 
    29, 22, 15, 23, 30, 37, 44, 51, 
    58, 59, 52, 45, 38, 31, 39, 46, 
    53, 60, 61, 54, 47, 55, 62, 63,
};

void jpeg_zigzag(int* data8x8) {
    int tmp[64];
   
    for (int i = 0; i < 64; i++) {
        tmp[i] = data8x8[s_zigzag_index_8x8[i]];
    }
    for (int i = 0; i < 64; i++) {
        data8x8[i] = tmp[i];
    }
}

void quant_2_zigzag(int (*yquant)[64], int (*uquant)[64], int (*vquant)[64], uint w, uint h) {
    int nwb = w / 8 + (w%8 ? 1 : 0);
    int nhb = h / 8 + (h%8 ? 1 : 0);
    int nb = nwb * nhb;

    for (int i = 0; i < nb; i++) {
        jpeg_zigzag(yquant[i]);
        jpeg_zigzag(uquant[i]);
        jpeg_zigzag(vquant[i]);
    }
}
