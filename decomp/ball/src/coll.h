#ifndef BALL_COLL_H
#define BALL_COLL_H

#include <types.h>
#include <math.h>

#define NUM_PILLARS 4

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

extern PillarDatabase pillarDatabase[NUM_PILLARS];

#endif