#ifndef BALL_EXTERN_H
#define BALL_EXTERN_H

#include <types.h>
#include <player.h>
#include "coll.h"

/* Variables */
extern s32 (* _Ball_Func_Ball_onCollide_World(Object * pBall, s32 ballAngle));
extern PillarObj _Ball_pillarObj[4];
extern PillarData _Ball_pillarData;
extern u16 _Ball_activeWalls;
extern s16 _Ball_playerSpeedCopy[NUM_PLAYERS];
extern s16 _Ball_defaultChallengeQuality;
extern u16 _Ball_unk_800c1f0c;
extern s32 _Ball_PLAYER_1_ID;
extern s32 _Ball_PLAYER_2_ID;
extern s32 _Ball_PLAYER_3_ID;
extern s32 _Ball_PLAYER_4_ID;

#endif