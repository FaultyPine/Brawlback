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
#include "EXIPacket.h"
#include "Utility/Utility.h"


#define MAX_ROLLBACK_FRAMES 7

struct FrameData {
    u32 frame;
    u32 randomSeed;
    gfPadGamecube pads[4];
}; //__attribute__((packed, aligned(4)));





#endif