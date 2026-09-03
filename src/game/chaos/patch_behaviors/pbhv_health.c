#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "behavior_data.h"
#include "course_table.h"
#include "game/area.h"
#include "game/level_update.h"
#include "game/mario.h"
#include "game/save_file.h"
#include "game/chaos/chaos.h"

#define TEMPORARY_INVINCIBILITY_DURATION (30 * 30)

/*
    Health Drain
*/

void chs_update_health_drain(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_HEALTH_DRAIN, &this);
    
    if(!(this->frameTimer % 90)) {
        gMarioState->hurtCounter++;
        this->frameTimer = 0;
    }
}

/*
    Health Gain
*/

void chs_update_health_gain(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_HEALTH_GAIN, &this);
    
    if(!(this->frameTimer % 90)) {
        gMarioState->healCounter++;
        this->frameTimer = 0;
    }
}

/*
    Health Up
*/

u8 chs_calculate_max_heal_counter(void) {
    return 31 + ((gMarioState->maxHealth - 0x880) / 0x40);
}

u8 chs_cond_health_up(void) {
    return (gMarioState->maxHealth < 0x1080);
}

void chs_act_health_up(void) {
    if (gChaosImmediateActDeact) {
        return;
    }

    gMarioState->maxHealth += 0x100;
    gMarioState->health += 0x100;

    gHudDisplay.wedges = (gMarioState->health > 0) ? (gMarioState->health >> 8) : 0;
}

void chs_deact_health_up(void) {
    if (gChaosImmediateActDeact) {
        return;
    }

    gMarioState->maxHealth -= 0x100;
    gMarioState->health -= 0x100;
    if (gMarioState->health < 0xFF) {
        gMarioState->health = 0xFF;
    }

    gHudDisplay.wedges = (gMarioState->health > 0) ? (gMarioState->health >> 8) : 0;
}

/*
    Health Down
*/

u8 chs_cond_health_down(void) {
    return (gMarioState->maxHealth > 0x480 && (gMarioState->maxHealth >= 0x800 || !chaos_check_if_patch_active(CHAOS_PATCH_NO_TOLERANCE_FALL_DAMAGE)));
}

void chs_act_health_down(void) {
    if (gChaosImmediateActDeact) {
        return;
    }

    gMarioState->maxHealth -= 0x100;
    gMarioState->health -= 0x100;
    if (gMarioState->health < 0xFF) {
        gMarioState->health = 0xFF;
    }

    gHudDisplay.wedges = (gMarioState->health > 0) ? (gMarioState->health >> 8) : 0;
}

void chs_deact_health_down(void) {
    if (gChaosImmediateActDeact) {
        return;
    }

    gMarioState->maxHealth += 0x100;
    gMarioState->health += 0x100;

    gHudDisplay.wedges = (gMarioState->health > 0) ? (gMarioState->health >> 8) : 0;
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
    Lava Patches
*/

u8 chs_cond_no_lava_damage(void) {
    return (save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR | SAVE_FLAG_MOAT_DRAINED));
}

u8 chs_cond_instakill_lava(void) {
    return (save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR | SAVE_FLAG_MOAT_DRAINED));
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

/*
    Shield
*/

u8 chs_cond_shield(void) {
    struct ChaosActiveEntry *match;
    chaos_find_first_active_patch(CHAOS_PATCH_SHIELD, &match);
    if(match) {
        return (match->remainingDuration < 9);
    } else {
        return TRUE;
    }
}

#define INVINCIBILITY_TIME_MAX      27000

/*
    Random Invincibility
*/

void chs_act_random_invincibility(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_INVINCIBILITY, &this);
    this->frameTimer = RAND(INVINCIBILITY_TIME_MAX); //Get a random offset to start the timer at
}

void chs_update_random_invincibility(void) {
    struct ChaosActiveEntry *this;
    chaos_find_first_active_patch(CHAOS_PATCH_RANDOM_INVINCIBILITY, &this);
    if(this->frameTimer > INVINCIBILITY_TIME_MAX) {
        gMarioState->invincTimer = 600;
        this->frameTimer = RAND(INVINCIBILITY_TIME_MAX); //Get a random offset to restart the timer at
    }
}

/*
    Mario Dies!
*/

u8 chs_cond_mario_dies(void) {
    return ((gCurrCourseNum != COURSE_NONE) && (gChaosGameMode != CHAOS_GAMEMODE_CHALLENGE || gMarioState->numLives >= chs_life_gambler_get_lives_lost()));
}

void chs_act_mario_dies(void) {
    gMarioState->health = 0;
}
