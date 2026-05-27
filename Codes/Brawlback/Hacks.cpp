#include "Brawlback.h"

#if 0
INJECTION("cameraRollbackBypass3", 0x8009fee4, R"(
    rlwinm. r0, r0, 0x1c, 0x1f, 0x1f
    bl cameraFFBypass
)");
extern "C" void cameraFFBypass() {
    if (FrameAdvance::getFramesToAdvance() > 1) {
        // if we are fast forwarding, skip camera update
        // forces branch at 8009fee8
        asm(R"(
            li r0, 0x0
            cmpwi r0, 0x0
        )");
    }
}
#endif


// TODO: skip effects processing during resim frames
// lbz	r0, 0x00ED (r30)
#if 0
INJECTION(EffectSystemRollbackSkip, 0x8001773c, R"(
    SAVE_REGS
    bl shouldSkipEffectSystem
    cmpwi r3, 0
    beq RUN_EFFECT_SYSTEM
    li r0, 0
    stb r0, 0x00ED (r30)
    RUN_EFFECT_SYSTEM:
    RESTORE_REGS

    lbz	r0, 0x00ED (r30)
)");
extern "C" bool shouldSkipEffectSystem() {
    return FrameAdvance::IsResimFrame();
}
#endif


// ~~~~~~~~~ RUMBLE ~~~~~~~~~~~~~~~
#if 0
// updateGame/[gfPadSystem]
INJECTION("PadRumbleInterpretNop", 0x8002a520, "nop");


// removeRumbleMask/[IpHuman]
INJECTION("PadRumbleRemoveMaskNop", 0x80048500, "nop");

// frameStartInit/[gfApplication]
INJECTION("PadRumbleActivateAllNop", 0x800175b4, "nop");

// removeRumbleId/[IpHuman]
INJECTION("PadRumbleRemoveIDNop", 0x800484cc, "nop");

// setRumble/[IpHuman]
INJECTION("lbSetRumbleNop", 0x80048430, "nop");
#endif