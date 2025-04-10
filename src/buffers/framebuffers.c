#include <ultra64.h>

#include "config.h"
#include "macros.h"

// 0x70800 bytes
ALIGNED64 u16 gFramebuffers[3][SCREEN_WIDTH * SCREEN_HEIGHT];
