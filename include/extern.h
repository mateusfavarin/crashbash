#ifndef EXTERN_H
#define EXTERN_H

#include <types.h>
#include <math.h>
#include <controller.h>
#include <player.h>
#include <bot.h>
#include <ball.h>

/* In game global functions */
u32 _SubtractAngles(s32 alpha, s32 beta);
s32 _Rand(s32 range);
s32 _ArcTan(s32 x, s32 y);
s32 _CSQRT(s32 n);
AnotherBallStruct * _GetStructure(BallLinkedList *list, u32 flags);
void _SetBotInput(Controller * controller, u32 inputs);

/* Ball functions */
void _UpdateBots(s32 playerID);

/* In game global variables */
extern u32 _PRNGSeed;
extern u32 _levelID;
extern u32 _gameFlags;
extern s32 _playersCampaign;
extern u16 _minigameMode;
extern PlayerMetadata _playerMetadata[NUM_PLAYERS];
extern BotDifficulty _botDifficulty[NUM_PLAYERS];
extern BallLinkedList * _pBallLinkedList;
extern Controller _controllers[NUM_PLAYERS];
extern u16 _unk_8009e0d8;
extern u16 _unk_8009e0e2;
extern s32 _noHumansLeft;
extern TrigTable _trigTable[0x1000];

/* Ball variables */
extern s16 _defaultChallengeQuality;
extern u16 _unk_800c1f0c;
extern s32 _PLAYER_1_ID;
extern s32 _PLAYER_2_ID;
extern s32 _PLAYER_3_ID;
extern s32 _PLAYER_4_ID;

#endif