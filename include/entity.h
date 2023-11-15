#ifndef ENTITY_H
#define ENTITY_H

#include <types.h>
#include <math.h>
#include <player.h>

typedef enum EntityColor
{
    GRAY_BALL   = 0x1,
    RED_BALL    = 0x2
} EntityColor;

typedef struct Entity
{
    u8 field0_0x0[20];
    u16 color;
    u8 field2_0x16[4];
    u16 state;
} Entity;

typedef struct EntityLinkedList
{
    u8 field0_0x0[8];
    void * field1_0x8;
    u8 field2_0xc[20];
    struct EntityLinkedList * next;
    u8 field4_0x24[36];
    void (* onInit)(s32);
    void (* onUpdate)(s32);
    u8 field7_0x50[4];
    Object * pEntityObject;
    Entity * pEntity;
} EntityLinkedList;

typedef struct EntityPhysics
{
    u8 field0_0x0[14];
    s16 speed;
    s16 accel;
    s16 deaccel;
    s16 speedCap;
    s16 angle;
} EntityPhysics;

#endif