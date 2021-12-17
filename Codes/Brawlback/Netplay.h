#ifndef __NETPLAYIMPL
#define __NETPLAYIMPL

#include "Brawlback.h"

namespace Netplay {

    void StartFindOpponent(bool isHost);

    void CheckShouldStartNetplay();

    void EndMatch();

    int getLocalPlayerIdx();
}


#endif