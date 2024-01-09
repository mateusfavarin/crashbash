#ifndef BALL_COLL_H
#define BALL_COLL_H

#include <types.h>
#include <math.h>

#define NUM_PILLARS 4
#define NUM_LEVELS 4
#define ANG_NO_COLL -1

typedef const struct PillarDatabase
{
    Circle pos[NUM_PILLARS];
    Circle maybeUnused[NUM_PILLARS];
    Vec3 ballSpawnPos[NUM_PILLARS];
} PillarDatabase;

typedef struct PillarData
{
    const Circle * pos;
    const Circle * maybeUnused;
    const Vec3 * ballSpawnPos;
} PillarData;

typedef struct PillarObj
{
    void * field0_0x0;
    void * field1_0x4;
    s16 field2_0x8;
    s16 field3_0xa;
    s16 field4_0xc;
    s16 field5_0xe;
    s16 field6_0x10;
    s16 lightCountdown;
} PillarObj;

extern PillarDatabase pillarDatabase[NUM_LEVELS];

#endif