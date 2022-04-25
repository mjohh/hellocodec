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
void fdct_2_quant(int (*ydct)[64], int (*udct)[64], int (*vdct)[64], int w, int h);


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
    //printf("----------print ydct----------\n");
    //PRNT_BLOCKS(ydct, nwb, nhb);
    //printf("%d,%d\n", w, h);
    //printf("end of dct\n");

    // dct to quant
    fdct_2_quant(ydct, udct, vdct, w, h);    
    //printf("----------print quant----------\n");
    //PRNT_BLOCKS(ydct, nwb, nhb);
    //printf("%d,%d\n", w, h);
    //printf("end of quant\n");

    // quant to zigzag
    quant_2_zigzag(ydct, udct, vdct, w, h);
    printf("----------print zigzag----------\n");
    PRNT_BLOCKS(ydct, nwb, nhb);
    printf("%d,%d\n", w, h);
    printf("end of zigzag\n");
    

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
