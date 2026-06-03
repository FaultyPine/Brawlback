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


// Skip nw4r effect system update during resim frames.
// field106_0xed bit 0 gates three things: effect system, gfSlowManager, gfTaskScheduler.
// We clear bit 0 only in r0 (register), NOT in memory, so only the effect system
// check is skipped while the two subsequent lbz reloads see the real field value.
#if 0
INJECTION("EffectSystemRollbackSkip", 0x8001773c, R"(
    SAVE_REGS
    bl shouldSkipUnnecessarySystems
    cmpwi r3, 0
    beq NOT_RESIM
    RESTORE_REGS
    lbz r0, 0x00ED (r30)
    rlwinm r0, r0, 0, 0, 0x1e
    b EFFECT_CHECK_DONE
    NOT_RESIM:
    RESTORE_REGS
    lbz r0, 0x00ED (r30)
    EFFECT_CHECK_DONE:
)");
extern "C" bool shouldSkipUnnecessarySystems() {
    return FrameAdvance::IsResimFrame();
}
#endif


// Skip ph2ndaryWorld (cloth/hair secondary physics) during resim frames.
// ph2ndaryMain gates all work on controller->field_0x20 (the enable flag) via
// lbz r0,0x20(r29) / cmpwi r0,0 / beq <end>
#if 0
INJECTION("Ph2ndaryWorldRollbackSkip", 0x8008b450, R"(
    SAVE_REGS
    bl shouldSkipUnnecessarySystems
    cmpwi r3, 0
    beq NOT_RESIM_PH2
    RESTORE_REGS
    li r0, 0
    b PH2NDARY_DONE
    NOT_RESIM_PH2:
    RESTORE_REGS
    lbz r0, 0x20 (r29)
    PH2NDARY_DONE:
)");
#endif


// ~~~~~~~~~ RUMBLE ~~~~~~~~~~~~~~~

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