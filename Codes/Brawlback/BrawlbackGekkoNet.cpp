//
// GekkoNet mode is dolphin-driven.
//
// Game-side still uses the existing matchmaking/setup EXI calls, but there are no
// per-frame or frame-advance hooks here. Dolphin hooks mainLoopSub directly and
// owns the gameProc loop.
//

#include "Brawlback.h"
