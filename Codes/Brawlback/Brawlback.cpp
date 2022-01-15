#include "Brawlback.h"
#include "Netplay.h"

#define NETPLAY_IMPL 1
#define ROLLBACK_IMPL 1



INJECTION("gfCreateModuleHook", 0x800266c4, R"(
    SAVE_REGS
    bl createModuleTest
    RESTORE_REGS
)");
extern "C" void createModuleTest() {
    
}




STARTUP(startupNotif) {
    OSReport("~~~~~~~~~~~~~~~~~~~~~~~~ Brawlback ~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

u32 getCurrentFrame() {
    return GAME_FRAME->persistentFrameCounter;
}

u32 naiveChecksum(unsigned char* p, int sizeOfObj) {
    unsigned int sum = 0;
    for (int i=0; i< sizeOfObj; i++) {
        sum += p[i];
    }
    return sum;
}

bool compareInputs(gfPadGamecube* p1, gfPadGamecube* p2) {
    bool buttons = p1->buttons.bits == p2->buttons.bits;
    bool triggers = p1->LTrigger == p2->LTrigger && p1->RTrigger == p2->RTrigger;
    bool analogSticks = p1->stickX == p2->stickX && p1->stickY == p2->stickY;
    bool cSticks = p1->cStickX == p2->cStickX && p1->cStickY == p2->cStickY;
    return buttons && triggers && analogSticks && cSticks;
    //u32 p1Checksum = naiveChecksum((unsigned char*)p1, sizeof(gfPadGamecube));
    //u32 p2Checksum = naiveChecksum((unsigned char*)p2, sizeof(gfPadGamecube));
    //return p1Checksum == p2Checksum;
}

// called when someone selects a character on the CSS
// r3 is "#ftSlot init [%d Color:%d Kind:%d] \n"
// r4 is the player's slot (0-3)
// r5 is the player character's color
// r6 is the player's character id
INJECTION("afterPlayerInit", 0x808277d0, R"(
    SAVE_REGS
    bl getPlayerCharData
    RESTORE_REGS
    crxor 4*cr1+eq,4*cr1+eq,4*cr1+eq
)");
extern "C" void getPlayerCharData(u32 s, int playerSlot, int playerColor, int playerCharKind) {
    OSReport("getPlayerCharData: %u %u %u\n", playerSlot, playerColor, playerCharKind);
    //Netplay::getGameSettings()->playerSettings[playerSlot].charID = playerCharKind;
    //Netplay::getGameSettings()->playerSettings[playerSlot].charColor = playerColor;
}


// fill gamesettings struct with game info
void fillOutGameSettings(GameSettings* settings) {
    settings->randomSeed = DEFAULT_MT_RAND->seed;
    settings->stageID = GM_GLOBAL_MODE_MELEE->stageID;

    // brawl loads all players into the earliest slots.
    // I.E. if players choose P1 and P3, they will get loaded into P1 and P2
    // this means we can use the number of players in a match to iterate over
    // players since we know they'll always be next to each other

    // TODO: replace this with some way to get the actual number of players in a match.
    // unfortunately FIGHTER_MANAGER->getEntryCount() isn't filled out at this point in the game loading
    // sequence. Gotta find another way to get it, or some better spot to grab the number of players
    settings->numPlayers = 2;
    OSReport("Num Players: %u\n", (unsigned int)settings->numPlayers);
}


// take gamesettings and apply it to our game
void MergeGameSettingsIntoGame(GameSettings* settings) {
    //DEFAULT_MT_RAND->seed = settings->randomSeed;
    DEFAULT_MT_RAND->seed = 0x496ffd00; // hardcoded for testing (arbitrary number)
    //OTHER_MT_RAND->seed = 0x496ffd00;

    //GM_GLOBAL_MODE_MELEE->stageID = settings->stageID;
    GM_GLOBAL_MODE_MELEE->stageID = 2;

    Netplay::localPlayerIdx = settings->localPlayerIdx;
    OSReport("Local player index is %u\n", (unsigned int)Netplay::localPlayerIdx);
}


namespace Match {
    bool isInMatch = false;
    bool IsInMatch() { return isInMatch; }


    // on scene start (AFTER the start/[scMelee] function has run)

    SIMPLE_INJECTION(startSceneMelee, 0x806d176c, "addi	sp, sp, 112") {
        _OSDisableInterrupts();
        OSReport("  ~~~~~~~~~~~~~~~~  Start Scene Melee  ~~~~~~~~~~~~~~~~  \n");
        //bool shouldNetplay = Netplay::CheckShouldStartNetplay();
        #if NETPLAY_IMPL
        Netplay::StartMatch(); // start netplay logic
        #endif
        isInMatch = true;
        _OSEnableInterrupts();
    }

    // on scene exit
    SIMPLE_INJECTION(exitSceneMelee, 0x806d4844, "li r4, 0x0") {
        _OSDisableInterrupts();
        OSReport("  ~~~~~~~~~~~~~~~~  Exit Scene Melee  ~~~~~~~~~~~~~~~~  \n");
        #if NETPLAY_IMPL
        Netplay::EndMatch();
        #endif
        isInMatch = false;
        _OSEnableInterrupts();
    }


}

namespace FrameAdvance {

    // how many game logic frames we should simulate this frame
    int framesToAdvance = 1;

    // sets the number of frames of game logic to run every frame
    void TriggerFastForwardState(u8 numFramesToFF) {
        if (framesToAdvance == 1 && numFramesToFF > 0) {
            framesToAdvance = numFramesToFF;
        }
    }
    void StallOneFrame() { 
        if (framesToAdvance == 1) {
            framesToAdvance = 0; 
        }
    }
    void SkipOneFrame() {
        if (framesToAdvance == 1) {
            framesToAdvance = 2; 
        }
    }
    void ResetFrameAdvance() { 
        if (framesToAdvance != 1) {
            OSReport("Resetting frameadvance to normal\n");
            framesToAdvance = 1; 
        }
    }

    // og instruction: cmplw r19, r24
    // # of frames to simulate is stored in r24

    // keep in mind that overriding this means that brawl's native
    // catchup system where it fast forwards a few frames when it lags is no longer active
    //     this *may*????? have unintended consequences, but I honestly don't think it will...
    INJECTION("handleFrameAdvance", 0x800173a4, R"(
        bl handleFrameAdvance
        cmplw r19, r24
    )");
    extern "C" void handleFrameAdvance() {
        asm("mr r24, %0"
            :
            : "r" (framesToAdvance)
        );
    }


    // array of inputs for each player to get injected
    PlayerFrameData* overrideInputs = nullptr;

    void InjectInputsForPlayer(gfPadGamecube* pad, u8 playerIdx) {
        memcpy(&PAD_SYSTEM->pads[playerIdx], pad, sizeof(gfPadGamecube));
    }

    // playerFrameDatas should have numPlayers # of framedatas
    void InjectInputsForAllPlayers(PlayerFrameData* playerFrameDatas) {
        for (int i = 0; i < Netplay::getGameSettings()->numPlayers; i++) {
            PlayerFrameData* playerFrameData = &playerFrameDatas[i];
            InjectInputsForPlayer(&playerFrameData->pad, playerFrameData->playerIdx);
        }
    }

    // for keeping track of the past few framedatas
    Queue<FrameData*> pastFrameDatas = Queue<FrameData*>();

    void FindAndInjectInputsForResimFrame(u32 gameLogicFrame) {
        auto node = pastFrameDatas.getNodeFront();
        // iterate through our queue of past frame datas
        while (node != nullptr) {
            //     .frame should be same for all framedatas (isn't rn... why? Fix this)
            if (node->data->playerFrameDatas[0].frame == gameLogicFrame) { // if the frame of that FrameData in the queue is the same as the current one
                // inject inputs and break out
                PlayerFrameData* playerFrameDatas = node->data->playerFrameDatas;
                InjectInputsForAllPlayers(playerFrameDatas);
                break;
            }
            node = node->next_ptr;
        }
    }

    // this is at the very beginning of the main game logic loop (right before 'gameProc'). This should be a good place to inject inputs for
    // each fast-forwarded frame
    SIMPLE_INJECTION(resimPoint, 0x80017354, "or r4, r19, r19") {
        _OSDisableInterrupts();
        // if we are currently resimulating
        if (framesToAdvance > 1) {
            u32 gameLogicFrame = getCurrentFrame();
            OSReport("Current game logic frame (in loop): %u\n", gameLogicFrame);
            if (!pastFrameDatas.empty()) {
                FindAndInjectInputsForResimFrame(gameLogicFrame);
            }
        }
        /*should this be else if? Or just if? i think else if is right 
        since we don't wanna inject other inputs during resimulation*/
        else if (overrideInputs != nullptr) {
            for (u8 i = 0; i < Netplay::getGameSettings()->numPlayers; i++) {
                InjectInputsForPlayer(&overrideInputs[i].pad, overrideInputs[i].playerIdx);
            }
            free(overrideInputs);
            overrideInputs = nullptr;
        }
        _OSEnableInterrupts();
    }

}


namespace FrameLogic {

    bool shouldLoadStatePerFrame = false;

    void ProcessFrameDataFromEmu(FrameData* framedata) {
        // probably not necessary
        // the "frame" of this framedata isn't used
        for (u8 i = 0; i < Netplay::getGameSettings()->numPlayers; i++) {
            PlayerFrameData* remotePlayerFrameData = &framedata->playerFrameDatas[i];
            swapByteOrder(&remotePlayerFrameData->frame);
        }
        
        if (FrameAdvance::overrideInputs != nullptr) {
            OSReport("Override inputs already populated!\n");
            free(FrameAdvance::overrideInputs);
            FrameAdvance::overrideInputs = nullptr;
        }
        u8 numPlayers = Netplay::getGameSettings()->numPlayers;
        FrameAdvance::overrideInputs = (PlayerFrameData*)malloc(sizeof(PlayerFrameData)*numPlayers);
        memcpy(FrameAdvance::overrideInputs, &framedata->playerFrameDatas[0], sizeof(PlayerFrameData)*numPlayers);
    }

    void FixRollbackInfoEndianess(RollbackInfo* rollbackInfo) {
        swapByteOrder(&rollbackInfo->beginFrame); // swap endianness since these values came from emu (and are longer than 1 byte)
        swapByteOrder(&rollbackInfo->endFrame);
        for (int pIdx = 0; pIdx < Netplay::getGameSettings()->numPlayers; pIdx++) {
            for (int i = 0; i < MAX_ROLLBACK_FRAMES; i++) {
                PlayerFrameData* pfd = &rollbackInfo->pastFrameDatas[i].playerFrameDatas[pIdx];
                if (pfd->frame != 0) { // frame 0 means there's no past framedata there
                    swapByteOrder(&pfd->frame);
                }
            }
            PlayerFrameData* predictedInput = &rollbackInfo->predictedInputs.playerFrameDatas[pIdx];
            swapByteOrder(&predictedInput->frame);
        }
    }

    void ProcessRollback(RollbackInfo* rollbackInfo, bool shouldSwitchEndian) {
        // number of frames to resim is the frame we received inputs again - the frame we began not receiving inputs
        if (shouldSwitchEndian) {
            FixRollbackInfoEndianess(rollbackInfo);
        }
        OSReport("ProcessRollback: endframe: %u  beginframe: %u\n", rollbackInfo->endFrame, rollbackInfo->beginFrame);
        int numFramesToResimulate = ((int)rollbackInfo->endFrame - (int)rollbackInfo->beginFrame);
        if (numFramesToResimulate <= 0 || numFramesToResimulate > MAX_ROLLBACK_FRAMES) {
            OSReport("Num frames to resim invalid! %i\n", numFramesToResimulate);
            return;
        }

        // TODO (pine):
        // move some or most of this logic out to dolphin

        bool shouldRollback = false;
        if (rollbackInfo->pastFrameDataPopulated) {

            for (int pIdx = 0; pIdx < Netplay::getGameSettings()->numPlayers; pIdx++) {
                if (pIdx == Netplay::localPlayerIdx) continue; // only check remote players inputs
                PlayerFrameData* predictedInput = &rollbackInfo->predictedInputs.playerFrameDatas[pIdx];
                for (int i = numFramesToResimulate; i >= 0; i--) { // iterate from oldest to newest
                    // most recent at idx 0
                    PlayerFrameData* pastFramedata = &rollbackInfo->pastFrameDatas[i].playerFrameDatas[pIdx];
                    if (!compareInputs(&predictedInput->pad, &pastFramedata->pad)) {
                        if (pastFramedata->frame == 0) {
                            OSReport("Blank past framedata! i = %u pIdx = %u\n", i, pIdx);
                            continue;
                        }
                        rollbackInfo->beginFrame = pastFramedata->frame;
                        shouldRollback = true;
                        OSReport("Predicted inputs don't match actual remote inputs on frame %i  pidx %u\n", rollbackInfo->beginFrame, pIdx);
                        break;
                    }
                }
            }
            if (!shouldRollback) {
                //OSReport("Predicted inputs match real inputs! No rollback needed.\n");
            }

        }



        if (shouldRollback || (ROLLBACK_IMPL && !NETPLAY_IMPL /*rollback debugging*/)) {

            FrameAdvance::pastFrameDatas.clear();
            if (rollbackInfo->pastFrameDataPopulated) {
                //FrameAdvance::pastFrameDatas.clear();
                // populate pastFrameDatas for resim
                for (int i = 0; i < numFramesToResimulate; i++) {
                    FrameData* pastFD = (FrameData*)malloc(sizeof(FrameData));
                    memcpy(pastFD, &rollbackInfo->pastFrameDatas[i], sizeof(FrameData));
                    FrameAdvance::pastFrameDatas.push_back(pastFD);
                }
                OSReport("Populated gameside pastFrameDatas. Num frames to resim: %i\n", numFramesToResimulate);
            }
            // load state
            EXIPacket stateReloadPckt = EXIPacket(EXICommand::CMD_LOAD_SAVESTATE, rollbackInfo, sizeof(RollbackInfo));
            stateReloadPckt.Send();

            // +1 to get to the frame we were at before
            FrameAdvance::TriggerFastForwardState(numFramesToResimulate  +1);

        }
        else {
            OSReport("Shouldn't rollback! Predicted inputs match actual inputs.\n");
        }

    }

    void ReadFrameData() {
        // game (us, right here) specifies how much memory to read in from emulator.
        
        u8* cmd_byte_read = (u8*)malloc(1);
        readEXI(cmd_byte_read, 1, EXIChannel::slotB, EXIDevice::device0, EXIFrequency::EXI_32MHz);
        u8 cmd_byte = cmd_byte_read[0];
        free(cmd_byte_read);

        u32 read_data_size = 0;

        switch (cmd_byte) {
            case CMD_FRAMEDATA:
                {
                    read_data_size = sizeof(FrameData);
                }
                break;
            case CMD_ROLLBACK:
                {
                    read_data_size = sizeof(RollbackInfo);
                }
                break;
            case CMD_TIMESYNC:
                {
                    // stall for one frame here
                    FrameAdvance::StallOneFrame();
                    OSReport("Gameside: stalling for one frame\n");
                }
                return;
            default:
                {
                    //OSReport("Unknown dmaread cmd byte");
                }
                break;
        }

        if (read_data_size > 1) {
            u8* data = (u8*)malloc(read_data_size);
            readEXI(data, read_data_size, EXIChannel::slotB, EXIDevice::device0, EXIFrequency::EXI_32MHz);
            
            switch (cmd_byte) {
                case CMD_FRAMEDATA:
                    {
                        // we've received inputs from the emulator here.
                        // We don't care anything about what the inputs contain or anything about them.
                        // the emulator takes care of the logic for when and how to send inputs, all we do here
                        // is inject them into the game.
                        FrameData* framedata = (FrameData*)data;
                        ProcessFrameDataFromEmu(framedata);
                    }
                    break;
                case CMD_ROLLBACK:
                    {
                        #if ROLLBACK_IMPL
                        OSReport("Rollback gameside\n");
                        RollbackInfo* rollbackInfo = (RollbackInfo*)data;
                        ProcessRollback(rollbackInfo, true);
                        #else
                        OSReport("Tried to rollback, but rollback impl was turned off!\n");
                        #endif
                    }
                default:
                    //OSReport("Unknown dmaread cmd byte");
                    break;
            }
            free(data);
        }
    }

    void FrameDataLogic(u32 currentFrame) {
        
        u8 localPlayerIdx = Netplay::localPlayerIdx;
        if (localPlayerIdx != Netplay::localPlayerIdxInvalid) {
            PlayerFrameData* fData = (PlayerFrameData*)malloc(sizeof(PlayerFrameData));
            fData->frame = currentFrame;
            fData->playerIdx = localPlayerIdx;
            memcpy(&fData->pad, &PAD_SYSTEM->pads[localPlayerIdx], sizeof(gfPadGamecube));

            // sending inputs + current game frame
            EXIPacket fDataPckt = EXIPacket(EXICommand::CMD_ONLINE_INPUTS, fData, sizeof(PlayerFrameData));
            fDataPckt.Send();
            free(fData);

            // reading in response from emu
            ReadFrameData();
        }
        else {
            OSReport("Invalid player index! Can't send inputs to emulator!\n");
        }
    }

    void SaveState(u32 currentFrame) {        
        EXIPacket saveSavePckt = EXIPacket(EXICommand::CMD_CAPTURE_SAVESTATE, &currentFrame, sizeof(currentFrame));
        saveSavePckt.Send();
    }

    //Does not update random seed, so multiple uses at once return same value
    int randi(int max) {
        return DEFAULT_MT_RAND->seed % max;
    }

    bool percentChance(int percent) {
        return randi(100) <= percent-1;
    }


    // called at the beginning of the game logic in a frame
    // a this point, inputs are populated for this frame
    // but the game logic that operates on those inputs has not yet happened
    void BeginFrame() {
        
        // this is the start of all our logic for each frame. Because EXI writes/reads are synchronous,
        // you can think of the control flow going like this
        // this function -> write data to emulator through exi -> emulator processes data and possibly queues up data
        // to send back to the game -> send data to the game if there is any -> game processes that data -> repeat

        if (Match::isInMatch) {
            _OSDisableInterrupts();

            u32 currentFrame = getCurrentFrame();
            OSReport("------ Begin Frame %u ------\n", currentFrame);
            
            
            #if NETPLAY_IMPL
            FrameDataLogic(currentFrame);
            #else
            //bool shouldRollback = PAD_SYSTEM->pads[0].buttons.A || PAD_SYSTEM->pads[1].buttons.A;
            //bool shouldRollback = percentChance(15);
            bool shouldRollback = currentFrame % 20 == 0;

            if (shouldRollback && currentFrame > 250) {
                const int numFramesToRollback = MAX_ROLLBACK_FRAMES;
                RollbackInfo rollbackInfo = RollbackInfo();
                rollbackInfo.beginFrame = getCurrentFrame() - numFramesToRollback;
                rollbackInfo.endFrame = getCurrentFrame();
                rollbackInfo.hasPreserveBlocks = false;
                rollbackInfo.pastFrameDataPopulated = false;
                ProcessRollback(&rollbackInfo, false);
            }
            #endif

            #if ROLLBACK_IMPL
            // only save state on frames when we aren't rolling back, or timesyncing
            if (FrameAdvance::framesToAdvance == 1) {
                SaveState(currentFrame);
            }
            #endif

            _OSEnableInterrupts();
        }
        else { // not in a match
            
        }
    }

    // called at the end of the game logic in a frame (rendering logic happens after this func in the frame)
    // at this point, I think its (maybe?) guarenteed that inputs are cleared out already
    void EndFrame() {
        if (Match::isInMatch) {
            _OSDisableInterrupts();
            // just resimulated/stalled/skipped/whatever, reset to normal
            FrameAdvance::ResetFrameAdvance();

            //OSReport("------ End Frame ------\n");
            _OSEnableInterrupts();
        }
    }


    SIMPLE_INJECTION(beginFrame, 0x80147394, "li r0, 0x1") { BeginFrame(); }
    SIMPLE_INJECTION(endFrame,   0x801473a0, "li r0, 0x0") { EndFrame(); }
  
}