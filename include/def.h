#ifndef __def_h__
#define __def_h__

typedef unsigned char uint8;
//typedef uint8 rgb[3];
//typedef uint8 yuv[3];

/*
struct rgb {
    uint8 r;
    uint8 g;
    uint8 b;
};

struct yuv {
    uint8 y;
    uint8 u;
    uint8 v;
};
*/

#define BOUND(v, min, max)\
    (v)<=(min) ? (min) : ((v)>=(max) ? (max) :(v))

//int load_rgb(const char* rgb_file, rgb** rgb_ary, int* w, int* h);
//void rgb_to_yuv(uint8 r, uint8 g, uint8 b, uint8* y, uint8* u, uint8* v);
//void rgb_to_yuv(rgb* rgb_ary, yuv* yuv_ary, int w, int h);
int load_rgb(const char* rgb_file, uint8** r, uint8** g, uint8** b, int* w, int* h);
void rgb_to_yuv(uint8* r, uint8* g, uint8* b, uint8* y, uint8* u, uint8* v, int w, int h);
void test_load_rgb();
void test_rgb_yuv();
#endif //__def_h__
