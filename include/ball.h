#ifndef BALL_H
#define BALL_H

#include <types.h>
#include <math.h>
#include <player.h>

typedef enum BallColor
{
    GRAY_BALL   = 0x1,
    RED_BALL    = 0x2
} BallColor;

typedef struct Ball
{
    u8 field0_0x0[20];
    u16 color;
    u8 field2_0x16[4];
    u16 state;
} Ball;

typedef struct BallLinkedList
{
    u8 field0_0x0[8];
    void * field1_0x8;
    u8 field2_0xc[20];
    struct BallLinkedList * next;
    u8 field4_0x24[36];
    void (* onInit)(s32);
    void (* onUpdate)(s32);
    u8 field7_0x50[4];
    Object * pBallObject;
    Ball * pBall;
} BallLinkedList;

typedef struct AnotherBallStruct
{
    u8 field0_0x0[22];
    s16 angle;
} AnotherBallStruct;

#endif