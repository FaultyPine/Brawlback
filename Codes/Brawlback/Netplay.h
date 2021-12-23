#ifndef __NETPLAYIMPL
#define __NETPLAYIMPL

#include "Brawlback.h"

namespace Netplay {

    bool CheckShouldStartNetplay();

    void StartMatch();
    void EndMatch();

    int getLocalPlayerIdx();
    GameSettings* getGameSettings();

    extern int localPlayerIdx;
}


#endif