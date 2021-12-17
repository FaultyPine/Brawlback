#include "Netplay.h"







namespace Netplay {

    bool isConnectingToOpponent = false;
    bool isHost = false;
    int localPlayerIdx = -1;

    int getLocalPlayerIdx() { return localPlayerIdx; }


    void StartFindOpponent(bool isHost) {
        isConnectingToOpponent = true;
        EXIPacket findOpponentPckt = EXIPacket(EXICommand::CMD_FIND_OPPONENT, &isHost, sizeof(isHost));
        findOpponentPckt.Send();

        EXIPacket startMatchPckt = EXIPacket(EXICommand::CMD_START_MATCH);
        startMatchPckt.Send();

        u8* data = (u8*)malloc(2); // cmd byte + local player idx byte
        readEXI(data, 2, EXIChannel::slotB, EXIDevice::device0, EXIFrequency::EXI_32MHz);
        if (data[0] == EXICommand::CMD_SETUP_PLAYERS) {
            localPlayerIdx = data[1];
            OSReport("Local player idx: %u\n", localPlayerIdx);
        }
    }

    void CheckShouldStartNetplay() {
        // if player 1 is pressing A and we're not already searching for an opponent, start connecting
        if (/*PAD_SYSTEM->pads[0].buttons.A && */ !isConnectingToOpponent) {
            StartFindOpponent(PAD_SYSTEM->pads[0].buttons.Z);
        }
    }

    void EndMatch() {

    }

}

