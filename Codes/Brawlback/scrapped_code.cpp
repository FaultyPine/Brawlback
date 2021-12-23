 // dumping grounds





// override convertKind (beginning of func)
INJECTION("convertKindOverrideBeginning", 0x808545ec, R"(
    li r3, 1
    lis	r5, 0x80AE
)");

extern "C" void convertKindBeginning(int p1, int* p2) {
    asm(R"(
        li r3, 1
    )"
    : 
    : 
    );
}




/*

0x8062131c --- init/[ftEntry] -> _ct/[ftInput] -> ct for some ai related stuff

0x80623320 --- init/[ftEntry] -> seems to set the char id for players 1-4 (8082bd60 in ghidra server)

0x80624818 --- init/[ftSlot] (called on css) -> (80835488 in ghidra server)

0x8128bd30 --- same as top

*/


// init/[ftSlot]
INJECTION("setCharid", 0x80827490, R"(
    li r4, 1
    stw	r4, 0x0160 (r3)
)");


// init/[ftEntry]

// overwrites local var
INJECTION("setCharidEntry", 0x8081dcf8, R"(
    lwz	r22, 0x0008 (r6)
    li r22, 1
)");

// overwrites struct field
INJECTION("setCharidEntry2", 0x8081ddd4, R"(
    lwz	r0, 0 (r28)
    li r0, 1
)");
















INJECTION("clearGeneralTermFix", 0x807821dc, R"(
    bl clearGeneralTermInfLoopFix
    bge+ -0x4C
)");
extern "C" void clearGeneralTermInfLoopFix() {
    asm("cmpwi 0x0, %0" : : "r" (isLoadedState));
}


INJECTION("clearGeneralTermFix2", 0x807821d8, R"(
    bl clearGeneralTermFix2Func

    beq NOT_LOADED_STATE

    li r8, -0x1

    NOT_LOADED_STATE:

    cmpwi r8, 0
)");
extern "C" void clearGeneralTermFix2Func() {
    asm(R"(
        cmpwi %0, 0x0
        
    )"
    :
    : "r" (isLoadedState)
    );
}


SIMPLE_INJECTION(overrideThreadStall, 0x801e0bd0, "cmpwi r0, 0x0") {
    unsigned long test = 0;
    asm("mr %0, r0"
        :
        : "r" (test)
    );
    OSReport("Thread stall varaible: %u\n", test);
}
INJECTION("overrideThreadStallInj", 0x801e0bd0, R"(
    li r0, 1
    cmpwi r0, 0x0
)");


INJECTION("gfTaskSchedulerFix", 0x8002e66c, R"(
    bl gfTaskSchedulerFixFunc
    bne+ -0x90
)");
extern "C" void gfTaskSchedulerFixFunc() {
    asm(R"(

        cmpwi %0, 0x1
        beq AFTER_BRANCH
        
        cmpwi r27, 0x0

        AFTER_BRANCH:

        blr
    )"
    :
    : "r" (isLoadedState)
    );
}

// hsd_padrumble inf loop
INJECTION("rumbleInfLoopFix", 0x8002c4dc, "nop")