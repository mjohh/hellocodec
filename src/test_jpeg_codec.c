#include<stdio.h>
#include<math.h>
#include<assert.h>
#include "def.h"

//typedef unsigned char uint8;

extern void _rgb_to_yuv(uint8 r, uint8 g, uint8 b, uint8* y, uint8* u, uint8* v);
extern void yuv_to_blocks8x8(uint8* y, uint8* u, uint8* v, int w, int h,
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64], int nwblocks, int nhblocks);
void yuv_to_blocks(uint8* y, uint8* u, uint8* v, int w, int h,
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64]);
void yuv_to_blocks8x8_2(uint8* y, uint8* u, uint8* v, int w, int h,
    uint8 yblocks[][64], uint8 ublocks[][64], uint8 vblocks[][64]);

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
