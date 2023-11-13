#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <types.h>

typedef enum Buttons
{
    BTN_NONE        = 0x0,
    BTN_SELECT      = 0x1,
    BTN_L3          = 0x2,
    BTN_R3          = 0x4,
    BTN_START       = 0x8,
    BTN_UP          = 0x10,
    BTN_RIGHT       = 0x20,
    BTN_DOWN        = 0x40,
    BTN_LEFT        = 0x80,
    BTN_L2          = 0x100,
    BTN_R2          = 0x200,
    BTN_L1          = 0x400,
    BTN_R1          = 0x800,
    BTN_TRIANGLE    = 0x1000,
    BTN_CIRCLE      = 0x2000,
    BTN_CROSS       = 0x4000,
    BTN_SQUARE      = 0x8000,
    BTN_SHOULDERS   = BTN_L2 | BTN_L1 | BTN_R1 | BTN_R2,
} Buttons;

typedef struct Controller
{
    s32 field0_0x0[11];
    u32 isBotInput;
    u32 botInput;
    s32 field3_0x34;
    s32 targetAngle;
    s32 field2_0x3c[5];
    u32 buttonsHeld;
    u32 buttonsPressed;
} Controller;

#endif