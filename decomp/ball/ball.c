#include <types.h>
#include <math.h>
#include <player.h>
#include <extern.h>
#include "ball.h"
#include "coll.h"
#include "ball_extern.h"

/*
    @brief: Update pBallPos to the coordinate on the collided circle circunference
    @address: 0x800be1c4
    @params:
        @pBallPos: ball position inside the circle
        @ballAngle
        @pCircle
        @totalRadius: ball radius + circle radius
*/
static void Ball_onCollide_Circle(Vec3 * pBallPos, s32 ballAngle, const Circle * pCircle, s32 totalRadius)
{
    /*
        The idea of this function is to calculate the ball positions x2, y2 outside of the circle,
        and then find the intersection point of the line (x1, y1) -> (x2, y2) within the circle circunference.
        Let the circle equation be:
            (x - x_circle)^2 + (y - y_circle)^2 = r_circle^2

        Let the point (x2, y2) be A, (x1, y1) be B, and the intersection in the circle be C. Also let D
        be a point such that triangle ADB is right angle, and let E be a point in AD such that
        triangle AEC is right angle.
        By construction, triangle ADB and AEC are similar, therefore AE / AD = CE / BD
        Assume that the intersection point is at coords (xc, yc). Which means:
            (x2 - xc) / (x2 - x1) = (y2 - yc) / (y2 - y1) = k

        Therefore we can write xc = x2 - (delta_x * k), yc = y2 - (delta_y * k)
        Plugging xc, yc into the initial equation, we have:
            (x2 - (delta_x * k) - x_circle)^2 + (y2 - (delta_y * k) - y_circle)^2 = r_circle^2
            Let x_circle - x2 = x_dist, and y_circle - y2 = y_dist. By expanding and re-arranging the terms:
            (delta_x^2 + delta_y^2) * k^2 + (2 * delta_x * x_dist + 2 * delta_y * y_dist) * k + x_dist^2 + y_dist^2 - r_circle^2  = 0

        We have a second degree equation f(k), and we can solve f(k) = 0 to find out the two intersection points in the circle.
        The intersection point we want will be the smallest positive solution of the equation.
    */
    s32 oppositeAngle = ANG_MOD(ballAngle + ANG_180);
    s32 x2 = pBallPos->x - (SIN(oppositeAngle) >> 3);
    s32 z2 = pBallPos->z - (COS(oppositeAngle) >> 3);
    s32 deltaX = x2 - pBallPos->x;
    if (deltaX == 0) return;

    s32 deltaZ = z2 - pBallPos->z;
    s32 distX = pCircle->x - x2;
    s32 distZ = pCircle->z - z2;

    // f(k) = ak^2 + bk + c
    s32 a = ((deltaX * deltaX) + (deltaZ * deltaZ)) >> 4;
    s32 b = (2 * ((deltaX * distX) + (deltaZ * distZ))) >> 4;
    s32 c = ((distX * distX) + (distZ * distZ) - (totalRadius * totalRadius)) >> 4;
    s32 delta = (b * b) - (4 * a * c);
    if (delta < 0) return;
    s32 deltaSquared = SQRT_SQ(delta);

    s32 k1 = FP_DIV((-b + deltaSquared), (2 * a));
    s32 k2 = FP_DIV((-b - deltaSquared), (2 * a));
    s32 kmax = Max(k1, k2);
    if (kmax < 0) return;
    s32 k = Min(k1, k2);
    if (k < 0)
    {
        k = kmax;
    }
    pBallPos->x = x2 - FP_MULT(deltaX, k);
    pBallPos->z = z2 - FP_MULT(deltaZ, k);
}

/*
    @brief: Calculates the ball collision with the world geometry (Pillars)
    @address: 0x800b4880
    @params:
        @pBall
        @ballAngle
    @return: collision angle if the ball collided, otherwise -1
*/
s32 Ball_onCollide_World(Object * pBall, s32 ballAngle)
{
    s32 radius;
    s32 pillarCollided = -1;
    for (s32 i = 0; i < NUM_PILLARS; i++)
    {
        const Circle * pPillarPos = &_Ball_pillarData.pos[i];
        // TODO: Figure out this flag
        radius = (pBall->field19_0x90 & 0x40) ? pPillarPos->radius + pBall->radius : pPillarPos->radius + FP(0.5);
        s32 deltaX = pBall->pos.x - pPillarPos->x;
        s32 deltaZ = pBall->pos.z - pPillarPos->z;
        if (((deltaX * deltaX) + (deltaZ * deltaZ)) < (radius * radius))
        {
            pillarCollided = i;
            break;
        }
    }
    if (pillarCollided == -1)
    {
        s32 collisionAngle = ANG_NO_COLL;
        if (_Ball_activeWalls == 0) return collisionAngle;

        // TODO: Figure out this flag
        s32 edgeCoords = (pBall->field19_0x90 & 0x40) ? FP(8) - pBall->radius : FP(8);
        /*
            Note that for Bash angles:
            0deg   = SOUTH
            90deg  = WEST
            180deg = NORTH
            270deg = EAST
        */
        if ((_Ball_activeWalls & 0x11) && (pBall->pos.z < -edgeCoords)) // P1
        {
            pBall->pos.z = -edgeCoords;
            collisionAngle = ANG_180;
        }

        if ((_Ball_activeWalls & 0x22) && (pBall->pos.z > edgeCoords)) // P2
        {
            pBall->pos.z = edgeCoords;
            collisionAngle = ANG_0;
        }

        if ((_Ball_activeWalls & 0x88) && (pBall->pos.x > edgeCoords)) // P3
        {
            pBall->pos.x = edgeCoords;
            collisionAngle = ANG_90;
        }

        if ((_Ball_activeWalls & 0x44) && (pBall->pos.x < -edgeCoords)) // P4
        {
            pBall->pos.x = -edgeCoords;
            collisionAngle = ANG_270;
        }

        if (collisionAngle != -1)
        {
            // TODO: PlaySFX
        }
        return collisionAngle;
    }

    const Circle * pPillarPos = &_Ball_pillarData.pos[pillarCollided];
    Ball_onCollide_Circle(&pBall->pos, ballAngle, pPillarPos, radius);
    _Ball_pillarObj[pillarCollided].lightCountdown = SECONDS(1/10);
    return _ArcTan(pPillarPos->x - pBall->pos.x, pPillarPos->z - pBall->pos.z);
}