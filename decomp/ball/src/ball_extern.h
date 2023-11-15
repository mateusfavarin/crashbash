#ifndef BALL_EXTERN_H
#define BALL_EXTERN_H

#include <types.h>
#include "coll.h"

/* Functions */
void _Ball_Bot_onUpdate(s32 playerID);

/* Variables */
extern s16 _Ball_defaultChallengeQuality;
extern u16 _Ball_unk_800c1f0c;
extern s32 _Ball_PLAYER_1_ID;
extern s32 _Ball_PLAYER_2_ID;
extern s32 _Ball_PLAYER_3_ID;
extern s32 _Ball_PLAYER_4_ID;
extern PillarData _Ball_pillarData;

#endif