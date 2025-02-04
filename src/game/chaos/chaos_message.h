#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

#ifdef CHAOS_ENGINE_DEBUG
#define chaosmsg_print_debug(...) { \
    sprintf(gChaosInternalBuffer, __VA_ARGS__); \
    chaosmsg_print(CHAOS_PATCH_NONE, gChaosInternalBuffer); \
}
#else
#define chaosmsg_print_debug(...)
#endif

// Print a persistent chaos-related message alert to the screen. This only needs to be invoked once (i.e. not every frame).
// A patch ID may optionally be passed as an argument, along with a string that contains an instance of "%s" in it.
// Passing "%s" will apply the name and positive/negative color of the input patch ID appropriately.
// If no patch ID is to be referenced, pass in CHAOS_PATCH_NONE as an argument.
void chaosmsg_print(enum ChaosPatchID patchId, const char *str);

// Print and update all chaos messages for this frame.
void chaosmsg_render(void);
