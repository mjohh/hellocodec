#include "def.h"
#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

#define BUF_SZ 128
int load_rgb(const char* rgb_file, rgb** rgb_ary, int* w, int* h) {
    FILE * pf = NULL;
    char buf[BUF_SZ];
    
    if((pf = fopen(rgb_file, "r")) == NULL) {
        printf("open %s fail!\n", rgb_file);
        return -1;
    }
   
    //int w=0;
    //int h=0;
    fgets(buf, BUF_SZ, pf);
    sscanf(buf, "%d,%d\n", w, h);
    rgb* p = malloc(sizeof(rgb) * (*w)*(*h));
    assert(p);

    int i = 0;
    while(!feof(pf)) {
        int r=0;
        int g=0;
        int b=0;
        fgets(buf, BUF_SZ, pf);
        sscanf(buf,"%d,%d,%d\n", &r, &g, &b);
        p[i][0] = BOUND(r, 0, 255);
        p[i][1] = BOUND(r, 0, 255);
        p[i][2] = BOUND(r, 0, 255);
        i++;
    };
    fclose(pf);


    assert(i==(*w)*(*h));
    *rgb_ary = p;
    return 0;
}
