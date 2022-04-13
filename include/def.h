#ifndef __def_h__
#define __def_h__

typedef unsigned char uint8;
typedef uint8 rgb[3];
typedef uint8 yuv[3];

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

int load_rgb(const char* rgb_file, rgb** rgb_ary, int* w, int* h);

#endif //__def_h__
