#ifndef PLAYER_H
#define PLAYER_H

#include <types.h>
#include <math.h>
#include <bot.h>

#define NUM_PLAYERS 4
#define PLAYER_1 0
#define PLAYER_2 1
#define PLAYER_3 2
#define PLAYER_4 3
#define NO_PLAYER -1

typedef enum PlayersCampaign
{
    SOLO = 0,
    COOP = 1,
} PlayersCampaign;

typedef enum PlayerAction
{
    ACTION_NONE     = 0,
    ACTION_READY    = 28,
    ACTION_MOVE     = 29,
    ACTION_TAUNT    = 30,
    ACTION_ATTACK   = 31,
    ACTION_WIN      = 34,
    ACTION_LOSE     = 36,
    ACTION_GHOST    = 37,
} PlayerAction;

typedef struct Object
{
    u32 status;
    u8 field1_0x4[12];
    Vec3 pos;
    ModelMatrix modelMatrix;
    Vec3 prevPos;
    u8 field16_0x6c[30];
    s16 radius;
    u8 field18_0x8c[4];
    u32 field19_0x90;
} Object;

typedef struct PlayerPhysics
{
    u8 field0_0x0[12];
    s32 speed;
    u8 field1_0x10[4];
    s16 angle;
    u16 field2_0x16;
    s16 angleCalc;
    u16 field3_0x1a;
    u8 field4_0x1c;
    u8 field5_0x1d;
    u8 field6_0x1e[12];
} PlayerPhysics;

typedef struct PlayerMetadata
{
    void * field0_0x0;
    Object * pPlayer;
    void * field2_0x8[4];
    PlayerPhysics * pPlayerPhysics;
    void * field4_0x1c;
    Bot * pBot;
    u8 field4_0x24[8];
    u16 action;
    s16 field6_0x2e;
    s16 teamID;
    u16 flags;
    s16 field9_0x34;
    s16 health;
    u8 field11_0x38[18];
    s16 moveCooldown;
    u8 field13_0x4c[4];
    u16 score;
    u16 scoreStr;
    u8 field15_0x54[18];
    s16 attackCooldown;
    u8 field17_0x68[4];
} PlayerMetadata;

typedef const struct PlayerAdjacencyList
{
    s32 * pRightPlayerID;
    s32 * pMiddlePlayerID;
    s32 * pLeftPlayerID;
} PlayerAdjacencyList;

#endif