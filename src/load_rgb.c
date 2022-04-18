#include "def.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#define BUF_SZ 128
int load_rgb(const char* rgb_file, uint8** r, uint8** g, uint8** b, int* w, int* h) {
    FILE * pf = NULL;
    char buf[BUF_SZ];
    
    if((pf = fopen(rgb_file, "r")) == NULL) {
        printf("open %s fail!, errno=%d\n", rgb_file, errno);
        return -1;
    }
   
    int width=0;
    int height=0;
    fgets(buf, BUF_SZ, pf);
    sscanf(buf, "%d,%d\n", &width, &height);
    //rgb* p = malloc(sizeof(rgb) * width * height);
    uint8* r1 = malloc(sizeof(uint8) * width * height);
    uint8* g1 = malloc(sizeof(uint8) * width * height);
    uint8* b1 = malloc(sizeof(uint8) * width * height);
    assert(r1);
    assert(g1);
    assert(b1);

    int i = 0;
    while(!feof(pf)) {
        fgets(buf, BUF_SZ, pf);
        int tmpr = 0;
        int tmpg = 0;
        int tmpb = 0;
        sscanf(buf,"%d,%d,%d\n", &tmpr, &tmpg, &tmpb);
        r1[i] = BOUND(tmpr, 0, 255);
        g1[i] = BOUND(tmpg, 0, 255);
        b1[i] = BOUND(tmpb, 0, 255);
        i++;
    };
    fclose(pf);


    printf("width,height=%d,%d\n", width, height);
    printf("i=%d\n", i);
    assert(i-1==width*height);
    *r = r1;
    *g = g1;
    *b = b1;
    *w = width;
    *h = height;
    return 0;
}

//void free_rgb(rgb* rgb_ary) {
void free_rgb(uint8* rgb) {
    free(rgb);
}


void test_load_rgb() {
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
    int i = 0;
    for (i=0; i<w*h; i++) {
        printf("%d,%d,%d\n", r[i], g[i], b[i]);
    }
    if (r) {
        free_rgb(r);
    }
    if (g) {
        free_rgb(g);
    }
    if (b) {
        free_rgb(b);
    }
}
