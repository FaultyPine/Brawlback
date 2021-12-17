 // dumping grounds



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