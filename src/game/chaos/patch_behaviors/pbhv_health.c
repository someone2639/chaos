#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"

/*
    Health Drain
*/

u8 chs_cond_health_drain(void) {return !chaos_check_if_patch_active(CHAOS_PATCH_HEALTH_GAIN);}

void chs_update_health_drain(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_HEALTH_DRAIN, &this);
    
    if(!(this->frameTimer % 60)) {
        gMarioState->hurtCounter++;
    }
}

/*
    Health Gain
*/

u8 chs_cond_health_gain(void) {return !chaos_check_if_patch_active(CHAOS_PATCH_HEALTH_DRAIN);}

void chs_update_health_gain(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_HEALTH_GAIN, &this);
    
    if(!(this->frameTimer % 45)) {
        gMarioState->healCounter++;
    }
}