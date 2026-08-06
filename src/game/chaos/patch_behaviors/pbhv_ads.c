#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "engine/behavior_script.h"
#include "game/chaos/chaos.h"
#include "hvqm/hvqm.h"
#include "game/emutest.h"
#include "game/game_init.h"

#define AD_MINUTES 5

#define HVQM_ENTRIES    \
HVQM_MACRO(blj)         \
HVQM_MACRO(chaos)       \
HVQM_MACRO(crash)       \
HVQM_MACRO(cod)         \
HVQM_MACRO(krabs)       \
HVQM_MACRO(fushigi)     \
HVQM_MACRO(pizza)       \
HVQM_MACRO(dream)       \
HVQM_MACRO(show)        \
HVQM_MACRO(castle)      \
HVQM_MACRO(miku)        \
HVQM_MACRO(towncountry) \
HVQM_MACRO(slapchop)    \
HVQM_MACRO(roguechaos)  \
HVQM_MACRO(webuygolf)   \
HVQM_MACRO(easthills)   \

#define HVQM_FILE(ptr) _ ## ptr ## SegmentRomStart
#define HVQM_PTR(ptr) (u32*) &HVQM_FILE(ptr)
#define HVQM_MACRO(ptr) extern u32 *HVQM_FILE(ptr);
HVQM_ENTRIES
#undef HVQM_MACRO

#define HVQM_MACRO(ptr) (u32*) &HVQM_FILE(ptr),
u32 *chsHVQMTable[] = {
HVQM_ENTRIES
};

u32 chsCurrentAd = 0;

void chs_act_serve_ads(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_AD_BREAK, &this);
    if (!this) {
        return;
    }

    // Extend chances of escaping the apparently still possible HVQM freeze
    this->frameTimer = 0;
}

void chs_update_serve_ads(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_AD_BREAK, &this);
    if (!this || (this->frameTimer < (AD_MINUTES * 60 * 30))) {
        return;
    }

    s32 adToPlay = 0;
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

    hvqm_play(chsHVQMTable[adToPlay]);
    this->frameTimer = 0;
}

void chs_debug_serve_ads(void) {
    s32 adToPlay = 0;
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

    hvqm_play(chsHVQMTable[adToPlay]);
}
