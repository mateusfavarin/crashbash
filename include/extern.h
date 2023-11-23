#ifndef EXTERN_H
#define EXTERN_H

#include <types.h>
#include <math.h>
#include <gpu.h>
#include <controller.h>
#include <player.h>
#include <bot.h>
#include <entity.h>

/* In game global functions */
s32 _ArcTan(s32 x, s32 y);
s32 _CSQRT(s32 n);
EntityPhysics * _GetStructure(EntityLinkedList *list, u32 flags);
void _SetBotInput(Controller * controller, u32 inputs);

/* In game global variables */
extern u32 _darknessLevel;
extern OrderingTable * _pOT;
extern OrderingTable * _pOTCopy;
extern Rect * _pFrameBuffer;
extern s32 _g_xOffset;
extern s32 _g_yOffset;
extern u16 _zIndexOffset;
extern u16 _maxZIndex;
extern u32 (* _FUNC_Player_onCollide)(Object *);
extern u32 _levelID;
extern u32 _gameFlags;
extern s32 _playersCampaign;
extern u16 _minigameMode;
extern PlayerMetadata _playerMetadata[NUM_PLAYERS];
extern BotDifficulty _botDifficulty[NUM_PLAYERS];
extern EntityLinkedList * _pBallLinkedList;
extern Controller _controllers[NUM_PLAYERS];
extern u16 _unk_8009e0d8;
extern u16 _unk_8009e0e2;
extern s32 _noHumansLeft;
extern TrigTable _trigTable[0x1000];

#endif