#include "Brawlback.h"
#include "Netplay.h"




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

    // on scene start (AFTER the start/[scMelee] function has run)

    SIMPLE_INJECTION(startSceneMelee, 0x806d176c, "addi	sp, sp, 112") {
        OSReport("  ~~~~~~~~~~~~~~~~  Start Scene Melee  ~~~~~~~~~~~~~~~~  \n");
        //bool shouldNetplay = Netplay::CheckShouldStartNetplay();
        Netplay::StartMatch(); // start netplay logic
        isInMatch = true;
    }

    // on scene exit
    SIMPLE_INJECTION(exitSceneMelee, 0x806d4844, "li r4, 0x0") {
        OSReport("  ~~~~~~~~~~~~~~~~  Exit Scene Melee  ~~~~~~~~~~~~~~~~  \n");
        Netplay::EndMatch();
        isInMatch = false;
    }


}

namespace FrameAdvance {

    // how many game logic frames we should simulate this frame
    int framesToAdvance = 1;

    // sets the number of frames of game logic to run every frame
    void TriggerFastForwardState(u8 numFramesToFF) {
        framesToAdvance = numFramesToFF;
    }
    void StallOneFrame() { framesToAdvance = 0; }
    void SkipOneFrame() { framesToAdvance = 2; }
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
            //     .frame should be same for all framedatas
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
    SIMPLE_INJECTION(controllerInjectionPoint, 0x80017354, "or r4, r19, r19") {
        // if we are currently resimulating
        if (framesToAdvance > 1) {
            u32 gameLogicFrame = GAME_FRAME->persistentFrameCounter; // increments even during resim frames
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

        // copy framedata into pastFrameDatas
        FrameData* new_framedata = (FrameData*)malloc(sizeof(FrameData));
        memcpy(new_framedata, framedata, sizeof(FrameData));
        if (FrameAdvance::pastFrameDatas.size() + 1 > MAX_ROLLBACK_FRAMES) {
            FrameAdvance::pastFrameDatas.pop_front(true);
        }
        FrameAdvance::pastFrameDatas.push_back(new_framedata);
    }

    void ProcessRollback(RollbackInfo* rollbackInfo) {
        if (!rollbackInfo->pastFrameDataPopulated) {
            OSReport("Past framedata not populated when trying to rollback gameside!\n");
            return;
        }

        // number of frames to resim is the frame we received inputs again - the frame we began not receiving inputs
        u32 numFramesToResimulate = rollbackInfo->endFrame - rollbackInfo->beginFrame;

        FrameAdvance::pastFrameDatas.clear();
        // populate pastFrameDatas for resim
        for (int i = 0; i < numFramesToResimulate; i++) {
            FrameData* pastFD = (FrameData*)malloc(sizeof(FrameData));
            memcpy(pastFD, &rollbackInfo->pastFrameDatas[i], sizeof(FrameData));
            FrameAdvance::pastFrameDatas.push_back(&rollbackInfo->pastFrameDatas[i]);
        }
        OSReport("Populated gameside pastFrameDatas. Num frames to resim: %u\n", numFramesToResimulate);

        // load state
        EXIPacket stateReloadPckt = EXIPacket(EXICommand::CMD_LOAD_SAVESTATE, rollbackInfo, sizeof(RollbackInfo));
        stateReloadPckt.Send();


        FrameAdvance::TriggerFastForwardState(numFramesToResimulate);
    }

    void ReadFrameData() {
        // game (us, right here) specifies how much memory to read in from emulator.
        
        u32 read_data_size = sizeof(FrameData)+1; // FrameData + cmd byte
        u8* read_data = (u8*)malloc(read_data_size);
        readEXI(read_data, read_data_size, EXIChannel::slotB, EXIDevice::device0, EXIFrequency::EXI_32MHz);

        u8 cmd_byte = read_data[0];
        u8* data = &read_data[1];
        
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
                    OSReport("Rollback gameside\n");
                    RollbackInfo* rollbackInfo = (RollbackInfo*)data;
                    ProcessRollback(rollbackInfo);
                }
            case CMD_TIMESYNC:
                {
                    // stall for one frame here
                    FrameAdvance::StallOneFrame();
                    OSReport("Gameside: stalling for one frame\n");
                }
                break;
            default:
                //OSReport("Unknown dmaread cmd byte");
                break;
        }
        free(read_data);
    }

    void FrameDataLogic(u32 currentFrame) {
        
        // sending inputs + current game frame
        {
            u8 localPlayerIdx = Netplay::localPlayerIdx;
            if (localPlayerIdx != Netplay::localPlayerIdxInvalid) {
                PlayerFrameData* fData = (PlayerFrameData*)malloc(sizeof(PlayerFrameData));
                fData->frame = currentFrame;
                fData->playerIdx = localPlayerIdx;
                memcpy(&fData->pad, &PAD_SYSTEM->pads[localPlayerIdx], sizeof(gfPadGamecube));

                EXIPacket fDataPckt = EXIPacket(EXICommand::CMD_ONLINE_INPUTS, fData, sizeof(PlayerFrameData));
                fDataPckt.Send();
                free(fData);
            }
            else {
                OSReport("Invalid player index! Can't send inputs to emulator!\n");
            }
        }
        ReadFrameData();
    }

    void SaveState(u32 currentFrame) {        
        EXIPacket saveSavePckt = EXIPacket(EXICommand::CMD_CAPTURE_SAVESTATE, &currentFrame, sizeof(currentFrame));
        saveSavePckt.Send();
    }



    // called at the beginning of the game logic in a frame
    // a this point, inputs are populated for this frame
    // but the game logic that operates on those inputs has not yet happened
    void BeginFrame() {
        
        // this is the start of all our logic for each frame. Because EXI writes/reads are synchronous,
        // you can think of the control flow going like this
        // this function -> write data to emulator through exi -> emulator processes data and possibly queues up data
        // to send back to the game -> send data to the game if there is any -> game processes that data -> repeat

        if (Match::isInMatch && Netplay::localPlayerIdx != Netplay::localPlayerIdxInvalid) {

            u32 currentFrame = GAME_FRAME->persistentFrameCounter; // or maybe use frameCounter here?
            OSReport("-- Begin Frame %u --\n", currentFrame);

            s64 start = getTime();
            FrameDataLogic(currentFrame);
            //SaveState(currentFrame);
            s64 end = getTime();

            OSCalendarTime startTime = OSTimeToCalendarTime(start);
            OSCalendarTime endTime = OSTimeToCalendarTime(end);
            int diff = endTime.usec - startTime.usec;
            OSReport("frame logic took %i microseconds\n", diff);

        }
        else { // not in a match
            
        }
    }

    // called at the end of the game logic in a frame (rendering logic happens after this func in the frame)
    // at this point, I think its (maybe?) guarenteed that inputs are cleared out already
    void EndFrame() {
        if (Match::isInMatch && Netplay::localPlayerIdx != Netplay::localPlayerIdxInvalid) {
            OSReport("-- End Frame --\n");

            // just resimulated/stalled/skipped/whatever, reset to normal
            FrameAdvance::ResetFrameAdvance();
            
        }
    }


    SIMPLE_INJECTION(beginFrame, 0x80147394, "li r0, 0x1") { BeginFrame(); }
    SIMPLE_INJECTION(endFrame,   0x801473a0, "li r0, 0x0") { EndFrame(); }
  
}