#ifndef __BRAWLBACK
#define __BRAWLBACK

#include "Assembly.h"
#include "Debug.h"
#include "Wii/EXI/EXI.h"
#include "Memory.h"

#include "Wii/PAD/PADStatus.h"
#include "Brawl/GF/GameFrame.h"
#include "Brawl/GF/gfPadSystem.h"
#include "Brawl/FT/ftManager.h"
#include "Brawl/gmGlobalModeMelee.h"
#include "Wii/mtRand.h"
#include "EXIPacket.h"
#include "Utility/Utility.h"



#define MAX_ROLLBACK_FRAMES 7

#define NAMETAG_SIZE 8
#define DISPLAY_NAME_SIZE 31
#define CONNECT_CODE_SIZE 10

struct FrameData {
    u32 frame;
    u32 randomSeed;
    gfPadGamecube pads[4];
}; //__attribute__((packed, aligned(4)));

enum PlayerType : u8
{
    PLAYERTYPE_LOCAL = 0x0,
    PLAYERTYPE_REMOTE = 0x1,
};

struct PlayerSettings
{
    u8 charID;
    u8 charColor;
    PlayerType playerType;
    u8 controllerPort;
    u16 nametag[NAMETAG_SIZE];
    u8 displayName[DISPLAY_NAME_SIZE];
    u8 connectCode[CONNECT_CODE_SIZE];
};

struct GameSettings
{
    u16 stageID;
    u32 randomSeed;
    PlayerSettings playerSettings[4];
};


void fillOutGameSettings(GameSettings* settings);
void MergeGameSettingsIntoGame(GameSettings* settings);

#endif