
#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"
#include "chaos_patch_behaviors.h"

const struct ChaosPatch gChaosPatches[CHAOS_PATCH_COUNT] = {
// Empty Modifiers
    [CHAOS_PATCH_NONE_POSITIVE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = 0,
        .severity          = 0,
        .isStackable       = TRUE,
        .duration          = 0,

        .conditionalFunc   = NULL,
        .activatedInitFunc = NULL,
        .areaInitFunc      = NULL,
        .frameUpdateFunc   = NULL,
        .deactivationFunc  = NULL,

        .name              = "@BFBFBF--" "---" "@--------",
        .shortDescription  = "@BFBFBF--No positive effect.@--------",
        .longDescription   = NULL,
    },
    [CHAOS_PATCH_NONE_NEGATIVE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = 0,
        .severity          = 0,
        .isStackable       = TRUE,
        .duration          = 0,

        .conditionalFunc   = NULL,
        .activatedInitFunc = NULL,
        .areaInitFunc      = NULL,
        .frameUpdateFunc   = NULL,
        .deactivationFunc  = NULL,

        .name              = "@BFBFBF--" "---" "@--------",
        .shortDescription  = "@BFBFBF--No negative effect.@--------",
        .longDescription   = NULL,
    },

/**************************************************************************************************************************/

// Lives Modifiers
    [CHAOS_PATCH_LIVES_INCREASE_LV1] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_LIVES_DECREASE_LV1,
        .severity          = 1,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_increase_lv1,
        .activatedInitFunc = chs_act_lives_increase_lv1,

        .name              = "Life Saver",
        .shortDescription  = "Increase Mario's total lives by 3.",
    },
    [CHAOS_PATCH_LIVES_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_LIVES_DECREASE_LV1,
        .severity          = 2,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_increase_lv2,
        .activatedInitFunc = chs_act_lives_increase_lv2,

        .name              = "The Gift of Life",
        .shortDescription  = "Increase Mario's total lives by 6.",
    },
    [CHAOS_PATCH_LIVES_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_LIVES_DECREASE_LV1,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_increase_lv3,
        .activatedInitFunc = chs_act_lives_increase_lv3,

        .name              = "Life Miracle",
        .shortDescription  = "Increase Mario's total lives by 10.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV1] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_LIVES_INCREASE_LV1,
        .severity          = 1,
        .isStackable       = TRUE,
    
        .conditionalFunc   = chs_cond_lives_decrease_lv1,
        .activatedInitFunc = chs_act_lives_decrease_lv1,

        .name              = "Loss of Life",
        .shortDescription  = "Decrease Mario's remaining lives by 3.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_LIVES_INCREASE_LV1,
        .severity          = 2,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_decrease_lv2,
        .activatedInitFunc = chs_act_lives_decrease_lv2,

        .name              = "Life Travesty",
        .shortDescription  = "Decrease Mario's remaining lives by 6.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_LIVES_INCREASE_LV1,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_decrease_lv3,
        .activatedInitFunc = chs_act_lives_decrease_lv3,

        .name              = "Life Apocalypse",
        .shortDescription  = "Decrease Mario's remaining lives by 10.",
    },

// Star/Save Modifiers
    [CHAOS_PATCH_STARS_SHUFFLE_STARS] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_star_shuffle,
        .activatedInitFunc = chs_act_star_shuffle,

        .name              = "Star Shuffle",
        .shortDescription  = "Take two stars in your possession and randomly reassign them to new courses/acts.",
    },
    [CHAOS_PATCH_STARS_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_STARS_DECREASE_LV2,
        .severity          = 2,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_increase_lv2,

        .name              = "One-Star Offer",
        .shortDescription  = "Receive a random star. This is NOT guaranteed to be a yellow star.",
    },
    [CHAOS_PATCH_STARS_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_STARS_DECREASE_LV3,
        .severity          = 3,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_increase_lv3,

        .name              = "Two-Star Treat",
        .shortDescription  = "Receive two random stars. These are NOT guaranteed to be yellow stars.",
    },
    [CHAOS_PATCH_STARS_INCREASE_GUARANTEE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_STARS_DECREASE_GUARANTEE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_stars_increase_guarantee,
        .activatedInitFunc = chs_act_stars_increase_guarantee,

        .name              = "One-Star Guarantee",
        .shortDescription  = "Receive a random star not already in your possession.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_STARS_INCREASE_LV2,
        .severity          = 2,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_decrease_lv2,

        .name              = "One-Star Risk",
        .shortDescription  = "Mark a random star as uncollected. This is NOT guaranteed to be a star you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_STARS_INCREASE_LV3,
        .severity          = 3,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_decrease_lv3,

        .name              = "Two-Star Trick",
        .shortDescription  = "Mark two random stars as uncollected. These are NOT guaranteed to be stars you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_GUARANTEE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_STARS_INCREASE_GUARANTEE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_stars_decrease_guarantee,
        .activatedInitFunc = chs_act_stars_decrease_guarantee,

        .name              = "One-Star Giveaway",
        .shortDescription  = "Lose one random star currently in your possession.",
    },
    [CHAOS_PATCH_GET_KEY_1] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,

        .activatedInitFunc = chs_act_get_key_1,
        .conditionalFunc   = chs_cond_get_key_1,

        .name              = "Free Key 1",
        .shortDescription  = "Unlock the basement key for free!",
    },
    [CHAOS_PATCH_GET_KEY_2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,

        .activatedInitFunc = chs_act_get_key_2,
        .conditionalFunc   = chs_cond_get_key_2,

        .name              = "Free Key 2",
        .shortDescription  = "Unlock the upstairs key for free!",
    },
    [CHAOS_PATCH_UNLOCK_CANNONS] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,

        .activatedInitFunc = chs_act_unlock_cannons,
        .conditionalFunc   = chs_cond_unlock_cannons,

        .name              = "1812 Overture",
        .shortDescription  = "Unlock all cannons in the game!",
    },

// Gravity Modifiers
    [CHAOS_PATCH_GRAVITY_DECREASE_LV1] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_GRAVITY_INCREASE_LV1,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_gravity_decrease_lv1,
        .activatedInitFunc = chs_act_gravity_decrease_lv1,
        .deactivationFunc  = chs_deact_gravity_decrease_lv1,

        .name              = "Decreased Gravity",
        .shortDescription  = "Decrease Mario's gravity by 15%.",
    },
    [CHAOS_PATCH_GRAVITY_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_GRAVITY_INCREASE_LV2,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_gravity_decrease_lv2,
        .activatedInitFunc = chs_act_gravity_decrease_lv2,
        .deactivationFunc  = chs_deact_gravity_decrease_lv2,

        .name              = "Low Gravity",
        .shortDescription  = "Decrease Mario's gravity by 30%.",
    },
    [CHAOS_PATCH_GRAVITY_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_GRAVITY_INCREASE_LV3,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_gravity_decrease_lv3,
        .activatedInitFunc = chs_act_gravity_decrease_lv3,
        .deactivationFunc  = chs_deact_gravity_decrease_lv3,

        .name              = "Moon Gravity",
        .shortDescription  = "Decrease Mario's gravity by 50%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE_LV1] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_GRAVITY_DECREASE_LV1,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_gravity_increase_lv1,
        .activatedInitFunc = chs_act_gravity_increase_lv1,
        .deactivationFunc  = chs_deact_gravity_increase_lv1,

        .name              = "Increased Gravity",
        .shortDescription  = "Increase Mario's gravity by 15%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_GRAVITY_DECREASE_LV2,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_gravity_increase_lv2,
        .activatedInitFunc = chs_act_gravity_increase_lv2,
        .deactivationFunc  = chs_deact_gravity_increase_lv2,

        .name              = "High Gravity",
        .shortDescription  = "Increase Mario's gravity by 30%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_GRAVITY_DECREASE_LV3,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_gravity_increase_lv3,
        .activatedInitFunc = chs_act_gravity_increase_lv3,
        .deactivationFunc  = chs_deact_gravity_increase_lv3,

        .name              = "Ultra Gravity",
        .shortDescription  = "Increase Mario's gravity by 50%.",
    },

// Health Modifiers
    [CHAOS_PATCH_NOHEAL_HEARTS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Heartbreak",
        .shortDescription  = "Spinning hearts no longer heal Mario.",
    },
    [CHAOS_PATCH_NOHEAL_WATER] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Watered Down",
        .shortDescription  = "Water no longer heals Mario.",
    },
    [CHAOS_PATCH_NOHEAL_COINS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Unaffordable Health Care",
        .shortDescription  = "Coins no longer heal Mario.",
    },
    [CHAOS_PATCH_HEALTH_DRAIN] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_HEALTH_GAIN,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_health_drain,
        .frameUpdateFunc   = chs_update_health_drain,

        .name              = "Bleeding Out",
        .shortDescription  = "Mario will slowly lose health over time.",
    },
    [CHAOS_PATCH_HEALTH_GAIN] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_HEALTH_DRAIN,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_health_gain,
        .frameUpdateFunc   = chs_update_health_gain,

        .name              = "Bleeding In",
        .shortDescription  = "Mario will slowly gain health over time.",
    },
    [CHAOS_PATCH_HEALTH_UP] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_HEALTH_DOWN,
        .severity          = 1,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_health_up,
        .activatedInitFunc = chs_act_health_up,

        .name              = "Health Up",
        .shortDescription  = "Health maximum increases by one.",
    },
    [CHAOS_PATCH_HEALTH_DOWN] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_HEALTH_UP,
        .severity          = 1,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_health_down,
        .activatedInitFunc = chs_act_health_down,

        .name              = "Health Down",
        .shortDescription  = "Health maximum decreases by one.",
    },
    [CHAOS_PATCH_NO_FALL_DAMAGE] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .conditionalFunc   = chs_cond_no_fall_damage,

        .name              = "Long Fall Boots",
        .shortDescription  = "Mario is immune to fall damage.",
    },
    [CHAOS_PATCH_TEMPORARY_INVINCIBILITY] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .levelInitFunc     = chs_lvlinit_temporary_invincibility,

        .name              = "Temporary Invincibility",
        .shortDescription  = "Mario cannot take damage for 30 seconds upon entering a new level.",
    },
    [CHAOS_PATCH_INSTAKILL_SQUISH] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,

        .name              = "Squishma",
        .shortDescription  = "Mario will instantly die upon taking squish damage.",
    },
    [CHAOS_PATCH_INSTAKILL_GOOMBA] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,

        .name              = "Gamer Goombas",
        .shortDescription  = "Goombas can now insta-kill Mario.",
    },
    [CHAOS_PATCH_INSTAKILL_LAVA] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,

        .name              = "The Lavas of Hell",
        .shortDescription  = "Mario will instantly die upon touching lava or freezing water.",
    },
    [CHAOS_PATCH_EXTRADAMAGE_ENEMIES] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 20,

        .conditionalFunc   = chs_cond_extradamage_enemies,
        .activatedInitFunc = chs_act_extradamage_enemies,
        .deactivationFunc  = chs_deact_extradamage_enemies,

        .name              = "Defense Drop",
        .shortDescription  = "Mario takes +1 damage from all standard enemies.",
    },
    [CHAOS_PATCH_EXTRADAMAGE_LAVA] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 20,

        .conditionalFunc   = chs_cond_extradamage_lava,
        .activatedInitFunc = chs_act_extradamage_lava,
        .deactivationFunc  = chs_deact_extradamage_lava,

        .name              = "Can't Beat the Heat",
        .shortDescription  = "Mario takes +1 additional damage from lava and fire.",
    },
    [CHAOS_PATCH_BREATH_BOOST] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 10,

        .name              = "Breath Boost",
        .shortDescription  = "Mario loses health more slowly underwater.",
    },

// Coin Modifiers
    [CHAOS_PATCH_DOUBLE_COINS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 10,

        .levelInitFunc     = chs_lvlinit_double_coins,

        .name              = "Coin Rush",
        .shortDescription  = "Coin values will be doubled for 30 seconds upon entering a new level.",
    },
    [CHAOS_PATCH_100C_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_100c_decrease_lv2,
        .activatedInitFunc = chs_act_100c_decrease_lv2,
        .deactivationFunc  = chs_deact_100c_decrease_lv2,

        .name              = "Coin Star Sale",
        .shortDescription  = "Reduce the price of the 100 coin star by 10 coins.",
    },
    [CHAOS_PATCH_100C_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_100c_decrease_lv3,
        .activatedInitFunc = chs_act_100c_decrease_lv3,
        .deactivationFunc  = chs_deact_100c_decrease_lv3,

        .name              = "Coin Star Bargain",
        .shortDescription  = "Reduce the price of the 100 coin star by 25 coins.",
    },
    [CHAOS_PATCH_PAY2WIN] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 10,

        .name              = "Pay to Win",
        .shortDescription  = "Stars are only collectible when Mario has at least 40 coins.",
    },
    [CHAOS_PATCH_6_RED_COINS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Ready for Redsies",
        .shortDescription  = "Red coin stars only require 6 red coins.",
    },
    [CHAOS_PATCH_SONIC_SIMULATOR] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Sonic Simulator",
        .shortDescription  = "Coins represent Mario's health. Just like in Sonic, Mario will drop all of his coins upon taking damage.",
    },
    [CHAOS_PATCH_COIN_SIZE] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_coin_size,

        .name              = "Coins That Double in Size",
        .shortDescription  = "Doubles the size of yellow coins.",
    },

// Random Griefing
    [CHAOS_PATCH_RANDOM_SLEEP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 20,

        .activatedInitFunc = chs_act_random_sleep,
        .frameUpdateFunc   = chs_update_random_sleep,

        .name              = "Narcolepsy",
        .shortDescription  = "Mario is feeling a little tired today. He may decide on his own to go to sleep."
    },
    [CHAOS_PATCH_RANDOM_SHOCK] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 15,

        .activatedInitFunc = chs_act_random_shock,
        .frameUpdateFunc   = chs_update_random_shock,
        .conditionalFunc   = chs_cond_random_shock,

        .name              = "Shocked to My Core",
        .shortDescription  = "Get shocked at random intervals."
    },
    [CHAOS_PATCH_RANDOM_BURN] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 15,

        .activatedInitFunc = chs_act_random_burn,
        .frameUpdateFunc   = chs_update_random_burn,
        .conditionalFunc   = chs_cond_random_burn,

        .name              = "Sick Burn",
        .shortDescription  = "Get burned at random intervals."
    },
    [CHAOS_PATCH_RANDOM_BLIND] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 9,

        .activatedInitFunc = chs_act_random_blind,
        .frameUpdateFunc   = chs_update_random_blind,

        .name              = "Blindfolded Speedrun",
        .shortDescription  = "Get blinded for 15 seconds periodically."
    },

// Movement Modifiers
    [CHAOS_PATCH_LOSEMOVE_BREAKDANCE] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 20,

        .name              = "Dance Break",
        .shortDescription  = "Mario can no longer breakdance.",
    },
    [CHAOS_PATCH_LOSEMOVE_LEDGE_GRAB] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Sore Arms",
        .shortDescription  = "Mario can no longer ledge grab.",
    },
    [CHAOS_PATCH_LOSEMOVE_KICK] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Leg Pain",
        .shortDescription  = "Mario can no longer kick.",
    },
    [CHAOS_PATCH_LOSEMOVE_SIDEFLIP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Unsimple Flips",
        .shortDescription  = "Mario can no longer sideflip.",
    },
    [CHAOS_PATCH_LOSEMOVE_LONG_JUMP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Lazy Leaper",
        .shortDescription  = "Mario can no longer long jump.",
    },
    [CHAOS_PATCH_LOSEMOVE_BACKFLIP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Flipped Off",
        .shortDescription  = "Mario can no longer backflip.",
    },
    [CHAOS_PATCH_LOSEMOVE_WALL_KICK] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Wall Kicks Won't Work",
        .shortDescription  = "Mario can no longer wall kick.",
    },
    [CHAOS_PATCH_LOSEMOVE_DOUBLE_JUMP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 10,

        .name              = "Double Trouble",
        .shortDescription  = "Mario can no longer double jump (and by extension, triple jump).",
    },
    [CHAOS_PATCH_BRAWL_TRIPPING] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 15,

        .frameUpdateFunc   = chs_update_brawl_tripping,

        .name              = "Brawl Mode",
        .shortDescription  = "Mario trips randomly sometimes. What a clumsy guy!",
    },
    [CHAOS_PATCH_GALAXY_SPIN] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 20,

        .frameUpdateFunc   = chs_update_galaxy_twirl,

        .name              = "First Introduced in Galaxy 2",
        .shortDescription  = "Press A while in midair to do a spin jump!",
    },
    [CHAOS_PATCH_GROUND_POUND_DIVE] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 20,

        .name              = "Do the Odyssey",
        .shortDescription  = "Press B while ground pounding to do a dive jump!",
    },
    [CHAOS_PATCH_GROUND_POUND_JUMP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 20,

        .name              = "Bounce Back",
        .shortDescription  = "Press A shortly after landing from a ground pound to do a bigger jump!",
    },

// Object Spawners
    [CHAOS_PATCH_GREEN_DEMON] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 1,

        .conditionalFunc   = chs_cond_green_demon,
        .frameUpdateFunc   = chs_lvlupdate_green_demon,

        .name              = "Green Demon",
        .shortDescription  = "Of course this one's in the game. Spawn a poison 1-UP mushroom that chases Mario.",
    },
    [CHAOS_PATCH_BULLET_HELL] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

        .areaInitFunc      = chs_area_init_bullet_hell,
        .frameUpdateFunc   = chs_update_bullet_hell,

        .name              = "Bullet Hell",
        .shortDescription  = "Every 30 seconds, spawn a number of Bullet Bills around Mario. Pay attention to hear the warning sound!",
    },
    [CHAOS_PATCH_SPAWN_ON_SHELL] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 4,

        .levelInitFunc     = chs_lvlinit_spawn_on_shell,
        .frameUpdateFunc   = chs_lvlupdate_spawn_on_shell,

        .name              = "Shell Spawn",
        .shortDescription  = "Start riding on a shell upon entering a new course.",
    },

// Visual Modifiers
    [CHAOS_PATCH_NO_Z_BUFFER] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 3,

        .conditionalFunc   = chs_cond_no_zbuffer,

        .name              = "Geometry Freakout",
        .shortDescription  = "Geometry can now render itself in whatever order it wants (sensory warning for most setups!)",
    },
    [CHAOS_PATCH_INVERTED_Z_BUFFER] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 2,

        .conditionalFunc   = chs_cond_inverted_zbuffer,

        .name              = "Inside-Out",
        .shortDescription  = "Geometry that is further from the camera will render on top of closer geometry. Quite the perspective!",
    },
    [CHAOS_PATCH_UPSIDE_DOWN_CAMERA] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 2,

        .name              = "Australia Mode",
        .shortDescription  = "Flips the camera upside-down so you can experience what it's like to be from the land down under.",
    },
    [CHAOS_PATCH_DECREASED_FOV] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_INCREASED_FOV,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 3,

        .conditionalFunc   = chs_cond_decreased_fov,

        .name              = "Narrow Focus",
        .shortDescription  = "Dramatically decrease Mario's field of view.",
    },
    [CHAOS_PATCH_INCREASED_FOV] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_DECREASED_FOV,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 3,

        .conditionalFunc   = chs_cond_increased_fov,

        .name              = "Fish Eyes",
        .shortDescription  = "Dramatically increase Mario's field of view.",
    },
    [CHAOS_PATCH_CONFUSED_OBJECTS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 6,

        .name              = "Confused Entities",
        .shortDescription  = "Most objects are (sometimes) facing the wrong way!",
    },
    [CHAOS_PATCH_NO_SKYBOX] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 5,

        .name              = "The Sky is...Gone?",
        .shortDescription  = "Meh, who needed it anyway...",
    },

// Time Limit
    [CHAOS_PATCH_TIME_LIMIT] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 15,

        .levelInitFunc      = chs_level_init_time_limit,
        .frameUpdateFunc    = chs_update_time_limit,
        .deactivationFunc   = chs_deact_time_limit,

        .name              = "Speedy Comet",
        .shortDescription  = "Better hurry up! You now have 3 minutes to collect each star.",
    },
    [CHAOS_PATCH_LOWER_TIME_LIMIT] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 15,

        .conditionalFunc   = chs_cond_lower_time_limit,
        .activatedInitFunc = chs_act_lower_time_limit,

        .name              = "Speedy Comet++",
        .shortDescription  = "Reset the active duration for the Speedy Comet patch, and lower the time limit by 15 seconds.",
    },

// Cheats
    [CHAOS_PATCH_L_TO_LEVITATE] = {
        .durationType      = CHAOS_DURATION_USE_COUNT,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 3,

        .conditionalFunc   = chs_cond_l_to_levitate,
        .frameUpdateFunc   = chs_update_l_to_levitate,

        .name              = "L to Levitate",
        .shortDescription  = "Press L to levitate! Each L press counts as a separate use.",
        .longDescription   = "While levitating, Mario may only barely move horizontally until the L button has been released. Make sure to line Mario up before carelessly wasting an L press!"
    },
    [CHAOS_PATCH_DEBUG_FREE_MOVE] = {
        .durationType      = CHAOS_DURATION_USE_COUNT,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 1,
        
        .conditionalFunc   = chs_cond_debug_free_move,
        .frameUpdateFunc   = chs_update_debug_free_move,

        .name              = "Debug Free Move",
        .shortDescription  = "Enables one use of debug free move. Press D-Pad Up to activate, and A to exit.",
        .longDescription   = "While using debug free move, D-Pad Up and D-Pad Down may be used to move up or down. Hold B to increase your movement speed, or Z to decrease it. Mario is (mostly) invulnerable while this mode is active!"
    },

// Chaos Modifiers
    [CHAOS_PATCH_REMOVE_NEGATIVE_PATCH] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,
        
        .conditionalFunc   = chs_cond_remove_negative_patch,
        .activatedInitFunc = chs_act_remove_negative_patch,

        .name              = "Show Me Mercy",
        .shortDescription  = "Deactivate one of the currently active negative patches at random.",
    },
    [CHAOS_PATCH_ADD_SELECTABLE_PATCH] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_REMOVE_SELECTABLE_PATCH,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,
        
        .conditionalFunc   = chs_cond_add_selectable_patch,

        .name              = "The More, The Merrier",
        .shortDescription  = "Add one more option to the patch selection menu.",
    },
    [CHAOS_PATCH_REMOVE_SELECTABLE_PATCH] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_ADD_SELECTABLE_PATCH,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,
        
        .conditionalFunc   = chs_cond_remove_selectable_patch,

        .name              = "Little Choice",
        .shortDescription  = "Remove an option from the patch selection menu.",
    },

// Speed Modifiers
    [CHAOS_PATCH_PUSH_BACK] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 5,

        .frameUpdateFunc   = chs_update_push_back,
        .conditionalFunc   = chs_cond_push_back,

        .name              = "A Little Push in the Wrong Direction",
        .shortDescription  = "Mario will constantly get pushed backwards a little.",
    },
    [CHAOS_PATCH_SPEED_LIMIT] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 4,

        .frameUpdateFunc   = chs_update_speed_limit,
        .conditionalFunc   = chs_cond_speed_limit,

        .name              = "Speed Limit",
        .shortDescription  = "Lowers Mario's maximum movement speed by a bit.",
    },
    [CHAOS_PATCH_WALKIES] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 1,

        .frameUpdateFunc   = chs_update_walkies,
        .conditionalFunc   = chs_cond_walkies,

        .name              = "Walkies",
        .shortDescription  = "Lowers Mario's maximum movement speed to a crawl.",
    },
    [CHAOS_PATCH_SPEED_TAX] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 5,

        .frameUpdateFunc   = chs_update_speed_tax,
        .conditionalFunc   = chs_cond_speed_tax,

        .name              = "Speed Tax",
        .shortDescription  = "If Mario goes too fast he will start needing to pay coins. If he can't pay, he will take damage!",
    },

// Cap Effects
    [CHAOS_PATCH_WING_CAP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 5,

        .frameUpdateFunc   = chs_update_wing_cap,

        .name              = "I Believe I Can Fly",
        .shortDescription  = "Mario gains an infinite wing cap effect!",
    },
    [CHAOS_PATCH_VANISH_CAP] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 18,

        .name              = "Wall Hacks",
        .shortDescription  = "Mario can now walk through all vanish cap walls, even without wearing the cap!",
    },

// Input Modifiers
    [CHAOS_PATCH_BUTTON_BROKEN_B] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 4,
        
        .conditionalFunc   = chs_cond_button_broken_b,

        .name              = "The Busted B",
        .shortDescription  = "The B button is completely nonfunctional (even for menus!)",
    },
    [CHAOS_PATCH_BUTTON_BROKEN_Z] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 4,
        
        .conditionalFunc   = chs_cond_button_broken_z,

        .name              = "The Zonked Z",
        .shortDescription  = "The Z button is completely nonfunctional (even for menus!)",
    },
    [CHAOS_PATCH_BUTTON_BROKEN_C] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 5,

        .conditionalFunc   = chs_cond_button_broken_c,

        .name              = "Camera Malfunction",
        .shortDescription  = "All C buttons are completely nonfunctional (even for menus!)",
    },
    [CHAOS_PATCH_SWAPPED_ZR_AB] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 5,
        
        .conditionalFunc   = chs_cond_swapped_zr_ab,

        .name              = "Bizarro Buttons",
        .shortDescription  = "The Z and R buttons will be swapped with A and B respectively (including for menus!)",
    },
    [CHAOS_PATCH_INVERTED_CAMERA_X] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 6,

        .conditionalFunc   = chs_cam_invert_x_check,

        .name              = "Camera Reversal",
        .shortDescription  = "Invert the X axis of the camera (i.e. C-Left and C-Right).",
    },
    [CHAOS_PATCH_INVERTED_STICK_X] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 2,

        .name              = "UNO Reverse Card",
        .shortDescription  = "Invert the X axis of the joystick when controlling Mario (i.e. left and right).",
    },
    [CHAOS_PATCH_INVERTED_STICK_Y] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 3,

        .name              = "Stick Shift",
        .shortDescription  = "Invert the Y axis of the joystick when controlling Mario (i.e. forward and backward).",
    },

// Size Modifiers
    [CHAOS_PATCH_MARIO_BIG] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_MARIO_SMALL,
        .severity          = 1,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_mario_big,
        .conditionalFunc   = chs_cond_mario_big,

        .name              = "One Makes You Larger",
        .shortDescription  = "Increase Mario's size by 12.5% (additive).",
    },
    [CHAOS_PATCH_MARIO_SMALL] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .negationId        = CHAOS_PATCH_MARIO_BIG,
        .severity          = 1,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_mario_small,
        .conditionalFunc   = chs_cond_mario_small,

        .name              = "One Makes You Smaller",
        .shortDescription  = "Decrease Mario's size by 12.5% (additive).",
    },

// Lethal damages
[CHAOS_PATCH_LETHAL_BONK] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 5,

        .name              = "Realistic Concussions",
        .shortDescription  = "Careful not to hit your head! Bonking now results in severe brain trauma.",
},
[CHAOS_PATCH_LETHAL_FALL_DAMAGE] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 5,

        .conditionalFunc   = chs_cond_lethal_fall_damage,

        .name              = "Realistic Fall Damage",
        .shortDescription  = "Falling will now instantly kill Mario.",
},

// Miscellaneous Modifiers
    [CHAOS_PATCH_MARIO_INVISIBLE] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_invisible,

        .name              = "Potion of Invisibility",
        .shortDescription  = "Mario is now invisible. Not even his shadow can be seen anymore!",
    },
    [CHAOS_PATCH_SIGNREAD_FAR] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Tutorial Mode",
        .shortDescription  = "New to SM64? We'll make extra sure you don't miss any tutorial signs by increasing their range!",
    },
    [CHAOS_PATCH_ONE_HIT_WONDER] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

        .conditionalFunc   = chs_cond_one_hit_wonder,

        .name              = "One-Hit Wonder",
        .shortDescription  = "Mario will die instantly upon taking any form of damage (other than from swimming or poison).",
    },
    [CHAOS_PATCH_NO_HUD] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Cinematic Mode",
        .shortDescription  = "Gets rid of that pesky HUD so you can appreciate the beauty of Super Mario 64.",
    },
    [CHAOS_PATCH_FORCED_MARIO_CAM] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 8,

        .name              = "Up Close and Personal",
        .shortDescription  = "Forces Mario Cam.",
    },
    [CHAOS_PATCH_BOWSER_THROWS] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,

        .name              = "Code That Makes You Miss Bowser Throws",
        .shortDescription  = "They have that in this game, I swear!",
    },
    [CHAOS_PATCH_BLUECOIN_LOTTERY] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

        .name              = "Blue Coin Lottery",
        .shortDescription  = "Test your luck! Every blue coin is a 1% chance to win big!",
    },
    [CHAOS_PATCH_INVERTED_SOUND] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 6,

        .name              = "Inverted Sound",
        .shortDescription  = "The game will sound upside-down!",
    },
    [CHAOS_PATCH_AD_BREAK] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 10,

        .frameUpdateFunc   = chs_update_serve_ads,

        .name              = "Ad Breaks",
        .shortDescription  = "As if the cash for making this romhack wasn't enough, have some ads too!",
    },
    [CHAOS_PATCH_ALL_STARS_SELECTABLE] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,

        .name              = "Sequence Break",
        .shortDescription  = "All course acts are now selectable, and may be completed in any order.",
    },
    [CHAOS_PATCH_MIRACLE] = {
        .durationType      = CHAOS_DURATION_USE_COUNT,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 1,

        .frameUpdateFunc   = chs_update_miracle,
        .conditionalFunc   = chs_cond_miracle,

        .name              = "The Miracle Happened",
        .shortDescription  = "The next time Mario dies, he is revived.",
    },
    [CHAOS_PATCH_DISABLE_FADE_WARPS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Fading Fantacy",
        .shortDescription  = "Fading teleports are all deactivated.",
    },
    [CHAOS_PATCH_LUIGI] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 10,

        .conditionalFunc   = chs_cond_luigi,
        .frameUpdateFunc   = chs_update_luigi,
        .deactivationFunc  = chs_deact_luigi,

        .name              = "L is Real 2401",
        .shortDescription  = "You can now play as Luigi.",
    },
    [CHAOS_PATCH_45_FPS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_20_FPS,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 2,

        .conditionalFunc   = chs_cond_45_fps,

        .name              = "45 FPS",
        .shortDescription  = "My internet's living, I'm adding frames, Grandma's living, she's adding frames...",
        .longDescription   = "This maaaaaaay not run at a consistent 45 FPS while running on hardware. Whether that makes the game easier or harder is for the player to decide.",
    },
    [CHAOS_PATCH_20_FPS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .negationId        = CHAOS_PATCH_45_FPS,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 2,

        .conditionalFunc   = chs_cond_20_fps,

        .name              = "20 FPS",
        .shortDescription  = "Alright, who plugged in the 5 dollar capture card?",
    },
    [CHAOS_PATCH_REVERB] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 8,

        .activatedInitFunc = chs_act_reverb,
        .deactivationFunc  = chs_deact_reverb,

        .name              = "High-Fidelity Reverb",
        .shortDescription  = "For that more immersive experience (even more epic in caves!)",
    },
    [CHAOS_PATCH_WEAK_BOSSES] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 6,

        .name              = "Not So Bossy",
        .shortDescription  = "All bosses die in one hit (except for during the final Bowser fight).",
    },
    [CHAOS_PATCH_TOP_DOWN_CAMERA] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

        .conditionalFunc   = chs_topdown_check,

        .name              = "Top-Down Camera",
        .shortDescription  = "Now you're playing Zelda! (without the items) (without the story) (without the combat) (without the",
    },
    [CHAOS_PATCH_QUICKTIME] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 12,

        .conditionalFunc   = chs_cond_quicktime,
        .frameUpdateFunc   = chs_update_quicktime,
        .activatedInitFunc = chs_act_quicktime,

        .name              = "Quicktime Events",
        .shortDescription  = "Follow the buttons in the order prompted on screen or die. Directed by David Cage.",
    },
    [CHAOS_PATCH_SHUFFLE_OBJECTS] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

        .areaInitFunc     = chs_start_shuffle,
        .frameUpdateFunc   = chs_shuffle_objects,

        .name              = "Shuffle Object Positions",
        .shortDescription  = "Kaze WISHES tornado did this!",
    },
};
