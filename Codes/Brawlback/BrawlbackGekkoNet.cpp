//
// Brawlback GekkoNet game-side hooks.
// Minimal: one hook at frame start (DMAWrite only), one hook to zero the gameProc loop count.
// Dolphin handles everything: GekkoNet session, rollback, input injection, gameProc calling.
//

#include "Brawlback.h"
#include "Netplay.h"
#include "GmGlobalModeMelee.h"

#define GKK_CMD_FRAME 20

#pragma pack(push, 1)
struct GKKFramePayload
{
    u32 frame;
    BrawlbackPad pad;
};
#pragma pack(pop)

// Per-frame hook inside beginFrameLogic().
// Sends frame + local pad to dolphin. Dolphin does ALL the work:
// GekkoNet, rollback (LoadFrame), input injection, gameProc calling, frame pacing.
SIMPLE_INJECTION(gkkBeginFrame, 0x80147394, "li r0, 0x1")
{
    if (!Netplay::IsInMatch())
        return;

    _OSDisableInterrupts();

    DEFAULT_MT_RAND->seed = 0x496ffd00;

    u32 currentFrame = getCurrentFrame();

    GKKFramePayload payload;
    payload.frame = currentFrame;
    payload.pad = GamePadToBrawlbackPad(&PAD_SYSTEM->pads[Netplay::localPlayerIdx]);

    EXIPacket(GKK_CMD_FRAME, &payload, sizeof(payload)).Send();

    _OSEnableInterrupts();
}

    INJECTION("handleFrameAdvanceHook", 0x800173a4, R"(
        bl dothinghehe # load framesToAdvance variable into r24
        cmplw r19, r24 # r24 is the number of times gameProc will be called (# of frames to simualate)
    )");
    extern "C" void dothinghehe() {
        if (!Netplay::IsInMatch()) return; // if we don't need to do anything special, let the game use it's own frame advance
        asm("li r24, 0");
        // asm("mr r24, %0"
        //     :
        //     : "r" (0)
        // );
    }

// Game's gameProc loop count is always 0 — dolphin calls gameProc directly.
// INJECTION("gkkFrameAdvanceHook", 0x800173a4, R"(
//     li r24, 0
//     cmplw r19, r24
// )");
