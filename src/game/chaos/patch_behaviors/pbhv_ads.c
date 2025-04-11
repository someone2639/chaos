#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "engine/behavior_script.h"
#include "game/chaos/chaos.h"
#include "hvqm/hvqm.h"
#include "game/emutest.h"
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
HVQM_EXTERN(dream);
HVQM_EXTERN(show);
HVQM_EXTERN(castle);
HVQM_EXTERN(miku);
HVQM_EXTERN(miku2);

u32 *chsHVQMTable[] = {
    HVQM_PTR(blj),
    HVQM_PTR(chaos),
    HVQM_PTR(chaos2),
    HVQM_PTR(chaos3),
    HVQM_PTR(chat),
    HVQM_PTR(cod),
    HVQM_PTR(krabs),
    HVQM_PTR(pizza),
    HVQM_PTR(fushigi),
    HVQM_PTR(dream),
    HVQM_PTR(show),
    HVQM_PTR(castle),
    HVQM_PTR(miku),
    HVQM_PTR(miku2),
};
u32 chsCurrentAd = 0;

void chs_update_serve_ads(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_AD_BREAK, &this);
    if (this->frameTimer < (AD_MINUTES * 60 * 30)) {
        return;
    }

    s32 adToPlay = 0;
    do {
        if (chsCurrentAd == 0) {
            // Shuffle order of ads
            for (s32 i = ARRAY_COUNT(chsHVQMTable) - 1; i >= 0; i--) {
                s32 randIndex = random_float() * (i + 1);
                u32 *tmp = chsHVQMTable[randIndex];
                chsHVQMTable[randIndex] = chsHVQMTable[i];
                chsHVQMTable[i] = tmp;
            }
        }

        adToPlay = chsCurrentAd++;
        if (chsCurrentAd >= ARRAY_COUNT(chsHVQMTable)) {
            chsCurrentAd = 0;
        }

    } while ((gEmulator & (EMU_CONSOLE | EMU_ARES | EMU_CEN64)) && chsHVQMTable[adToPlay] == HVQM_PTR(chaos3)); // Do not play fake crash on console when video is already unstable

    hvqm_play(chsHVQMTable[adToPlay]);
    this->frameTimer = 0;
}
