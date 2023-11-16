#ifndef BOT_H
#define BOT_H

#include <types.h>
#include <math.h>

#define MAX_BOT_QUALITY 32

typedef enum BotState
{
    BOTSTATE_NONE            = 0,
    BOTSTATE_SET_TARGET      = 1,
    BOTSTATE_DEFEND          = 3,
    BOTSTATE_FOLLOW_TARGET   = 4,
    BOTSTATE_TAUNT           = 5,
    BOTSTATE_DO_NOTHING      = 20,
    BOTSTATE_WAIT            = 21
} BotState;

typedef struct BotDifficulty
{
    s16 quality;
    s16 field1_0x2;
    s16 publicEnemyFactor;
} BotDifficulty;

typedef struct Bot
{
    u8 field0_0x0[16];
    s32 enemyID;
    Vec3 targetPos;
    u16 state;
    u16 field6_0x22;
    u16 aggroCooldown;
    u16 magnetTimer;
    u16 field9_0x28;
    s16 targetAngle;
    u32 nextInputs;
    u16 field12_0x30;
    u16 tauntCooldown;
} Bot;

#endif