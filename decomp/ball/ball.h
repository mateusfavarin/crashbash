#ifndef BALL_H
#define BALL_H

typedef enum BallState
{
    BALLSTATE_INIT   = 0,
    BALLSTATE_PLAY   = 1,
    BALLSTATE_LAUNCH = 6,
    BALLSTATE_AIR    = 8,
    BALLSTATE_SPAWN  = 12,
    BALLSTATE_SCORED = 17,
    BALLSTATE_DELETE = 18,
} BallState;

#endif