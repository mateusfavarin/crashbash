#include <types.h>
#include <math.h>
#include <player.h>
#include <extern.h>
#include "coll.h"
#include "ball_extern.h"

u32 Player_onCollide(Object * pPlayer)
{
    for (s32 i = 0; i < NUM_PILLARS; i++)
    {
        Circle * pPillarPos = &_Ball_pillarData.pos[i];
        s32 xDist = pPlayer->pos.x - pPillarPos->x;
        s32 zDist = pPlayer->pos.z - pPillarPos->z;
        s32 radius;
        if (pPlayer->field19_0x90 & 0x40) // TODO: Figure out this flag
        {
            radius = pPillarPos->radius + pPlayer->radius;
        }
        else
        {
            radius = pPillarPos->radius + FP(0.5);
        }
        if ((radius * radius) > ((xDist * xDist) + (zDist * zDist)))
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