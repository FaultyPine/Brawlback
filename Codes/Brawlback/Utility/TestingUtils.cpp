#include "TestingUtils.h"




// ============ Always loads falco ==================
/*
//                               fighter  slot  css   cosmetic
static int hardcodedCharIDs[4] = { 0x13, 0x15, 0x13, 0x12 }; // falco

// override convertKind (end of func)
INJECTION("convertKindOverrideEndHook", 0x8085467c, R"(
    SAVE_REGS
    bl convertKindEnd
    RESTORE_REGS
    addi r1, r1, 32
)");
extern "C" void convertKindEnd(int p1, int* p2) {
    //u32 FighterID = p2[0];
    //u32 SlotID = p2[?];
    //u32 CosmeticID = p2[?];
    //u32 CSSID = p2[?];
    for (int i = 0; i < 4; i++) {
        write32((u32)&p2[i], hardcodedCharIDs[i]); 
    }
}
*/