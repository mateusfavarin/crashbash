#ifndef BALL_COLL_H
#define BALL_COLL_H

#include <types.h>
#include <math.h>

#define NUM_PILLARS 4

typedef struct PillarDatabase
{
    Circle pos[NUM_PILLARS];
    Circle maybeUnused[NUM_PILLARS];
    Vec3 ballSpawnPos[NUM_PILLARS];
} PillarDatabase;

typedef struct PillarData
{
    Circle * pos;
    Circle * maybeUnused;
    Vec3 * ballSpawnPos;
} PillarData;

extern const PillarDatabase pillarDatabase[NUM_PILLARS];

#endif