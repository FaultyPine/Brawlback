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

    OSReport("Rand seed: %u\n", settings->randomSeed);
    OSReport("stage id: %u\n", settings->stageID);

    for (int i = 0; i < 4; i++) {
        // character id and char color are set in the 'afterPlayerInit' injection
        PlayerSettings* playerSettings = &settings->playerSettings[i];
        /*playerSettings->nametag = {};
        playerSettings->displayName = {};
        playerSettings->controllerPort = 254;
        playerSettings->connectCode = {};
        playerSettings->charID = 0;
        playerSettings->charColor = 0;*/
        // PlayerType filled in later
    }
}


// take gamesettings and apply it to our game
void MergeGameSettingsIntoGame(GameSettings* settings) {
    //DEFAULT_MT_RAND->seed = settings->randomSeed;
    DEFAULT_MT_RAND->seed = 0x496ffd00; // hardcoded for testing
    //GM_GLOBAL_MODE_MELEE->stageID = settings->stageID;
    GM_GLOBAL_MODE_MELEE->stageID = 2;

    
    for (int i = 0; i < 4; i++) {
        PlayerSettings* playerSettings = &settings->playerSettings[i];
        if (playerSettings->playerType == PlayerType::PLAYERTYPE_LOCAL) {
            OSReport("Local player index is %i\n", i);
            Netplay::localPlayerIdx = i;
            break;
        }
    }
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

    int framesToAdvance = 1;

    // sets the number of frames of game logic to run every frame
    void TriggerFastForwardState(u8 numFramesToFF) {
        framesToAdvance = numFramesToFF;
    }
    void EndFastForwardState() { framesToAdvance = 1; }

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


    // assign gfPadGamecube[4] to this if you want to override input for a single frame
    gfPadGamecube* overrideInputs = nullptr;

    void InjectInputs(gfPadGamecube* pads, int localPlayer = -1) {        
        for (int i = 0; i < 4; i++) { // overwrite player's pads with past pad inputs (unless it's local player)
            if (localPlayer == -1 || i != localPlayer) {
                PAD_SYSTEM->pads[i] = pads[i];
            }
        }
    }

    // for keeping track of the past few framedatas
    Queue<FrameData*> pastFrameDatas = Queue<FrameData*>();
    // this is at the very beginning of the main game logic loop. This should be a good place to inject inputs for
    // each fast-forwarded frame
    SIMPLE_INJECTION(controllerInjectionPoint, 0x80017354, "or r4, r19, r19") {
        // if we are currently resimulating
        if (framesToAdvance > 1) {
            u32 gameLogicFrame = GAME_FRAME->persistentFrameCounter;
            //OSReport("Current game logic frame (in loop): %u\n", gameLogicFrame);
            if (!pastFrameDatas.empty()) {
                auto node = pastFrameDatas.getNodeFront();
                // iterate through our queue of past frame datas
                while (node != nullptr) {
                    if (node->data->frame == gameLogicFrame) { // if the frame of that FrameData in the queue is the same as the current one
                        InjectInputs(node->data->pads); // inject inputs and break out
                        break;
                    }
                    node = node->next_ptr;
                }
            }
        }
        if (overrideInputs) {
            InjectInputs(overrideInputs, Netplay::localPlayerIdx);
            free(overrideInputs);
            overrideInputs = nullptr;
        }
    }

}


namespace FrameLogic {

    bool shouldLoadStatePerFrame = false;


    // writes out data to emulator
    void WriteLogic() {
        u32 currentFrame = GAME_FRAME->persistentFrameCounter; // or maybe use frameCounter here?
        
        // sending inputs + current game frame
        {
            FrameData* fData = (FrameData*)malloc(sizeof(FrameData));
            fData->frame = currentFrame;
            fData->randomSeed = 0; // tmp
            memcpy(fData->pads, PAD_SYSTEM->pads, sizeof(PAD_SYSTEM->pads));
            fData->pads[0].buttons.UpDPad = 0; // get rid of load state input -- for testing

            if (FrameAdvance::pastFrameDatas.size() + 1 > MAX_ROLLBACK_FRAMES) {
                FrameAdvance::pastFrameDatas.pop_front(true);
            }
            FrameAdvance::pastFrameDatas.push_back(fData); // tmp

            EXIPacket fDataPckt = EXIPacket(EXICommand::CMD_ONLINE_INPUTS, fData, sizeof(FrameData));
            fDataPckt.Send();
        }
        
        
        bool isLoadStateButton = PAD_SYSTEM->pads[0].buttons.UpDPad; // for testing


        if (!shouldLoadStatePerFrame) shouldLoadStatePerFrame = isLoadStateButton;
        
        // saving state each frame
        {
            EXIPacket saveSavePckt = EXIPacket(EXICommand::CMD_CAPTURE_SAVESTATE);
            saveSavePckt.Send();
        }
        
        if (shouldLoadStatePerFrame && currentFrame % 5 == 0) { // rollback and resimulate every 5 frames
            PAD_SYSTEM->pads[0].buttons.UpDPad = 0; // get rid of load state input (so it doesn't infinitely make you do that same input) -- for testing
            EXIPacket LoadSaveStatePckt = EXIPacket(EXICommand::CMD_LOAD_SAVESTATE);
            LoadSaveStatePckt.Send();
            FrameAdvance::TriggerFastForwardState(MAX_ROLLBACK_FRAMES);
        }
        

    }

    // reads in data from emulator
    void ReadLogic() {
        // game (us, right here) specifies how much memory to read in from emulator.

        u32 read_data_size = sizeof(FrameData)+1;
        u8* read_data = (u8*)malloc(read_data_size);
        readEXI(read_data, read_data_size, EXIChannel::slotB, EXIDevice::device0, EXIFrequency::EXI_32MHz);

        u8 cmd_byte = read_data[0];
        u8* data = &read_data[1];

        if (cmd_byte == CMD_FRAMEDATA) {
            FrameData* framedata = (FrameData*)data;
            swapByteOrder(&framedata->frame);
            swapByteOrder(&framedata->randomSeed);

            //OSReport("[Emu->Game] Opponents inputs:\n");
            //print_half(framedata->pads[0].buttons.bits);

            //OSReport("[Emu->Game] Frame: %u\n", framedata->frame);

            FrameAdvance::overrideInputs = (gfPadGamecube*)malloc(sizeof(gfPadGamecube)*4);
            memcpy(FrameAdvance::overrideInputs, &framedata->pads[0], sizeof(gfPadGamecube)*4);
        }
        free(read_data);
    }

    // called at the beginning of the game logic in a frame
    // a this point, inputs are populated for this frame
    // but the game logic that operates on those inputs has not yet happened
    void BeginFrame() {
        
        // this is the start of all our logic for each frame. Because EXI writes/reads are synchronous,
        // you can think of the control flow going like this
        // this function -> write data to emulator through exi -> emulator processes data and possible queues up data
        // to send back to the game -> send data to the game if there is any -> game processes that data -> repeat

        if (Match::isInMatch) {
            //OSReport("-----------------------------\n");

            // write data into dolphin
            WriteLogic();
            // dolphin then does some stuff with that data, and if it needs to, puts data into the dmaread queue,
            // so we read in that data and deal with it here
            ReadLogic();

        }
        else { // not in a match
            
        }
    }

    // called at the end of the game logic in a frame (rendering logic happens after this func in the frame)
    // at this point, I think its (maybe?) guarenteed that inputs are cleared out already
    void EndFrame() {
        if (Match::isInMatch) {

            if (FrameAdvance::framesToAdvance > 1) {
                FrameAdvance::EndFastForwardState();
            }
            
        }
    }


    SIMPLE_INJECTION(beginFrame, 0x80147394, "li r0, 0x1") { BeginFrame(); }
    SIMPLE_INJECTION(endFrame,   0x801473a0, "li r0, 0x0") { EndFrame(); }
  
}