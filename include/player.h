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

typedef struct Object
{
    u32 status;
    u8 field1_0x4[12];
    Vec3 pos;
    AngVec rot;
    u8 field8_0x22[10];
    Vec3 scale;
    u8 field12_0x38[40];
    Vec3 prevPos;
    u8 field16_0x6c[30];
    s16 radius;
    u8 field18_0x8c[4];
    u32 field19_0x90;
} Object;

typedef struct Kart
{
    u8 field0_0x0[20];
    s16 angle;
    u16 field2_0x16;
    s16 angleCalc;
} Kart;

typedef struct PlayerMetadata
{
    void * field0_0x0;
    Object * pPlayer;
    void * field2_0x8[4];
    Kart * pKart;
    void * field4_0x1c;
    Bot * pBot;
    u8 field4_0x24[8];
    u16 field5_0x2c;
    s16 field6_0x2e;
    s16 teamID;
    u16 flags;
    s16 field9_0x34;
    s16 health;
    u8 field11_0x38[18];
    s16 moveCooldown;
    u8 field13_0x4c[4];
    u16 score;
    u8 field15_0x52[20];
    s16 attackCooldown;
    u8 field17_0x68[4];
} PlayerMetadata;

typedef struct PlayerAdjacencyList
{
    s32 * pRightPlayerID;
    s32 * pMiddlePlayerID;
    s32 * pLeftPlayerID;
} PlayerAdjacencyList;

#endif