#ifndef DEFS_H
#define DEFS_H

#define SDL_MAIN_HANDLED

#define WINDOW_W 1280
#define WINDOW_H 720

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include <SDL2/SDL.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;

typedef float f32;
typedef double f64;

typedef u8 b8;

#define TRUE ((b8)1)
#define FALSE ((b8)0)

typedef struct {
	f32 x, y, z, w;
}vec_t;

#define VEC_ZERO ((vec_t){ 0, 0, 0, 0 })
#define VEC2(x, y) ((vec_t){ (x), (y), 0, 0 })
#define VEC3(x, y, z) ((vec_t){ (x), (y), (z), 0 })
#define VEC4(x, y, z, w) ((vec_t){ (x), (y), (z), (w) })
#define VEC_CMP(v0, v1) ( (b8)( ((v0).x == (v1).x) && ((v0).y == (v1).y) && ((v0).z == (v1).z) && ((v0).w == (v1).w) ) )
#define VEC_IS_ZERO(v) ((b8)(VEC_CMP((v), VEC_ZERO)))
#define VEC_DOT(v0, v1) (((v0).x * (v1).x) + ((v0).y * (v1).y) + ((v0).z * (v1).z) + ((v0).w * (v1).w))
#define VEC_MAG(v) (sqrtf(VEC_DOT((v),(v))))
#define VEC_DELTA(v0, v1) (VEC4((v0).x - (v1).x, (v0).y - (v1).y, (v0).z - (v1).z, (v0).w - (v1).w))

#define VEC_ADD(v0, v1) \
do{ \
	(v0)->x += (v1).x; \
	(v0)->y += (v1).y; \
	(v0)->z += (v1).z; \
	(v0)->w += (v1).w; \
}while(0)

#define VEC_SUB(v, n) \
do{ \
	f32 _n = (f32)(n); \
	(v)->x -= _n; \
	(v)->y -= _n; \
	(v)->z -= _n; \
	(v)->w -= _n; \
}while(0)

#define VEC_SCALE(v, n) \
do { \
    f32 _n = (f32)(n); \
    (v)->x *= _n; \
    (v)->y *= _n; \
    (v)->z *= _n; \
    (v)->w *= _n; \
}while(0)

#define VEC_DIV(v, n) \
do{ \
	if((n) != 0.0f) { \
	(v)->x /= (n); \
	(v)->y /= (n); \
	(v)->z /= (n); \
	(v)->w /= (n); \
	} \
}while(0)

#define VEC_NORM(v) \
do{ \
	f32 mag = VEC_MAG(*(v)); \
	VEC_DIV(v, mag); \
}while(0)

void quit(i32 code);

#endif