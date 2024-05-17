#include <types.h>
#include <math.h>
#include <player.h>
#include <extern.h>
#include <kernel.h>
#include "coll.h"
#include "ball_extern.h"

#define MAX_SPEED_RUNNING    160
#define ACCEL_RATE_RUNNING   56
#define MAX_SPEED_NORMAL     104
#define ACCEL_RATE_NORMAL    34
#define ACCEL_RATE_NORMAL_P2 42 // P2 is faster if you're not holding the shoulder buttons
#define DEACCEL_RATE         18

/*
    @brief: Handles the player collision with the world geometry
    @address: 0x800b42f4
    @params:
        @pPlayer
    @return: bool
*/
u32 Player_onCollide(Object * pPlayer)
{
    for (s32 i = 0; i < NUM_PILLARS; i++)
    {
        const Circle * pPillarPos = &_Ball_pillarData.pos[i];
        s32 xDist = pPlayer->pos.x - pPillarPos->x;
        s32 zDist = pPlayer->pos.z - pPillarPos->z;
        // TODO: Figure out this flag
        s32 radius = (pPlayer->field19_0x90 & 0x40) ? pPillarPos->radius + pPlayer->radius : pPillarPos->radius + FP(0.5);
        if ((radius * radius) > DIST_SQ(xDist, zDist))
        {
            s32 ang = _ArcTan(-xDist, -zDist);
            if ((pPlayer == _playerMetadata[_Ball_PLAYER_1_ID].pPlayer) ||
                (pPlayer == _playerMetadata[_Ball_PLAYER_2_ID].pPlayer))
            {
                pPlayer->pos.x = pPillarPos->x - PSYQ_FP_MULT(radius, SIN(ang));
            }
            else
            {
                pPlayer->pos.z = pPillarPos->z - PSYQ_FP_MULT(radius, COS(ang));
            }
            return true;
        }
    }
    return false;
}

/*
    @brief: Updates player speed per frame
    @address: 0x800b558c
    @params:
        @playerID
*/
void Player_onSpeed(s32 playerID)
{
    PlayerMetadata pm = _playerMetadata[playerID];
    Controller controller = _controllers[playerID];
    if (pm.moveCooldown)
    {
        controller.buttonsHeld = BTN_NONE;
        controller.buttonsPressed = BTN_NONE;
    }
    if ((!(controller.flags & CONTROLLER_CONNECTED)) && (pm.pBot->state == BOTSTATE_NONE)) { return; }

    Object * pPlayer = pm.pPlayer;
    PlayerPhysics * pPhys = pm.pPlayerPhysics;
    s32 speedCap = MAX_SPEED_NORMAL;
    s32 accel = (playerID == _Ball_PLAYER_2_ID) ? ACCEL_RATE_NORMAL_P2 : ACCEL_RATE_NORMAL;
    if (controller.buttonsHeld & BTN_SHOULDERS)
    {
        speedCap = MAX_SPEED_RUNNING;
        accel = ACCEL_RATE_RUNNING;
    }

    u32 hasCollided = _FUNC_Player_onCollide(pPlayer);
    u32 posInput = 0;
    u32 negInput = 0;
    if ((playerID == _Ball_PLAYER_1_ID) || (playerID == _Ball_PLAYER_2_ID))
    {
        posInput = controller.buttonsHeld & BTN_RIGHT;
        negInput = controller.buttonsHeld & BTN_LEFT;
    }
    else
    {
        posInput = controller.buttonsHeld & BTN_UP;
        negInput = controller.buttonsHeld & BTN_DOWN;
    }

    if (posInput | negInput)
    {
        if (pPhys->field5_0x1d)
        {
            s32 accelRate = 0;
            if (posInput) { accelRate += accel; }
            if (negInput) { accelRate -= accel; }
            if ((hasCollided) && (accelRate != 0)) { accelRate += (accelRate > 0) ? speedCap / 2 : -speedCap / 2; }
            pPhys->speed = Max(Min(pPhys->speed + accelRate, speedCap), -speedCap);
            if ((pPhys->speed == 0) && (accelRate != 0)) { pPhys->speed = (accelRate > 0) ? 1 : -1; }
        }
    }
    else
    {
        if(hasCollided) { pPhys->speed = 0; }
        else { pPhys->speed = (pPhys->speed > 0) ? Max(pPhys->speed - DEACCEL_RATE, 0) : Min(pPhys->speed + DEACCEL_RATE, 0); }
    }
    _Ball_playerSpeedCopy[playerID] = pPhys->speed;
    pPhys->angle = pPhys->angleCalc;
}