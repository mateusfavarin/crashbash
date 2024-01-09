#include <types.h>
#include <math.h>
#include <extern.h>

/*
    @brief: Advances seed and returns a random number modulo range
    @address: 0x80015590
    @params:
        @range: [2, +inf)
    @return: number mod range if valid range, else 0
*/
u32 Rand(u32 range)
{
    if (range > 1)
    {
        AdvanceSeed();
        return _prng.seed % range;
    }
    return 0;
}

/*
    @brief: Subtracts angle x from y
    @address: 0x800154a0
    @params:
        @x
        @y
    @return: (-180, 180]
*/
s32 SubtractAngles(s32 x, s32 y)
{
    s32 res = ANG_MOD(y - x);
    if (res > ANG_180) return res - ANG_360;

    return res;
}

/*
    @brief: Returns the smallest positive angle from y - x
    @address: 0x80015480
    @params:
        @x
        @y
    @return: [0, 180]
*/
s32 AbsSubtractAngles(s32 x, s32 y)
{
    s32 res = ANG_MOD(y - x);
    if (res > ANG_180) return ANG_360 - res;

    return res;
}