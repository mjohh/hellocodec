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

// *level = [0~15]
void vli_encode(int val, uint8* level, int* code) {
    assert(val>=-32767 && val<=32767);

    int l = 0;
    int abs = (val>0 ? val : 0-val);
    for (int i = 15; i>=0; i--) {
        if (abs & (1<<i)) {
            l = i + 1;
            break;
        }
    }
    if (val >= 0) {
        *code = val;
    } else {
        // short mask = 0;
        // for (int i = 0; i < l; i++) {
        //    mask += (1<<i);
        // }
        // *code = (val-1) & mask;
        *code = (1 << l) - abs -1;
    }
    *level = (uint8) l;
}

// for first block64, *dc==0
void dc_dpcm_encode(int* block64, int* dc, int* code) {
    *code = block64[0] - *dc;
    *dc = block64[0];
}


void ac_rle_encode(int* block64, struct rle_data* rle64) {
    int cnt = 0;
    int j = 0;
    // begin with ac data
    for (int i = 1; i < 64; i++) {
        if (block64[i] == 0) {
            cnt++;
            continue;
        } 
        while (cnt > 15) {
            rle64[j].run_length = 15;
            rle64[j].data = 0;
            j++;
            cnt -= 15;
        }
        if (cnt >= 0) {
            rle64[j].run_length = cnt;
            rle64[j].data = block64[i];
            j++;
            cnt = 0;
        }
    }
    if (cnt > 0) {
        rle64[j].run_length = 0;
        rle64[j].data = 0;
    }
}

const uint8 std_huftab_lumin_ac[] = {
        0x00, 0x02, 0x01, 0x03, 0x03, 0x02, 0x04, 0x03, 0x05, 0x05, 0x04, 0x04, 0x00, 0x00, 0x01, 0x7d,
        0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12, 0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
        0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08, 0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
        0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
        0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
        0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
        0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
        0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
        0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
        0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
        0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa,
};
const uint8 std_huftab_lumin_dc[] = {
        0x00, 0x01, 0x05, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
};
const uint8 std_huftab_chrom_ac[] = {
        0x00, 0x02, 0x01, 0x02, 0x04, 0x04, 0x03, 0x04, 0x07, 0x05, 0x04, 0x04, 0x00, 0x01, 0x02, 0x77,
        0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21, 0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
        0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91, 0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
        0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34, 0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
        0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
        0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
        0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
        0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
        0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
        0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
        0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
        0xf9, 0xfa
};
const uint8 std_huftab_chrom_dc[] = {
        0x00, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
};

