#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "game/chaos/chaos.h"
#include "hvqm/hvqm.h"
#include "game/game_init.h"

#define HVQM_FILE(ptr) _ ## ptr ## SegmentRomStart
#define HVQM_PTR(ptr) &HVQM_FILE(ptr)
#define HVQM_EXTERN(ptr) extern void *HVQM_FILE(ptr)

#define AD_MINUTES 1

HVQM_EXTERN(spoon32);
HVQM_EXTERN(krabs);
void *chsHVQMTable[] = {
    HVQM_PTR(spoon32),
    HVQM_PTR(krabs),
};
u32 chsCurrentAd = 0;


void chs_serve_ads(void) {
    if (gGlobalTimer % (AD_MINUTES * 60 * 30) == 0) {
        hvqm_play(chsHVQMTable[chsCurrentAd++]);
        if (chsCurrentAd >= ARRAY_COUNT(chsHVQMTable)) {
            chsCurrentAd = 0;
        }
    }
}