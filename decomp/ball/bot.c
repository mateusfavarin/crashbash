#include <types.h>
#include <controller.h>
#include <kernel.h>
#include <minigame.h>
#include <player.h>
#include <bot.h>
#include <entity.h>
#include <extern.h>
#include "ball_extern.h"

#define MAX_DIST 0x7fffff

static PlayerAdjacencyList playerAdjacencyList[NUM_PLAYERS] = {
    [0] = {
        .pRightPlayerID = &_Ball_PLAYER_3_ID,
        .pMiddlePlayerID = &_Ball_PLAYER_1_ID,
        .pLeftPlayerID = &_Ball_PLAYER_4_ID
    },
    [1] = {
        .pRightPlayerID = &_Ball_PLAYER_4_ID,
        .pMiddlePlayerID = &_Ball_PLAYER_2_ID,
        .pLeftPlayerID = &_Ball_PLAYER_3_ID
    },
    [2] = {
        .pRightPlayerID = &_Ball_PLAYER_2_ID,
        .pMiddlePlayerID = &_Ball_PLAYER_3_ID,
        .pLeftPlayerID = &_Ball_PLAYER_1_ID
    },
    [3] = {
        .pRightPlayerID = &_Ball_PLAYER_1_ID,
        .pMiddlePlayerID = &_Ball_PLAYER_4_ID,
        .pLeftPlayerID = &_Ball_PLAYER_2_ID
    }
};

static const RectPoints goalLine[NUM_PLAYERS] = {
    [0] = {
        .x1 = FP(-8), .z1 = FP(-7.625),
        .x2 = FP(8), .z2 = FP(-7.625)
    },
    [1] = {
        .x1 = FP(-8), .z1 = FP(7.625),
        .x2 = FP(8), .z2 = FP(7.625)
    },
    [2] = {
        .x1 = FP(7.625), .z1 = FP(-8),
        .x2 = FP(7.625), .z2 = FP(8)
    },
    [3] = {
        .x1 = FP(-7.625), .z1 = FP(-8),
        .x2 = FP(-7.625), .z2 = FP(8)
    },
};

static const RectPoints attackLine[NUM_PLAYERS] = {
    [0] = {
        .x1 = FP(-5), .z1 = FP(-8),
        .x2 = FP(5), .z2 = FP(-8)
    },
    [1] = {
        .x1 = FP(-5), .z1 = FP(8),
        .x2 = FP(5), .z2 = FP(8)
    },
    [2] = {
        .x1 = FP(8), .z1 = FP(-5),
        .x2 = FP(8), .z2 = FP(5)
    },
    [3] = {
        .x1 = FP(-8), .z1 = FP(-5),
        .x2 = FP(-8), .z2 = FP(5)
    },
};

// By degrees: 0, 45, 90, 135, 180, 225, 270, 315
static const u32 inputDirectionAngle[8] = {
    BTN_UP, BTN_UP | BTN_RIGHT, BTN_RIGHT, BTN_RIGHT | BTN_DOWN,
    BTN_DOWN, BTN_LEFT | BTN_DOWN, BTN_LEFT, BTN_LEFT | BTN_UP
};

// LEFT, CENTER, RIGHT, CENTER
static const s32 targetPosAdjustFactor[NUM_PLAYERS] = {FP(-0.25), FP(0), FP(0.25), FP(0)};

// 22.5 to 196.875 degrees
static const s32 angleDistortionData[MAX_BOT_QUALITY] = {
    0x100, 0x140, 0x180, 0x1C0,
    0x200, 0x240, 0x280, 0x2C0,
    0x300, 0x340, 0x380, 0x3C0,
    0x400, 0x440, 0x480, 0x4C0,
    0x500, 0x540, 0x580, 0x5C0,
    0x600, 0x640, 0x680, 0x6C0,
    0x700, 0x740, 0x780, 0x7C0,
    0x800, 0x840, 0x880, 0x8C0,
};

/*
    @brief: Selects the next player that will be target by the bot
    @address: 0x800b84b8
    @params:
        @playerID
    @return: enemyID
*/
static s32 PickEnemy(s32 playerID)
{
    s32 totalPublicEnemyFactor = 0;
    s32 teamID = _playerMetadata[playerID].teamID;
    for (s32 i = 0; i < NUM_PLAYERS; i++)
    {
        if ((i != playerID) && (i != teamID) && ((_playerMetadata[i].flags & 0xFA00) == 0)) // TODO: figure out enum flags
        {
            totalPublicEnemyFactor += _botDifficulty[i].publicEnemyFactor;
        }
    }

    if (totalPublicEnemyFactor == 0) return NO_PLAYER;

    s32 randEnemyFactor = Rand(totalPublicEnemyFactor);
    s32 combinedEnemyFactor = 0;
    for (s32 i = 0; i < NUM_PLAYERS; i++)
    {
        if ((i != playerID) && (i != teamID) && ((_playerMetadata[i].flags & 0xFA00) == 0)) // TODO: figure out enum flags
        {
            combinedEnemyFactor += _botDifficulty[i].publicEnemyFactor;
            if (randEnemyFactor < combinedEnemyFactor) return i;
        }
    }

    return NO_PLAYER;
}

static inline u32 TestBallCrossedLine(s32 prevPos, s32 nextPos, s32 p1, s32 p2)
{
    if (((prevPos < p1) && (prevPos < p2) && (nextPos < p1) && (nextPos < p2)) ||
        ((prevPos > p1) && (prevPos > p2) && (nextPos > p1) && (nextPos > p2)))
    {
        return false;
    }
    return true;
}

/*
    @brief: Calculates whether the ball will cross the line in the future, and if so, calculates the intersection coordinates
    @address: 0x800b7c60 and 0x800b808c
    @params:
        @playerID: used to get what line to use in the calculations
        @pBallPos
        @pPredictedPos: pointer to an empty Vec3 that will contain the intersection coordinates
        @ballAngle: angle that the function will assume the ball is going
        @pLines: pointer to array[NUM_PLAYERS] of Rects describing the coordinates each line for each player
    @return: bool

    Author notes: this function is originally split as two separated functions in the original game. However,
    the functions are virtually the same, with the exception of the lines used in the calculation. Therefore I've
    decided to merge these two functions by adding the line as a 5th parameter.
*/
static u32 PredictBallCrossingLine(s32 playerID, Vec3 * pBallPos, Vec3 * pPredictedPos, s32 ballAngle, const RectPoints * pLines)
{
    s32 cornerIndex;
    if (playerID == _Ball_PLAYER_1_ID)
    {
        cornerIndex = PLAYER_1;
    }
    if (playerID == _Ball_PLAYER_2_ID)
    {
        cornerIndex = PLAYER_2;
    }
    if (playerID ==  _Ball_PLAYER_3_ID)
    {
        cornerIndex = PLAYER_3;
    }
    if (playerID == _Ball_PLAYER_4_ID)
    {
        cornerIndex = PLAYER_4;
    }

    RectPoints line = pLines[cornerIndex];
    s32 angleDistortionFactor = Max(MAX_BOT_QUALITY - (_botDifficulty[playerID].quality + _Ball_defaultChallengeQuality), 0);
    s32 angleDistortion = angleDistortionData[angleDistortionFactor];
    s32 predictedDistortion = Rand(angleDistortion);
    s32 predictedAngle = ballAngle + (predictedDistortion - angleDistortion);
    pPredictedPos->x = pBallPos->x - SIN(predictedAngle);
    pPredictedPos->z = pBallPos->z - COS(predictedAngle);

    if ((!TestBallCrossedLine(pBallPos->x, pPredictedPos->x, line.x1, line.x2)) ||
        (!TestBallCrossedLine(pBallPos->z, pPredictedPos->z, line.z1, line.z2)))
    {
        return false;
    }

    /*
        The idea of this code is to figure out the coordinates that the ball will intersect with the line.
        Let the ball coordinates be (x1_b, y1_b) and the predicted ball coordinates be (x2_b, y2_b), and the
        line coordinates be (x1_l, y1_l), (x2_l, y2_l). Then we can write a first degree function to predict the ball movement:

        f(x) = ax + b where:

        a(x1_b) + b = (y1_b)
        a(x2_b) + b = (y2_b)

        Doing some algebra manipulations, we conclude that:

        a = ((y1_b) - (y2_b)) / ((x1_b) - (x2_b))
        b = (y1_b) - a(x2_b)

        To simplify terms, lets call y1_b - y2_b = delta_yb and x1_b - x2_b = delta_xb, therefore:

        a = delta_yb / delta_xb
        b = (y1_b) - a(x1_b)

        We want to find the intersection points (x_c, y_c). Case 1: assume that y1_l = y2_l (P1 or P2), therefore
        y_c = y1_l.

        we have that:

        y1_l = a(x_c) + b <==> x_c = (y1_l - b) / a

        x_c = (y1_l + a(x1_b) - y1_b) / a

        x_c = (y1_l + ((delta_yb / delta_xb) * (x1_b)) - y1_b) / (delta_yb / delta_xb)

        x_c = (y1_l * delta_xb) + (delta_yb * x1_b) - (y1_b * delta_xb) / (delta_yb)

        x_c = x1_b + ((delta_xb * (y1_l - y1_b)) / delta_yb)

        Case 2: x1_l = x2_l (P3 or P4), therefore x_c = x1_l.

        y_c = a(x_c) + b

        y_c = ((delta_yb / delta_xb) * x1_l) + (y1_b - ((delta_yb / delta_xb) * (x1_b)))

        y_c = y1_b + ((delta_yb * (x1_l - x1_b)) / delta_xb)

        Notice that both expressions are very similar, so we can use some tricks to combine them into one.
        Without loss of generalization, lets work on the x axis:
        * if x1_l = x2_l (i.e x2_l - x1_l = 0), we want x_c = x1_l;
        * if y1_l = y2_l, we want x_c =  x1_b + ((delta_xb * (y1_l - y1_b)) / delta_yb);
            Lets call x_c' = x1_b + ((delta_xb * (y1_l - y1_b)) / delta_yb)

        Since x2_l - x1_l = 0, we can make that if expression into one mathematical expression:

        x_c = x1_l + ((x2_l - x1_l) * ((x_c' - x1_l) / (x2_l - x1_l)))

        let X_C = (x_c' - x1_l) / (x2_l - x1_l)

        X_C = (x1_b + ((delta_xb * (y1_l - y1_b)) / delta_yb) - x1_l) / (x2_l - x1_l)

        X_C = (((delta_yb * (x1_b - x1_l)) + (delta_xb * (y1_l - y1_b))) / delta_yb) / (x2_l - x1_l)

        X_C = ((delta_yb * (x1_b - x1_l)) + (delta_xb * (y1_l - y1_b)) / (delta_yb * delta_xl)

        .*. Y_C = ((delta_xb) * (y1_b - y1_l) + (delta_yb) * (x1_l - x1_b)) / (delta_xb * delta_yl)

        We can re-arrange the terms so that:

        X_C = ((delta_yb * (x1_l - x1_b)) - ((delta_xb * (y1_l - y1_b)) / -(delta_yb * delta_xl)
        Y_C = ((delta_yb * (x1_l - x1_b)) - ((delta_xb * (y1_l - y1_b)) / (delta_xb * delta_yl)

        Notice that both terms are identical, except for the divisors.
        We know that if delta_xl != 0, then delta_yl = 0 (and vice-versa), therefore:

        K_C = ((delta_yb * (x1_l - x1_b)) - ((delta_xb * (y1_l - y1_b)) / ((delta_xb * delta_yl) - (delta_yb * delta_xl))
        where K yields 0 for the wrong axis, and yields the correct value for the correct axis.

        now we have:

        x_c = x1_l + (delta_xl * K_C)
        y_c = y1_l + (delta_yl * K_C)

        We know the intersection point for an infinitely long line; however, we want to clamp
        the calculation for our goal.
        For simplicity sake, lets call the nominator and denominator of K_C by n, d:
        Also note that, by construction: x2_l > x1_l and y2_l > y1_l ==> delta_kl >= 0, therefore:

        k2_l >= k_c >= k1_l, so we CANNOT have:
            k_c < k1_l:
                k1_l + (delta_kl * K_C) < k1_l
                (delta_kl * K_C) < 0:
                    K_C < 0
                    n / d < 0:
                        n > 0, if d < 0
                        OR
                        n < 0, if d > 0
            k2_l < k_c:
                k2_l < k1_l + (delta_kl * K_C)
                delta_kl < delta_kl * K_C
                K_C > 1:
                    n / d > 1
                    n > d, if d > 0
                    n < d, if d < 0

        Finally, we need to make sure that our ball is hitting the correct goal, and
        the ball isn't already past our goal.

        Without loss of generality,
        assume that the ball is going UP, therefore:

        y2_b > y1_l
        y2_b - y1_b > y1_l - y1_b

        Doing a similar math trick, we want to cancel the comparison if the goal line is P3 or P4:

        (y2_b - y1_b) * delta_xl > (y1_l - y1_b) * delta_xl

        Similarly, the result is inverted if the ball is going DOWN.
        Note that the left side of the equation is exactly our denominator of K_C inverted,
        therefore we can construct:

        line_comparison = ((y1_l - y1_b) * delta_xl) - ((x1_l - x1_b) * delta_yl)

        And use this expression to compare against our denominator in order to
        figure out whether the line hit the correct goal.

        if d > 0:
            delta_xb * delta_yl > delta_yb * delta_xl:
                delta_yl = 0, y2_b > y1_b, ball going towards UP goal
                OR
                delta_xl = 0, x1_b > x2_b, ball going towards LEFT goal

        By congruence, if d < 0, ball goes either towards the DOWN or RIGHT goal, therefore:

        if d > 0: (UP or LEFT)
            if line_comparison > d, then:
                y2_b < y1_l (Ball wasn't predicted to go past the goal)
                OR
                x2_b > x1_l (Ball wasn't predicted to go past the goal)
            if line_comparison < 0:
                y1_b > y1_l (Ball started inside the goal)
                OR
                x1_b < x1_l (Ball started inside the goal)

        Similar argument explains the case when d < 0
    */

    /*
        For some reason, the game does (>> 4) operations after each subtraction step instead of only doing
        (>> 8) in the FP_Mult step. The final result should be the same (x / 16) * (y / 16) = (x * y) / 256;
        however due to integer math precision, doing two divisions by 16 instead of only one by 256 causes
        loss of precision. Commented below is a better and more precise way of doing the FP math:

        s32 deltaXBall = pBallPos->x - pPredictedPos->x;
        s32 deltaZBall = pBallPos->z - pPredictedPos->z;
        s32 deltaXLine = line.x2 - line.x1;
        s32 deltaZLine = line.z2 - line.z1;
        s32 distXLineBall = line.x1 - pBallPos->x;
        s32 distZLineBall = line.z1 - pBallPos->z;
        s32 coordNominator = FP_MULT(deltaZBall, distXLineBall) - FP_MULT(deltaXBall, distZLineBall);
        s32 coordDenominator = FP_MULT(deltaXBall, deltaZLine) - FP_MULT(deltaZBall, deltaXLine);
        s32 lineComparison = FP_MULT(deltaXLine, distZLineBall) - FP_MULT(deltaZLine, distXLineBall);
    */

    s32 deltaXBall = (pBallPos->x - pPredictedPos->x) >> 4;
    s32 deltaZBall = (pBallPos->z - pPredictedPos->z) >> 4;
    s32 deltaXLine = (line.x2 - line.x1) >> 4;
    s32 deltaZLine = (line.z2 - line.z1) >> 4;
    s32 distXLineBall = (line.x1 - pBallPos->x) >> 4;
    s32 distZLineBall = (line.z1 - pBallPos->z) >> 4;
    s32 coordNominator = (deltaZBall * distXLineBall) - (deltaXBall * distZLineBall);
    s32 coordDenominator = (deltaXBall * deltaZLine) - (deltaZBall * deltaXLine);
    s32 lineComparison = (deltaXLine * distZLineBall) - (deltaZLine * distXLineBall);

    if (coordDenominator == 0) return false; // division by 0

    if (coordDenominator > 0) // ball going towards UP or LEFT goals
    {
        if (coordNominator < 0) return false; // intersection < line.x1 or line.z1

        if (coordDenominator < coordNominator) return false; // intersection > line.x2 or line.z2

        if (lineComparison < 0) return false; // ball started inside the goal

        if (coordDenominator < lineComparison) return false; // Ball wasn't predicted to go past the goal
    }
    else
    {
        if (coordNominator > 0) return false; // intersection < line.x1 or line.z1

        if (coordDenominator > coordNominator) return false; // intersection > line.x2 or line.z2

        if (lineComparison > 0) return false; // ball started inside the goal

        if (coordDenominator > lineComparison) return false; // Ball wasn't predicted to go past the goal
    }
    s32 temp = FP_DIV(coordNominator, coordDenominator);
    pPredictedPos->x = line.x1 + FP_MULT(line.x2 - line.x1, temp);
    pPredictedPos->z = line.z1 + FP_MULT(line.z2 - line.z1, temp);
    return true;
}

/*
    @brief: Calculates whether playerID should use its attack on enemyID
    @address: 0x800b85dc
    @params:
        @playerID
        @enemyID
        @maxDistance: attack radius?
        @minAngle: used to compare with the angle that ball will be shot
        @maxAngle: used to compare with the angle that ball will be shot
        @isAggro: bool
    @return: bool
*/
static u32 ShouldShootBall(s32 playerID, s32 enemyID, s32 maxDistance, s32 minAngle, s32 maxAngle, u32 isAggro)
{
    PlayerMetadata pm = _playerMetadata[playerID];
    Object * pPlayer = pm.pPlayer;
    Vec3 predictedPos;
    if (_levelID == LEVELID_SKY_BALLS)
    {
        // TODO
    }
    if (pm.attackCooldown) return false;

    EntityLinkedList * pBallList = _pBallLinkedList;
    EntityLinkedList * pSelectedBall = nullptr;
    while (pBallList)
    {
        Object * pBallObject = pBallList->pEntityObject;
        Entity * pBall = pBallList->pEntity;
        if ((pBallObject) && (pBall) && (pBall->color != RED_BALL))
        {
            s32 xDist = pPlayer->pos.x - pBallObject->pos.x;
            s32 zDist = pPlayer->pos.z - pBallObject->pos.z;
            if (((xDist * xDist) + (zDist * zDist)) < (maxDistance * maxDistance))
            {
                s32 distAngle = _ArcTan(xDist, zDist);
                s32 shotAngle = SubtractAngles(distAngle, pm.pPlayerPhysics->angle);
                if (enemyID == NO_PLAYER)
                {
                    if ((shotAngle > minAngle) && (shotAngle < maxAngle))
                    {
                        pSelectedBall = pBallList;
                    }
                }
                else
                {
                    if (_GetStructure(pBallList, 0x800))
                    {
                        pSelectedBall = pBallList;
                        if (PredictBallCrossingLine(enemyID, &pBallObject->pos, &predictedPos, distAngle, attackLine)) return true;
                    }
                }
            }
        }
        pBallList = pBallList->next;
    }
    if (pSelectedBall == nullptr) return false;

    if (isAggro)
    {
        if (_playersCampaign != SOLO)
        {
            Object * pBallObject = pSelectedBall->pEntityObject;
            s32 xDist = pPlayer->pos.x - pBallObject->pos.x;
            s32 zDist = pPlayer->pos.z - pBallObject->pos.z;
            s32 distAngle = _ArcTan(xDist, zDist);
            if ((_GetStructure(pBallList, 0x800) == nullptr) ||
                (PredictBallCrossingLine(pm.teamID, &pBallObject->pos, &predictedPos, distAngle, attackLine))) return false;
        }
        return true;
    }
    return false;
}

static inline void BotChangeStateDefend(Bot * pBot)
{
    pBot->state = BOTSTATE_DEFEND;
    pBot->field6_0x22 = 0;
}

static inline void BotSetDirectionalInputs(Bot * pBot, s32 targetAngle)
{
    pBot->targetAngle = targetAngle;
    pBot->nextInputs |= inputDirectionAngle[pBot->targetAngle >> 9]; // ang / 45
}

static inline void BotShootBall(Bot * pBot)
{
    pBot->enemyID = NO_PLAYER;
    pBot->nextInputs |= BTN_SQUARE;
}

/*
    @brief: Bot object logic, runs once per frame for each bot
    @address: 0x800b8a68
    @params:
        @playerID
*/
void Bot_onUpdate(s32 playerID)
{
    if (_gameFlags & 0x4000000) return;

    PlayerMetadata pm = _playerMetadata[playerID];
    Object * pPlayer = pm.pPlayer;
    Bot * pBot = pm.pBot;
    pBot->nextInputs = BTN_NONE;

    if (pBot->tauntCooldown)
    {
        pBot->tauntCooldown--;
    }
    if (pBot->aggroCooldown)
    {
        pBot->aggroCooldown--;
    }
    if (pBot->magnetTimer)
    {
        pBot->magnetTimer--;
    }
    if (pBot->field9_0x28)
    {
        pBot->field9_0x28--;
    }
    if (pBot->field12_0x30)
    {
        pBot->field12_0x30--;
    }

    if ((pm.action == _unk_8009e0d8) || (pm.action ==  _unk_8009e0e2))
    {
        pBot->tauntCooldown = SECONDS(1/6);
        pBot->state = BOTSTATE_DO_NOTHING;
    }

    if (pBot->enemyID == NO_PLAYER)
    {
        pBot->enemyID = PickEnemy(playerID);
    }

    EntityLinkedList * pBallList = _pBallLinkedList;
    Vec3 predictedBallPos;
    s32 xTarget = 0;
    s32 zTarget = 0;
    s32 minDistSquared = MAX_DIST;
    while (pBallList)
    {
        Object * pBallObject = pBallList->pEntityObject;
        Entity * pBall = pBallList->pEntity;
        if ((pBallObject) && (pBall) && (pBall->state == 1)) // TODO: figure out ball->state and make it an Enum
        {
            EntityPhysics * pBallPhysics = _GetStructure(pBallList, 0x800);
            if ((pBallPhysics) &&
                (PredictBallCrossingLine(playerID, &pBallObject->pos, &predictedBallPos, pBallPhysics->angle, goalLine)))
            {
                s32 distSquared = ((pBallObject->pos.x - predictedBallPos.x) * (pBallObject->pos.x - predictedBallPos.x)) + ((pBallObject->pos.z - predictedBallPos.z) * (pBallObject->pos.z - predictedBallPos.z));
                if (distSquared < minDistSquared)
                {
                    minDistSquared = distSquared;
                    xTarget = predictedBallPos.x;
                    zTarget = predictedBallPos.z;
                }
            }
        }
        pBallList = pBallList->next;
    }

    if (pBot->enemyID != NO_PLAYER)
    {
        s32 oppositeBot = PLAYER_1;
        if (playerID == _Ball_PLAYER_1_ID)
        {
            oppositeBot = PLAYER_2;
        }
        else if (playerID == _Ball_PLAYER_3_ID)
        {
            oppositeBot = PLAYER_4;
        }
        else if (playerID == _Ball_PLAYER_4_ID)
        {
            oppositeBot = PLAYER_3;
        }

        s32 i = 3;
        if (pBot->enemyID == *playerAdjacencyList[oppositeBot].pRightPlayerID)
        {
            i = 0;
        }
        else if (pBot->enemyID == *playerAdjacencyList[oppositeBot].pMiddlePlayerID)
        {
            i = 1;
        }
        else if (pBot->enemyID == *playerAdjacencyList[oppositeBot].pLeftPlayerID)
        {
            i = 2;
        }

        s32 posAdjustFactor = targetPosAdjustFactor[i];
        if (playerID == _Ball_PLAYER_1_ID)
        {
            xTarget += posAdjustFactor;
        }
        else if (playerID == _Ball_PLAYER_2_ID)
        {
            xTarget -= posAdjustFactor;
        }
        else if (playerID == _Ball_PLAYER_3_ID)
        {
            xTarget -= posAdjustFactor; // Probably a typo, should be zTarget += posAdjustFactor
        }
        else if (playerID == _Ball_PLAYER_4_ID)
        {
            zTarget -= posAdjustFactor;
        }
    }

    if (_levelID == LEVELID_N_BALLISM)
    {
        // TODO
    }

    s32 minDist = 0;
    s32 targetAngle;
    if (minDistSquared == MAX_DIST)
    {
        targetAngle = _ArcTan(pBot->targetPos.x - pPlayer->pos.x, pBot->targetPos.z - pPlayer->pos.z);
    }
    else
    {
        minDist = SQRT_SQ(minDistSquared);
        targetAngle = _ArcTan(xTarget - pPlayer->pos.x, zTarget - pPlayer->pos.z);
    }

    if (pBot->magnetTimer)
    {
        // TODO
    }

    switch(pBot->state)
    {
        case BOTSTATE_DEFEND:
            s32 botQuality = _botDifficulty[playerID].quality;
            s32 qualityDist = Max((botQuality * 16) - _Ball_defaultChallengeQuality, 1);
            s32 randDist = qualityDist + Rand(qualityDist) + FP(1);
            if (_levelID == LEVELID_BEACH_BALL)
            {
                // TODO
            }
            else
            {
                if (pBot->enemyID != NO_PLAYER)
                {
                    u32 isAggro = false;
                    if (pBot->aggroCooldown == 0)
                    {
                        s32 randQuality = Rand(MAX_BOT_QUALITY);
                        if (randQuality > botQuality)
                        {
                            pBot->aggroCooldown = MAX_BOT_QUALITY + ((MAX_BOT_QUALITY - botQuality) / 2);
                        }
                        else
                        {
                            isAggro = true;
                        }
                    }
                    if (_noHumansLeft == false)
                    {
                        isAggro = true;
                    }
                    if (ShouldShootBall(playerID, pBot->enemyID, randDist, ANG(-45), ANG(45), isAggro))
                    {
                        if ((!(_minigameMode & MINIGAME_CRYSTAL)) || (_Ball_unk_800c1f0c))
                        {
                            BotShootBall(pBot);
                        }
                        else
                        {
                            pBot->nextInputs |= BTN_CIRCLE;
                        }
                    }
                    else
                    {
                        if (pBot->aggroCooldown)
                        {
                            pBot->aggroCooldown--;
                        }
                    }
                }
            }
            if (minDist)
            {
                s32 targetDist = pPlayer->pos.z - zTarget;
                if ((playerID == _Ball_PLAYER_1_ID) || (playerID == _Ball_PLAYER_2_ID))
                {
                    targetDist = pPlayer->pos.x - xTarget;
                }
                targetDist = Abs(targetDist);
                s32 noiseDist = Abs(targetDist + ((botQuality + _Ball_defaultChallengeQuality) * MAX_BOT_QUALITY));
                if (noiseDist > FP(2.5))
                {
                    if (noiseDist > minDist)
                    {
                        pBot->nextInputs |= BTN_SHOULDERS;
                    }
                    BotSetDirectionalInputs(pBot, targetAngle);
                }
            }
            else
            {
                pBot->state = BOTSTATE_SET_TARGET;
            }
            break;
        case BOTSTATE_FOLLOW_TARGET:
            if (minDist == 0)
            {
                s32 xDist = pPlayer->pos.x - pBot->targetPos.x;
                s32 zDist = pPlayer->pos.z - pBot->targetPos.z;
                s32 dist = FP_DIST(xDist, zDist);
                if (dist >= FP(0.5))
                {
                    BotSetDirectionalInputs(pBot, targetAngle);
                }
                else
                {
                    pBot->tauntCooldown = Rand(SECONDS(2)) + SECONDS(1); // max 3s of cooldown
                    pBot->state = BOTSTATE_TAUNT;
                }
            }
            else
            {
                BotChangeStateDefend(pBot);
            }
            break;
        case BOTSTATE_TAUNT:
            if (minDist == 0)
            {
                if (pBot->tauntCooldown == 0)
                {
                    pBot->state = BOTSTATE_WAIT;
                    pBot->nextInputs |= BTN_TRIANGLE;
                }
            }
            else
            {
                BotChangeStateDefend(pBot);
            }
            break;
        case BOTSTATE_DO_NOTHING:
            break;
        case BOTSTATE_WAIT:
            if (minDist == 0)
            {
                if (pm.action != ACTION_TAUNT)
                {
                    pBot->state = BOTSTATE_SET_TARGET;
                }
            }
            else
            {
                BotChangeStateDefend(pBot);
            }
            break;
        default: // Wander around in the center of the map
            pBot->targetPos = pPlayer->pos;
            if ((playerID == _Ball_PLAYER_1_ID) || (playerID == _Ball_PLAYER_2_ID))
            {
                pBot->targetPos.x = Rand(FP(4)) - FP(2);
            }
            else
            {
                pBot->targetPos.z = Rand(FP(4)) - FP(2);
            }
            pBot->state = BOTSTATE_FOLLOW_TARGET;
            break;
    }
    _SetBotInput(&_controllers[playerID], pBot->nextInputs);
    _controllers[playerID].targetAngle = pBot->targetAngle;
}