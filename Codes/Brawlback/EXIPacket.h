#ifndef __EXIPACKET
#define __EXIPACKET

#include "Utility/Utility.h"

enum EXICommand : u8
{
    CMD_UNKNOWN = 0,

    CMD_ONLINE_INPUTS = 1,
    CMD_CAPTURE_SAVESTATE = 2,
    CMD_LOAD_SAVESTATE = 3,

    CMD_FIND_OPPONENT = 5,
    CMD_START_MATCH = 13,
    CMD_SETUP_PLAYERS = 14,
    CMD_FRAMEDATA = 15,
    CMD_TIMESYNC = 16,
    CMD_ROLLBACK = 17,

    CMD_START_TIMER = 7,
    CMD_END_TIMER = 8,

    CMD_GAME_PROC_OVERRIDE = 9,
    CMD_GAME_PROC = 10,

};


class EXIPacket {

public:
    // EXICmd: EXICommand enum (implicit cast to byte)
    EXIPacket(u8 EXICmd, void* source, u32 size);
    EXIPacket(u8 EXICmd);
    EXIPacket();
    ~EXIPacket();

    static void EXIFunction(EXICommand cmd, u8* src, u32 srcSize, u8* dst, u32 dstSize);

    void Send();
    void Receive(u8* buf, u32 size);

    EXICommand getCmd();

private:

    u8* source = nullptr;
    u32 size = 0;
    EXICommand cmd = EXICommand::CMD_UNKNOWN;

};




#endif