#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "behavior_data.h"
#include "course_table.h"
#include "game/area.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/chaos/chaos.h"

#define TEMPORARY_INVINCIBILITY_DURATION (30 * 30)

/*
    Health Drain
*/

u8 chs_cond_noheal_coins(void) {return !chaos_check_if_patch_active(CHAOS_PATCH_HEALTH_DRAIN);}
u8 chs_cond_health_drain(void) {return !(chaos_check_if_patch_active(CHAOS_PATCH_HEALTH_GAIN) || !chaos_check_if_patch_active(CHAOS_PATCH_NOHEAL_COINS));}

void chs_update_health_drain(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_HEALTH_DRAIN, &this);
    
    if(!(this->frameTimer % 90)) {
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
    
    if(!(this->frameTimer % 90)) {
        gMarioState->healCounter++;
    }
}

/*
    Health Up
*/

u8 chs_calculate_max_heal_counter(void) {
    return 31 + ((gMarioState->maxHealth - 0x880) / 0x40);
}

u8 chs_cond_health_up(void) {
    return (gMarioState->maxHealth < 0x1000);
}

void chs_act_health_up(void) {
    gMarioState->maxHealth += 0x100;
    gMarioState->health += 0x100;
    gHudDisplay.wedges++;
}

void chs_deact_health_up(void) {
    gMarioState->maxHealth -= 0x100;
    if (gMarioState->health > gMarioState->maxHealth) {
        gMarioState->health = gMarioState->maxHealth;
    }
}

/*
    Health Down
*/

u8 chs_cond_health_down(void) {
    return (gMarioState->maxHealth > 0x200);
}

void chs_act_health_down(void) {
    gMarioState->maxHealth -= 0x100;
    gHudDisplay.wedges--;
    if (gMarioState->health > gMarioState->maxHealth) {
        gMarioState->health = gMarioState->maxHealth;
    }
}

void chs_deact_health_down(void) {
    gMarioState->maxHealth += 0x100;
}

/*
    Temporary Invincibility
*/

u8 chs_check_temporary_invincibility(void) {
    if (!chaos_check_if_patch_active(CHAOS_PATCH_TEMPORARY_INVINCIBILITY)) {
        return FALSE;
    }

    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_TEMPORARY_INVINCIBILITY, &this);
    if (this->frameTimer >= TEMPORARY_INVINCIBILITY_DURATION || gCurrCourseNum == COURSE_NONE) {
        return FALSE;
    }

    return TRUE;
}

void chs_lvlinit_temporary_invincibility(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_TEMPORARY_INVINCIBILITY, &this);

    this->frameTimer = 0;
}

/*
    Extra damage patches
*/

u8 chs_cond_extradamage_enemies(void) {
    return (gMarioState->extraDamageEnemy < (4 * 4));
}

void chs_act_extradamage_enemies(void) {
    gMarioState->extraDamageEnemy += 4;
}

void chs_deact_extradamage_enemies(void) {
    gMarioState->extraDamageEnemy -= 4;
}

u8 chs_cond_extradamage_lava(void) {
    return (gMarioState->extraDamageLava < (4 * 4));
}

void chs_act_extradamage_lava(void) {
    gMarioState->extraDamageLava += 4;
}

void chs_deact_extradamage_lava(void) {
    gMarioState->extraDamageLava -= 4;
}
