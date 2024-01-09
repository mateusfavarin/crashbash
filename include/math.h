#ifndef MATH_H
#define MATH_H

#include <types.h>

/* Frame dependent math */

#define FPS 30
#define SECONDS(x) (((s32) ((float) (x))) * FPS)
#define MINUTES(x) (60 * SECONDS(x))
#define HOURS(x) (60 * MINUTES(x))

/* Fixed point math used by the PSX SDK */

#define PSYQ_FRACTIONAL_BITS 12
#define PSYQ_FP_ONE (1 << PSYQ_FRACTIONAL_BITS)
#define PSYQ_FP_MULT(x, y) (((x) * (y)) >> PSYQ_FRACTIONAL_BITS)
#define PSYQ_FP_DIV(x, y) (((x) << PSYQ_FRACTIONAL_BITS) / (y))

/* Fixed point math used by Bash's internal functions */

#define FRACTIONAL_BITS 8
#define FP_ONE (1 << FRACTIONAL_BITS)
#define FP(x) ((s32)(((float)(x)) * FP_ONE))
#define FP_TO_INT(x) ((x) >> FRACTIONAL_BITS)
#define FP_MULT(x, y) (((x) * (y)) >> FRACTIONAL_BITS)
#define FP_DIV(x, y) (((x) << FRACTIONAL_BITS) / (y))

/*
    CSQRT (PSYQ square root fixed point implementation) uses 0x1000 as 1.0;
    Our 1.0 is 0x100, therefore we can use CSQRT to evaluate our own square root by manipulating the equation:
    sqrt(x) = CSQRT(x << 4) >> 4

    A very common case of using the square root function is after you square terms
    (e.g when calculating the distance between two points).
    Due to how fixed point multiplication works, x * y needs to be divided by 2^(fractional bits) after the multiplication.
    So sqrt(x * y) = CSQRT(((x * y) >> 8) << 4) >> 4, however we can do better.

    Lets take the square root of the division without the bitshift by 8, i.e CSQRT(x * y). If we simply apply CSQRT over x * y,
    then we'll be evaluating the square root of a number 16 times bigger than what it truly is (the result is 2^8 bigger than
    the true result, however we already need to multiply by 2^4 to convert our 1.0 scale to CSQRT's scale, therefore our number
    is only bigger by a factor of 2^4, i.e 16).

    So what we're doing is basically sqrt(16 * x), which is 4 * sqrt(x). Therefore we only need to divide the final result by 4,
    which leads to: sqrt(x * y) = CSQRT(x * y) >> 6. Unfortunately I couldn't think of a better way to generalize the macro since
    it depends on the pre-computed result of sqrt(16), which is highly dependant on the size of FRACTIONAL_BITS
*/

#define FP_SQRT(x) (_CSQRT((x) << (PSYQ_FRACTIONAL_BITS - FRACTIONAL_BITS)) >> (PSYQ_FRACTIONAL_BITS - FRACTIONAL_BITS))
#if FRACTIONAL_BITS == 8
    #define SQRT_SQ(x) (_CSQRT((x)) >> 6)
    #define FP_DIST(x, y) (SQRT_SQ(((x) * (x)) + ((y) * (y))))
#else
    #define SQRT_SQ(x) (FP_SQRT((x) >> FRACTIONAL_BITS))
    #define FP_DIST(x, y) (FP_SQRT((((x) * (x)) + ((y) * (y))) >> FRACTIONAL_BITS))
#endif

/* Trigonometry */

#define ANG_0   0x000
#define ANG_90  0x400
#define ANG_180 0x800
#define ANG_270 0xC00
#define ANG_360 0x1000
#define ANG_MOD(x) ((x) & (ANG_360 - 1))
#define ANG(x) ((s32)((((float)(x)) * 0x1000) / 360))

/*
    Despite having sine and cossine data, the game uses the cossine
    for the angle calculation, deriving the sine as cos(x - 90)
*/

#define SIN(x) (_trigTable[ANG_MOD((x) - ANG_90)].cos)
#define COS(x) (_trigTable[ANG_MOD(x)].cos)

/* Structures */

typedef struct Vec3
{
    s32 x;
    s32 y;
    s32 z;
} Vec3;

typedef struct Vec3Pad
{
    s32 x;
    s32 y;
    s32 z;
    s32 pad;
} Vec3Pad;

typedef struct SVec3Pad
{
    s16 x;
    s16 y;
    s16 z;
    s16 pad;
} SVec3Pad;

typedef struct Matrix
{
    s16 m[3][3];
    s32 t[3];
} Matrix;

typedef struct ModelMatrix
{
    SVec3Pad rot;
    SVec3Pad unk;
    Vec3Pad scale;
    Matrix rotMatrix;
    u8 boolScaleModel;
    u8 rotType;
    u16 hasTransformed;
} ModelMatrix;

typedef struct Rect
{
    s16 x;
    s16 y;
    s16 w;
    s16 h;
} Rect;

typedef struct RectPoints
{
    s32 x1;
    s32 z1;
    s32 x2;
    s32 z2;
} RectPoints;

typedef struct Circle
{
    s16 x;
    s16 z;
    s16 radius;
} Circle;

typedef struct TrigTable
{
    s16 sin;
    s16 cos;
} TrigTable;

typedef struct PRNG
{
    u32 seed;
    u32 increment;
} PRNG;

/* Inline functions */

force_inline s32 Abs(s32 x)
{
    if (x > 0) return x;

    return -x;
}

force_inline s32 Max(s32 x, s32 y)
{
    if (x > y) return x;

    return y;
}

force_inline s32 Min(s32 x, s32 y)
{
    if (x > y) return y;

    return x;
}

extern PRNG _prng;
force_inline void AdvanceSeed()
{
    _prng.seed += _prng.increment;
    _prng.increment += (_prng.seed - (((s32)_prng.seed) >> 31));
}

/* Functions */

u32 Rand(u32 range);
s32 SubtractAngles(s32 x, s32 y);
s32 AbsSubtractAngles(s32 x, s32 y);

#endif