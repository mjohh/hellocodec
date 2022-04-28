#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<assert.h>
#include "def.h"

//typedef unsigned char uint8;

extern void _rgb_to_yuv(uint8 r, uint8 g, uint8 b, uint8* y, uint8* u, uint8* v);
extern void yuv_to_blocks8x8(uint8* y, uint8* u, uint8* v, int w, int h,
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64], int nwblocks, int nhblocks);
extern void yuv_to_blocks(uint8* y, uint8* u, uint8* v, int w, int h,
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64]);
extern void yuv_to_blocks8x8_2(uint8* y, uint8* u, uint8* v, int w, int h,
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64]);
extern int load_rgb(const char* rgb_file, uint8** r, uint8** g, uint8** b, int* w, int* h); 
extern void free_rgb(uint8* rgb);
extern void rgb_to_yuv(uint8* r, uint8* g, uint8* b, uint8* y, uint8* u, uint8* v, int w, int h);
extern void quant_2_zigzag(int (*yquant)[64], int (*uquant)[64], int (*vquant)[64], uint w, uint h);
extern void blocks_fdct(const uint8 (*yblocks)[64], const uint8 (*ublocks)[64], const uint8 (*vblocks)[64],
    uint w, uint h, int (*ydct)[64], int (*udct)[64], int (*vdct)[64]);
extern void fdct_2_quant(int (*ydct)[64], int (*udct)[64], int (*vdct)[64], int w, int h);
extern void vli_encode(short val, uint8* level, short* code);
extern void ac_rle_encode(int* block64, struct rle_data* rle64);

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
void test_yuv_to_blocks8x8_2() {
    printf("bgn test_yuv_to_blocks8x8_2()\n");
    do {
	uint8 y[64];
	int w = 8;
	int h = 8;
	for (int i = 0; i < 64; i++) {
		y[i] = i;
	}

	uint8 yblocks[64];
	uint8 ublocks[64];
	uint8 vblocks[64];

	yuv_to_blocks8x8_2(y, y, y, w, h, &yblocks, &ublocks, &vblocks);

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
    }while(0);
    do {
	uint8 y[8*14];
	int w = 8;
	int h = 14;
	for (int i = 0; i < 8*14; i++) {
		y[i] = i;
	}

	uint8 yblocks[2][8*8];
	uint8 ublocks[2][8*8];
	uint8 vblocks[2][8*8];

	yuv_to_blocks8x8_2(y, y, y, w, h, yblocks, ublocks, vblocks);
        
	for (int i = 0; i < 2; i++) {
	    for (int j = 0; j < 8; j++) {
	        for (int k = 0; k < 8; k++) {
		    printf("%d, ", yblocks[i][j*8+k]);
                }
                printf("\n");
            }
            printf("\n\n");
	}
    }while(0);
    do {
	uint8 y[7*14];
	int w = 7;
	int h = 14;
	for (int i = 0; i < 7*14; i++) {
		y[i] = i;
	}

	uint8 yblocks[2][8*8];
	uint8 ublocks[2][8*8];
	uint8 vblocks[2][8*8];

	yuv_to_blocks8x8_2(y, y, y, w, h, yblocks, ublocks, vblocks);
        
	for (int i = 0; i < 2; i++) {
	    for (int j = 0; j < 8; j++) {
	        for (int k = 0; k < 8; k++) {
		    printf("%d, ", yblocks[i][j*8+k]);
                }
                printf("\n");
            }
            printf("\n\n");
	}
    }while(0);
    
    printf("end test_yuv_to_blocks8x8_2()\n");
}

void test_yuv_to_blocks8x8() {

    do {
        uint8 y[64];
        int w = 8;
        int h = 8;
        for (int i = 0; i < 64; i++) {
            y[i] = i;
        }

	uint8 yblocks[64];
	uint8 ublocks[64];
	uint8 vblocks[64];
    
	int nwblock = w / 8 + (w % 8 ? 1 : 0);
	int nhblock = h / 8 + (h % 8 ? 1 : 0);
	yuv_to_blocks8x8(y, y, y, w, h, &yblocks, &ublocks, &vblocks, nwblock, nhblock);

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
    }while(0);

    //////////////////////////////
    do {
        uint8 y[120];
        int w = 15;
        int h = 8;
        for (int i = 0; i < 120; i++) {
            y[i] = i;
        }

        uint8 yblocks[2][64];
        uint8 ublocks[2][64];
        uint8 vblocks[2][64];
    
        int nwblock = w / 8 + (w % 8 ? 1 : 0);
        int nhblock = h / 8 + (h % 8 ? 1 : 0);
        yuv_to_blocks8x8(y, y, y, w, h, yblocks, ublocks, vblocks, nwblock, nhblock);
    

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 64; j++) {
                printf("%d\n", yblocks[i][j]);
            }
        }
        printf("-------------------\n");
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 64; j++) {
                printf("%d\n", ublocks[i][j]);
            }
        }
        printf("-------------------\n");
        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 64; j++) {
                printf("%d\n", vblocks[i][j]);
            }
        }
    }while(0);
    
    //////////////////////////////
    do {
        printf("\n ttttttt\n");
        uint8 y[120];
        int w = 15;
        int h = 8;
        for (int i = 0; i < 120; i++) {
            y[i] = i;
        }

        printf("----------------\n");

        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 15; j++) {
                printf("%d, ", y[i*15+j]);
            }
            printf("\n");
        }
        uint8 yblocks[2][64];
        uint8 ublocks[2][64];
        uint8 vblocks[2][64];
    
        uint8* pyblocks = (uint8*) yblocks;
        int nwblock = w / 8 + (w % 8 ? 1 : 0);
        int nhblock = h / 8 + (h % 8 ? 1 : 0);
        yuv_to_blocks8x8(y, y, y, w, h, yblocks, ublocks, vblocks, nwblock, nhblock);
    
        // uint8* publocks = (uint8*) ublocks;
        // uint8* pvblocks = (uint8*) vblocks;
        printf("--------------------\n");
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 16; j++) {
                printf("%d, ", pyblocks[i*16+j]);
            }
            printf("\n");
        }
    }while(0);

    do {
        uint8 y[56];
        int w = 7;
        int h = 8;
        for (int i = 0; i < 56; i++) {
            y[i] = i;
        }

       uint8 yblocks[1][64];
       uint8 ublocks[1][64];
       uint8 vblocks[1][64];

       //int nwblock = 1;
       //int nhblock = 1;
       int nwblock = w / 8 + (w % 8 ? 1 : 0);
       int nhblock = h / 8 + (h % 8 ? 1 : 0);

       uint8* pyblocks = (uint8*) yblocks;

       yuv_to_blocks8x8(y, y, y, w, h, yblocks, ublocks, vblocks, nwblock, nhblock);
       for (int i = 0; i < 8; i++) {
           for (int j = 0; j < 8; j++) {
               printf("%d, ", pyblocks[i*8+j]);
           }
           printf("\n");
       }
    } while(0);

    do {
        uint8 y[14*15];
        int w = 14;
        int h = 15;
        for (int i = 0; i < 14*15; i++) {
            y[i] = i;
        }

       uint8 yblocks[4][64];
       uint8 ublocks[4][64];
       uint8 vblocks[4][64];

       //int nwblock = 1;
       //int nhblock = 1;
       int nwblock = w / 8 + (w % 8 ? 1 : 0);
       int nhblock = h / 8 + (h % 8 ? 1 : 0);

       uint8* pyblocks = (uint8*) yblocks;

       yuv_to_blocks8x8(y, y, y, w, h, yblocks, ublocks, vblocks, nwblock, nhblock);
       for (int i = 0; i < 16; i++) {
           for (int j = 0; j < 16; j++) {
               printf("%d, ", pyblocks[i*16+j]);
           }
           printf("\n");
       }
       for (int i = 0; i < 4; i++) {
           for (int j = 0; j < 64; j++) {
               printf("%d,", yblocks[i][j]);
           }
           printf("\n");
       }
    } while(0);
}

//void print_blocks(int(*blocks)[64], int nw, int nh) {
#define PRNT_BLOCKS(blocks, nw, nh)\
    for (int i = 0; i < nw*nh; i++) {\
        for (int j = 0; j < 8; j++) {\
            for (int k = 0; k < 8; k++) {\
                printf("%d, ", blocks[i][j*8+k]);\
            }\
            printf("\n");\
        }\
        printf("\n\n");\
    }

void test_codec_pipeline() {
    // load rgb

    uint8* r = NULL;
    uint8* g = NULL;
    uint8* b = NULL;
    int w = 0;
    int h = 0;
    // char* sfile = "/Users/mjohh/Documents/hellocodec/src/room.txt";
    // 相对于可执行文件
    char* sfile = "py/room.txt";

    int ret = load_rgb(sfile, &r, &g, &b, &w, &h);
    printf("ret=%d, w=%d, h=%d\n", ret, w, h);
    //int i = 0;
    //for (i=0; i<w*h; i++) {
    //    printf("%d,%d,%d\n", r[i], g[i], b[i]);
    //}

    // rgb to yuv
    uint8* y = malloc(w*h);
    uint8* u = malloc(w*h);
    uint8* v = malloc(w*h);
    assert(y);
    assert(u);
    assert(v);
    rgb_to_yuv(r, g, b, y, u, v, w, h);

    // yuv to blocks
    int nwb = w/8 + (w%8 ? 1 : 0);
    int nhb = h/8 + (h%8 ? 1 : 0);
    uint8 (*yblocks)[64] = (uint8(*)[64]) malloc(nwb * nhb * 64);
    uint8 (*ublocks)[64] = (uint8(*)[64]) malloc(nwb * nhb * 64);
    uint8 (*vblocks)[64] = (uint8(*)[64]) malloc(nwb * nhb * 64);
    assert(yblocks);
    assert(ublocks);
    assert(vblocks);
    yuv_to_blocks8x8_2(y, u, v, w, h, yblocks, ublocks, vblocks);
    //printf("----------print yblock----------\n");
    //PRNT_BLOCKS(yblocks, nwb, nhb);
    //printf("%d,%d\n", w, h);
    //printf("end of blocks\n");

    // blocks to dct
    int (*ydct)[64] = (int(*)[64]) malloc(nwb * nhb * sizeof(int) * 64);
    int (*udct)[64] = (int(*)[64]) malloc(nwb * nhb * sizeof(int) * 64);
    int (*vdct)[64] = (int(*)[64]) malloc(nwb * nhb * sizeof(int) * 64);
    assert(ydct);
    assert(udct);
    assert(vdct);
    blocks_fdct(yblocks, ublocks, vblocks, w, h, ydct, udct, vdct); 
    printf("----------print ydct----------\n");
    PRNT_BLOCKS(ydct, nwb, nhb);
    printf("%d,%d\n", w, h);
    printf("end of dct\n");

    // dct to quant
    fdct_2_quant(ydct, udct, vdct, w, h);    
    //printf("----------print quant----------\n");
    //PRNT_BLOCKS(ydct, nwb, nhb);
    //printf("%d,%d\n", w, h);
    //printf("end of quant\n");

    // quant to zigzag
    quant_2_zigzag(ydct, udct, vdct, w, h);
    // printf("----------print zigzag----------\n");
    // PRNT_BLOCKS(ydct, nwb, nhb);
    // printf("%d,%d\n", w, h);
    // printf("end of zigzag\n");
    

    // free memory
    free_rgb(r);
    free_rgb(g);
    free_rgb(b);
    free(yblocks);
    free(ublocks);
    free(vblocks);
    free(ydct);
    free(udct);
    free(vdct);
}


void test_vli_encode() {
    short val = -32767;
    short code = 0;
    uint8 level = 0;

    vli_encode(val, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 15);
    assert(code == 0);
   
    vli_encode(32767, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 15);
    assert(code == 32767);
    
    vli_encode(0, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 0);
    assert(code == 0);

    vli_encode(-1, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 1);
    assert(code == 0);

    vli_encode(1, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 1);
    assert(code == 1);

    vli_encode(-2, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 2);
    assert(code == 1);

    vli_encode(2, &level, &code);
    printf("level=%d\n", level);
    printf("code=%d\n", code);
    assert(level == 2);
    assert(code == 2);
}

int block[] = {
    123, 9, 3, 9, 0,0,0, 5, 
    0, 8, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 12,0, 0,
    3, 5, 7, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0,
};

void test_ac_rle_encode() {
    struct rle_data rle[64];
    ac_rle_encode(block, rle);
    
    assert(rle[0].run_length==0 && rle[0].data==9);
    assert(rle[1].run_length==0 && rle[1].data==3);
    assert(rle[2].run_length==0 && rle[2].data==9);
    assert(rle[3].run_length==3 && rle[3].data==5);
    assert(rle[4].run_length==1 && rle[4].data==8);
    assert(rle[5].run_length==15 && rle[5].data==0);
    assert(rle[6].run_length==4 && rle[6].data==12);
    assert(rle[7].run_length==2 && rle[7].data==3);
    assert(rle[8].run_length==0 && rle[8].data==5);
    assert(rle[9].run_length==0 && rle[9].data==7);
    assert(rle[10].run_length==0 && rle[10].data==0);
}

const uint8 huftable[] = {
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
void test_tbl() {
    uint8 hufsize[256];
    int hufcode[256];

    int k = 0;
    int code = 0x00;
    int i;
    int j;

    for (i = 0; i < 16; i++) {
        for (j = 0; j < huftable[i]; j++) {
            hufsize[k] = i + 1;
            hufcode[k] = code;
            code++;
            k++;
        }
        code <<= 1;
    }
    printf("k=%d\n", k);
    for (i = 0; i < k; i++) {
        printf("%d,", hufsize[i]);
        if((i+1)%16==0) {
            printf("\n");
        }
    }
    printf("\n\n");
    for (i = 0; i < k; i++) {
        printf("0x%x,", hufcode[i]);
        if((i+1)%16==0) {
            printf("\n");
        }
    }
}
