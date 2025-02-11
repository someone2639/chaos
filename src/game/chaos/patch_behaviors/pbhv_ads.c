#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "engine/behavior_script.h"
#include "game/chaos/chaos.h"
#include "hvqm/hvqm.h"
#include "game/game_init.h"

#define HVQM_FILE(ptr) _ ## ptr ## SegmentRomStart
#define HVQM_PTR(ptr) (u32*) &HVQM_FILE(ptr)
#define HVQM_EXTERN(ptr) extern u32 *HVQM_FILE(ptr)

#define AD_MINUTES 5

HVQM_EXTERN(anime);
HVQM_EXTERN(blj);
HVQM_EXTERN(chaos);
HVQM_EXTERN(chaos2);
HVQM_EXTERN(chaos3);
HVQM_EXTERN(chat);
HVQM_EXTERN(cod);
HVQM_EXTERN(krabs);
HVQM_EXTERN(ligma);
HVQM_EXTERN(fushigi);
HVQM_EXTERN(pizza);

u32 *chsHVQMTable[] = {
    HVQM_PTR(anime),
    HVQM_PTR(blj),
    HVQM_PTR(chaos),
    HVQM_PTR(chaos2),
    HVQM_PTR(chaos3),
    HVQM_PTR(chat),
    HVQM_PTR(cod),
    HVQM_PTR(krabs),
    HVQM_PTR(ligma),
    HVQM_PTR(fushigi),
    HVQM_PTR(pizza),
};
u32 chsCurrentAd = 0;

u8 chs_cond_serve_ads(void) { return gFBEEnabled; }

void chs_update_serve_ads(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_AD_BREAK, &this);
    if (this->frameTimer < (AD_MINUTES * 60 * 30)) {
        return;
    }

    if (chsCurrentAd == 0) {
        // Shuffle order of ads
        for (s32 i = ARRAY_COUNT(chsHVQMTable) - 1; i >= 0; i--) {
            s32 randIndex = random_float() * (i + 1);
            u32 *tmp = chsHVQMTable[randIndex];
            chsHVQMTable[randIndex] = chsHVQMTable[i];
            chsHVQMTable[i] = tmp;
        }
    }

    this->frameTimer = 0;
    hvqm_play(chsHVQMTable[chsCurrentAd++]);
    if (chsCurrentAd >= ARRAY_COUNT(chsHVQMTable)) {
        chsCurrentAd = 0;
    }
}
