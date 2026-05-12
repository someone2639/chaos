
#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"
#include "chaos_patch_behaviors.h"

const struct ChaosPatch gChaosPatches[CHAOS_PATCH_COUNT] = {
// Empty Modifiers
    [CHAOS_PATCH_NONE_POSITIVE] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = 0,
        .severity           = 0,
        .isStackable        = TRUE,
        .disableForHardcore = FALSE,
        .duration           = 0,
        .durationHard       = 0,
        .durationImpossible = 0,

        .conditionalFunc    = NULL,
        .activatedInitFunc  = NULL,
        .areaInitFunc       = NULL,
        .frameUpdateFunc    = NULL,
        .deactivationFunc   = NULL,

        .name               = "@BFBFBF--" "---" "@--------",
        .shortDescription   = "@BFBFBF--No positive effect.@--------",
        .longDescription    = NULL,
    },
    [CHAOS_PATCH_NONE_NEGATIVE] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = 0,
        .severity           = 0,
        .isStackable        = TRUE,
        .disableForHardcore = FALSE,
        .duration           = 0,
        .durationHard       = 0,
        .durationImpossible = 0,

        .conditionalFunc    = NULL,
        .activatedInitFunc  = NULL,
        .areaInitFunc       = NULL,
        .frameUpdateFunc    = NULL,
        .deactivationFunc   = NULL,

        .name               = "@BFBFBF--" "---" "@--------",
        .shortDescription   = "@BFBFBF--No negative effect.@--------",
        .longDescription    = NULL,
    },

/**************************************************************************************************************************/

// Lives Modifiers
    [CHAOS_PATCH_LIVES_INCREASE_LV1] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_LIVES_DECREASE_LV1,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_lives_increase_lv1,
        .activatedInitFunc  = chs_act_lives_increase_lv1,

        .name               = "Life Saver",
        .shortDescription   = "Increase Mario's total lives by 2.",
    },
    [CHAOS_PATCH_LIVES_INCREASE_LV2] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_LIVES_DECREASE_LV2,
        .severity           = 2,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_lives_increase_lv2,
        .activatedInitFunc  = chs_act_lives_increase_lv2,

        .name               = "The Gift of Life",
        .shortDescription   = "Increase Mario's total lives by 4.",
    },
    [CHAOS_PATCH_LIVES_INCREASE_LV3] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_LIVES_DECREASE_LV3,
        .severity           = 3,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_lives_increase_lv3,
        .activatedInitFunc  = chs_act_lives_increase_lv3,

        .name               = "Life Miracle",
        .shortDescription   = "Increase Mario's total lives by 6.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV1] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_LIVES_INCREASE_LV1,
        .severity           = 1,
        .isStackable        = TRUE,
    
        .conditionalFunc    = chs_cond_lives_decrease_lv1,
        .activatedInitFunc  = chs_act_lives_decrease_lv1,

        .name               = "Loss of Life",
        .shortDescription   = "Decrease Mario's remaining lives by 3.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV2] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_LIVES_INCREASE_LV2,
        .severity           = 2,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_lives_decrease_lv2,
        .activatedInitFunc  = chs_act_lives_decrease_lv2,

        .name               = "Life Travesty",
        .shortDescription   = "Decrease Mario's remaining lives by 6.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV3] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_LIVES_INCREASE_LV3,
        .severity           = 3,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_lives_decrease_lv3,
        .activatedInitFunc  = chs_act_lives_decrease_lv3,

        .name               = "Life Apocalypse",
        .shortDescription   = "Decrease Mario's remaining lives by 10.",
    },
    [CHAOS_PATCH_LIFE_GAMBLER] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = TRUE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .conditionalFunc    = chs_cond_life_gambler,

        .name               = "Life Gambler",
        .shortDescription   = "Lose twice as many lives when dying.",
    },
    [CHAOS_PATCH_INSTANT_GAME_OVER] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 3,
        .durationImpossible = 4,

        .conditionalFunc    = chs_cond_instant_game_over,

        .name               = "All In",
        .shortDescription   = "Trigger a game over if you die, no matter how many lives you have remaining. Better be careful!",
    },

// Star/Save Modifiers
    [CHAOS_PATCH_STARS_SHUFFLE_STARS] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_star_shuffle,
        .activatedInitFunc  = chs_act_star_shuffle,

        .name               = "Star Shuffle",
        .shortDescription   = "Take two stars in your possession and randomly reassign them to new courses/acts.",
    },
    [CHAOS_PATCH_STARS_INCREASE_LV2] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_STARS_DECREASE_LV2,
        .severity           = 2,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_stars_increase_lv2,
        .activatedInitFunc  = chs_act_stars_increase_lv2,

        .name               = "One-Star Offer",
        .shortDescription   = "Receive a random star. This is NOT guaranteed to be a yellow star.",
    },
    [CHAOS_PATCH_STARS_INCREASE_LV3] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_STARS_DECREASE_LV3,
        .severity           = 3,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_stars_increase_lv3,
        .activatedInitFunc  = chs_act_stars_increase_lv3,

        .name               = "Two-Star Treat",
        .shortDescription   = "Receive two random stars. These are NOT guaranteed to be yellow stars.",
    },
    [CHAOS_PATCH_STARS_INCREASE_GUARANTEE] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_STARS_DECREASE_GUARANTEE,
        .severity           = 3,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_stars_increase_guarantee,
        .activatedInitFunc  = chs_act_stars_increase_guarantee,

        .name               = "One-Star Guarantee",
        .shortDescription   = "Receive a random star not already in your possession.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV2] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_STARS_INCREASE_LV2,
        .severity           = 2,
        .isStackable        = TRUE,

        .activatedInitFunc  = chs_act_stars_decrease_lv2,

        .name               = "One-Star Risk",
        .shortDescription   = "Mark a random star as uncollected. This is NOT guaranteed to be a star you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV3] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_STARS_INCREASE_LV3,
        .severity           = 3,
        .isStackable        = TRUE,

        .activatedInitFunc  = chs_act_stars_decrease_lv3,

        .name               = "Two-Star Trick",
        .shortDescription   = "Mark two random stars as uncollected. These are NOT guaranteed to be stars you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_GUARANTEE] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_STARS_INCREASE_GUARANTEE,
        .severity           = 3,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_stars_decrease_guarantee,
        .activatedInitFunc  = chs_act_stars_decrease_guarantee,

        .name               = "One-Star Giveaway",
        .shortDescription   = "Lose one random star currently in your possession.",
    },
    [CHAOS_PATCH_GET_KEY_1] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,

        .activatedInitFunc  = chs_act_get_key_1,
        .conditionalFunc    = chs_cond_get_key_1,

        .name               = "Free Key 1",
        .shortDescription   = "Unlock the basement key for free!",
    },
    [CHAOS_PATCH_GET_KEY_2] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,

        .activatedInitFunc  = chs_act_get_key_2,
        .conditionalFunc    = chs_cond_get_key_2,

        .name               = "Free Key 2",
        .shortDescription   = "Unlock the upstairs key for free!",
    },
    [CHAOS_PATCH_GET_WING_CAP] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,

        .activatedInitFunc  = chs_act_get_wing_cap,
        .conditionalFunc    = chs_cond_get_wing_cap,

        .name               = "Free Wing Cap",
        .shortDescription   = "Unlock the wing cap for free!",
    },
    [CHAOS_PATCH_GET_METAL_CAP] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,

        .activatedInitFunc  = chs_act_get_metal_cap,
        .conditionalFunc    = chs_cond_get_metal_cap,

        .name               = "Free Metal Cap",
        .shortDescription   = "Unlock the metal cap for free!",
    },
    [CHAOS_PATCH_GET_VANISH_CAP] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,

        .activatedInitFunc  = chs_act_get_vanish_cap,
        .conditionalFunc    = chs_cond_get_vanish_cap,

        .name               = "Free Vanish Cap",
        .shortDescription   = "Unlock the vanish cap for free!",
    },
    [CHAOS_PATCH_UNLOCK_CANNONS] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,

        .activatedInitFunc  = chs_act_unlock_cannons,
        .conditionalFunc    = chs_cond_unlock_cannons,

        .name               = "1812 Overture",
        .shortDescription   = "Unlock all cannons in the game!",
    },
    [CHAOS_PATCH_STAR_CLONING_DEVICE] = {
        .durationType       = CHAOS_DURATION_USE_COUNT,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = TRUE,
        .duration           = 2,

        .conditionalFunc    = chs_cond_star_cloning_device,

        .name               = "Star Cloning Device",
        .shortDescription   = "When collected, a blue star will be swapped with another random uncollected star in the course (if applicable).",
        .longDescription    = "This will act like collecting a yellow star normally, and will not punish the player with the effects of collecting a blue star (as long as there are still yellow stars remaining in the course). This patch will take effect only when a star swap can been made.",
    },
    [CHAOS_PATCH_COIN_FLIP] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = TRUE,

        .activatedInitFunc  = chs_act_coin_flip,
        .conditionalFunc    = chs_cond_coin_flip,

        .name               = "One-Star Coin Flip",
        .shortDescription   = "Flip a coin! If heads, gain a random unobtained star. If tails, lose a star in your possession...",
    },

// Gravity Modifiers
    [CHAOS_PATCH_GRAVITY_DECREASE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_GRAVITY_INCREASE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 10,

        .conditionalFunc    = chs_cond_gravity_decrease,
        .activatedInitFunc  = chs_act_gravity_decrease,
        .deactivationFunc   = chs_deact_gravity_decrease,

        .name               = "Moon Gravity",
        .shortDescription   = "Decrease Mario's gravity by 37.5%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_GRAVITY_DECREASE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .conditionalFunc    = chs_cond_gravity_increase,
        .activatedInitFunc  = chs_act_gravity_increase,
        .deactivationFunc   = chs_deact_gravity_increase,

        .name               = "Ultra Gravity",
        .shortDescription   = "Increase Mario's gravity by 37.5%.",
    },

// Health Modifiers
    [CHAOS_PATCH_NOHEAL_HEARTS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 15,
        .durationHard       = 20,
        .durationImpossible = 25,

        .name               = "Heartbreak",
        .shortDescription   = "Spinning hearts no longer heal Mario.",
    },
    [CHAOS_PATCH_NOHEAL_WATER] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 15,
        .durationHard       = 18,
        .durationImpossible = 22,

        .name               = "Watered Down",
        .shortDescription   = "Water no longer heals Mario.",
    },
    [CHAOS_PATCH_NOHEAL_COINS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 12,
        .durationHard       = 15,
        .durationImpossible = 18,

        .conditionalFunc    = chs_cond_noheal_coins,

        .name               = "Unaffordable Health Care",
        .shortDescription   = "Coins no longer heal Mario.",
    },
    [CHAOS_PATCH_HEALING_BONUS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 7,

        .conditionalFunc    = chs_cond_healing_bonus,

        .name               = "Healing Bonus",
        .shortDescription   = "Coins heal twice as much health.",
    },
    [CHAOS_PATCH_HEALTH_DRAIN] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_HEALTH_GAIN,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .conditionalFunc    = chs_cond_health_drain,
        .frameUpdateFunc    = chs_update_health_drain,

        .name               = "Bleeding Out",
        .shortDescription   = "Mario will slowly lose health over time.",
    },
    [CHAOS_PATCH_HEALTH_GAIN] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_HEALTH_DRAIN,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 8,

        .conditionalFunc    = chs_cond_health_gain,
        .frameUpdateFunc    = chs_update_health_gain,

        .name               = "Bleeding In",
        .shortDescription   = "Mario will slowly gain health over time.",
    },
    [CHAOS_PATCH_HEALTH_UP] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_HEALTH_DOWN,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_health_up,
        .activatedInitFunc  = chs_act_health_up,
        .deactivationFunc   = chs_deact_health_up,

        .name               = "Health Up",
        .shortDescription   = "Health maximum increases by one.",
    },
    [CHAOS_PATCH_HEALTH_DOWN] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_HEALTH_UP,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_health_down,
        .activatedInitFunc  = chs_act_health_down,
        .deactivationFunc   = chs_deact_health_down,

        .name               = "Health Down",
        .shortDescription   = "Health maximum decreases by one.",
    },
    [CHAOS_PATCH_NO_FALL_DAMAGE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 12,

        .conditionalFunc    = chs_cond_no_fall_damage,

        .name               = "Long Fall Boots",
        .shortDescription   = "Mario is immune to fall damage.",
    },
    [CHAOS_PATCH_NO_LAVA_DAMAGE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 4,

        .conditionalFunc    = chs_cond_no_lava_damage,

        .name               = "Extreme Thermal Shoes",
        .shortDescription   = "Mario is immune to lava and freezing water (and may run on it normally).",
    },
    [CHAOS_PATCH_TEMPORARY_INVINCIBILITY] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 12,

        .levelInitFunc     = chs_lvlinit_temporary_invincibility,

        .name               = "Temporary Invincibility",
        .shortDescription   = "Mario cannot take damage for 30 seconds upon entering a new level.",
    },
    [CHAOS_PATCH_INSTAKILL_SQUISH] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,

        .name               = "Squishma",
        .shortDescription   = "Mario will instantly die upon taking squish damage.",
    },
    [CHAOS_PATCH_INSTAKILL_GOOMBA] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,

        .name               = "Gamer Goombas",
        .shortDescription   = "Goombas can now insta-kill Mario.",
    },
    [CHAOS_PATCH_INSTAKILL_LAVA] = {
        .durationType       = CHAOS_DURATION_INFINITE, // TODO: Nerf? This only ever affects LLL, SL, and BitFS so infinite seems appropriate, but most people don't really realize that on instinct...
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,

        .conditionalFunc    = chs_cond_instakill_lava,

        .name               = "The Lavas of Hell",
        .shortDescription   = "Mario will instantly die upon touching lava or freezing water.",
    },
    [CHAOS_PATCH_EXTRADAMAGE_ENEMIES] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = TRUE,
        .duration           = 16,
        .durationHard       = 20,
        .durationImpossible = 24,

        .conditionalFunc    = chs_cond_extradamage_enemies,
        .activatedInitFunc  = chs_act_extradamage_enemies,
        .deactivationFunc   = chs_deact_extradamage_enemies,

        .name               = "Defense Drop",
        .shortDescription   = "Mario takes +1 damage from all standard enemies.",
    },
    [CHAOS_PATCH_EXTRADAMAGE_LAVA] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = TRUE,
        .duration           = 16,
        .durationHard       = 20,
        .durationImpossible = 24,

        .conditionalFunc    = chs_cond_extradamage_lava,
        .activatedInitFunc  = chs_act_extradamage_lava,
        .deactivationFunc   = chs_deact_extradamage_lava,

        .name               = "Can't Beat the Heat",
        .shortDescription   = "Mario takes +1 additional damage from lava and fire.",
    },
    [CHAOS_PATCH_BREATH_BOOST] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 12,

        .name               = "Breath Boost",
        .shortDescription   = "Mario loses health more slowly underwater.",
    },
    [CHAOS_PATCH_SHIELD] = {
        .durationType       = CHAOS_DURATION_USE_COUNT,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = TRUE,
        .duration           = 2,

        .conditionalFunc    = chs_cond_shield,

        .name               = "Ligma Shield",
        .shortDescription   = "Ignore the next damage Mario takes.",
    },
    [CHAOS_PATCH_RANDOM_INVINCIBILITY] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,

        .frameUpdateFunc    = chs_update_random_invincibility,
        .activatedInitFunc  = chs_act_random_invincibility,

        .name               = "Windfall",
        .shortDescription   = "At random, gain 20 seconds of invincibility (watch for Mario's flashing animation!)",
    },
    [CHAOS_PATCH_A_BUTTON_CHALLENGE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 6,
        .durationImpossible = 7,

        .conditionalFunc    = chs_cond_a_button_challenge,

        .name               = "A Button Challenge",
        .shortDescription   = "Lose 1 slice of health each time the A button is pressed. How many A presses do you really need to win?",
    },
    [CHAOS_PATCH_DAMAGE_LOTTERY] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .conditionalFunc    = chs_cond_damage_lottery,

        .name               = "Damage Lottery",
        .shortDescription   = "Lose a random amount of health whenever Mario takes damage, between 0 and the max number of health slices.",
        .longDescription    = "Insta-kill patches are given priority over this patch, but this overrides other health-related patches. This patch ignores damage from water or poison.",
    },

// Coin Modifiers
    [CHAOS_PATCH_DOUBLE_COINS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 12,

        .levelInitFunc     = chs_lvlinit_double_coins,

        .name               = "Coin Rush",
        .shortDescription   = "Coin values will be doubled for 30 seconds upon entering a new level.",
    },
    [CHAOS_PATCH_100C_DECREASE_LV2] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = TRUE,
        .duration           = 10,

        .conditionalFunc    = chs_cond_100c_decrease_lv2,
        .activatedInitFunc  = chs_act_100c_decrease_lv2,
        .deactivationFunc   = chs_deact_100c_decrease_lv2,

        .name               = "Coin Star Sale",
        .shortDescription   = "Reduce the price of the 100 coin star by 10 coins.",
    },
    [CHAOS_PATCH_100C_DECREASE_LV3] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = TRUE,
        .duration           = 10,

        .conditionalFunc    = chs_cond_100c_decrease_lv3,
        .activatedInitFunc  = chs_act_100c_decrease_lv3,
        .deactivationFunc   = chs_deact_100c_decrease_lv3,

        .name               = "Coin Star Bargain",
        .shortDescription   = "Reduce the price of the 100 coin star by 25 coins.",
    },
    [CHAOS_PATCH_PAY2WIN] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 8,
        .durationImpossible = 10,

        .name               = "Pay to Win",
        .shortDescription   = "Stars are only collectible when Mario has at least 30 coins.",
    },
    [CHAOS_PATCH_6_RED_COINS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 12,

        .name               = "Ready for Redsies",
        .shortDescription   = "Red coin stars only require 6 red coins.",
    },
    [CHAOS_PATCH_SONIC_SIMULATOR] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 15,

        .conditionalFunc    = chs_cond_sonic_simulator,

        .name               = "Sonic Simulator",
        .shortDescription   = "Coins represent Mario's health. Just like in Sonic, Mario will drop all of his coins upon taking damage.",
    },
    [CHAOS_PATCH_BLUECOIN_LOTTERY] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 8,

        .name               = "Blue Coin Lottery",
        .shortDescription   = "Test your luck! Every blue coin is a 4% chance to win big!",
        .longDescription    = "This patch will expire the instant a blue coin lottery has been won. Use this wisely!"
    },
    [CHAOS_PATCH_COIN_SIZE] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_coin_size,

        .name               = "Coins That Double in Size",
        .shortDescription   = "Doubles the size of yellow coins.",
    },
    [CHAOS_PATCH_SCARED_COINS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .name               = "Sca-red Coins",
        .shortDescription   = "Red coins will try to avoid being collected.",
    },
    [CHAOS_PATCH_COIN_MAGNET] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 10,

        .name               = "Money Magnet",
        .shortDescription   = "Yellow and Blue coins will find you more attractive.",
    },


// Random Griefing
    [CHAOS_PATCH_RANDOM_SLEEP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 12,
        .durationHard       = 16,
        .durationImpossible = 20,

        .conditionalFunc    = chs_cond_random_sleep,
        .activatedInitFunc  = chs_act_random_sleep,
        .frameUpdateFunc    = chs_update_random_sleep,

        .name               = "Narcolepsy",
        .shortDescription   = "Mario is feeling a little tired today. He may decide on his own to go to sleep."
    },
    [CHAOS_PATCH_RANDOM_SHOCK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .disableForHardcore = TRUE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .conditionalFunc    = chs_cond_random_shock,
        .activatedInitFunc  = chs_act_random_shock,
        .frameUpdateFunc    = chs_update_random_shock,

        .name               = "Shocked to My Core",
        .shortDescription   = "Get shocked at random intervals."
    },
    [CHAOS_PATCH_RANDOM_BURN] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .disableForHardcore = TRUE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .activatedInitFunc  = chs_act_random_burn,
        .frameUpdateFunc    = chs_update_random_burn,
        .conditionalFunc    = chs_cond_random_burn,

        .name               = "Sick Burn",
        .shortDescription   = "Get burned at random intervals."
    },
    [CHAOS_PATCH_RANDOM_BLIND] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .disableForHardcore = TRUE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .activatedInitFunc  = chs_act_random_blind,
        .frameUpdateFunc    = chs_update_random_blind,

        .name               = "Blindfolded Speedrun",
        .shortDescription   = "Get blinded for 5 seconds periodically."
    },
    [CHAOS_PATCH_RANDOM_DIALOGUE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 12,
        .durationHard       = 16,
        .durationImpossible = 20,

        .activatedInitFunc  = chs_act_random_dialogue,
        .frameUpdateFunc    = chs_update_random_dialogue,

        .name               = "Hey! Listen!",
        .shortDescription   = "Receive extra dialogue periodically."
    },
    [CHAOS_PATCH_KAIZO_BLOCKS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 12,
        .durationHard       = 15,
        .durationImpossible = 18,

        .activatedInitFunc  = chs_act_kaizo_blocks,
        .frameUpdateFunc    = chs_update_kaizo_blocks,

        .name               = "Super Mario Maker",
        .shortDescription   = "My little brother made this level, I hope you enjoy! (Randomly Spawn Kaizo Blocks.)"
    },
    [CHAOS_PATCH_BATTLEFIELD] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 20,
        .durationHard       = 24,
        .durationImpossible = 28,

        .name               = "Bring Back Level Intro Text",
        .shortDescription   = "Wow! You're smack in the middle of the battlefield.",
    },
    [CHAOS_PATCH_TROLL_SOUNDS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 16,
        .durationHard       = 18,
        .durationImpossible = 20,

        .activatedInitFunc  = chs_act_troll_sounds,
        .frameUpdateFunc    = chs_update_troll_sounds,

        .name               = "Hearing Things",
        .shortDescription   = "I don't think this one actually does anything.",
    },
    [CHAOS_PATCH_RED_LIGHT] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .disableForHardcore = TRUE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .activatedInitFunc  = chs_act_red_light,
        .conditionalFunc    = chs_cond_red_light,
        .frameUpdateFunc    = chs_update_red_light,

        .name               = "Red Light Green Light",
        .shortDescription   = "When you see 'Red Light' appear on screen, stop using the controller! Just like Mr. Beast's Squid Game.",
    },
    [CHAOS_PATCH_COSMIC_RAYS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .activatedInitFunc  = chs_act_cosmic_rays,
        .conditionalFunc    = chs_cond_cosmic_rays,
        .frameUpdateFunc    = chs_update_cosmic_rays,

        .name               = "Cosmic Rays",
        .shortDescription   = "Now you too can experience the famous TTC upwarp!",
        .longDescription    = "Was it a solar flare? A tilted cartridge? A one in a million glitch? Probably not!",
    },
    [CHAOS_PATCH_RANDOM_BUTTON_PRESSES] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 11,

        .frameUpdateFunc    = chs_update_random_button_presses,

        .name               = "Random Button Presses",
        .shortDescription   = "Periodically trigger a random button press.",
    },

// Movement Modifiers
    [CHAOS_PATCH_LOSEMOVE_BREAKDANCE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 20,
        .durationHard       = 25,
        .durationImpossible = 30,

        .name               = "Dance Break",
        .shortDescription   = "Mario can no longer breakdance.",
    },
    [CHAOS_PATCH_LOSEMOVE_LEDGE_GRAB] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 14,

        .name               = "Sore Arms",
        .shortDescription   = "Mario can no longer ledge grab.",
    },
    [CHAOS_PATCH_LOSEMOVE_KICK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 14,

        .name               = "Leg Pain",
        .shortDescription   = "Mario can no longer kick.",
    },
    [CHAOS_PATCH_LOSEMOVE_SIDEFLIP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 14,

        .name               = "Unsimple Flips",
        .shortDescription   = "Mario can no longer sideflip.",
    },
    [CHAOS_PATCH_LOSEMOVE_LONG_JUMP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 14,
        
        .conditionalFunc    = chs_cond_losemove_longjump,

        .name               = "Lazy Leaper",
        .shortDescription   = "Mario can no longer long jump.",
    },
    [CHAOS_PATCH_LOSEMOVE_BACKFLIP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 14,

        .name               = "Flipped Off",
        .shortDescription   = "Mario can no longer backflip.",
    },
    [CHAOS_PATCH_LOSEMOVE_WALL_KICK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 14,

        .conditionalFunc    = chs_cond_losemove_wall_kick,

        .name               = "Wall Kicks Won't Work",
        .shortDescription   = "Mario can no longer wall kick.",
    },
    [CHAOS_PATCH_LOSEMOVE_DOUBLE_JUMP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .name               = "Double Trouble",
        .shortDescription   = "Mario can no longer double jump (and by extension, triple jump).",
    },
    [CHAOS_PATCH_BRAWL_TRIPPING] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .disableForHardcore = TRUE,
        .duration           = 9,
        .durationHard       = 12,
        .durationImpossible = 15,

        .frameUpdateFunc    = chs_update_brawl_tripping,

        .name               = "Brawl Mode",
        .shortDescription   = "Mario trips randomly sometimes. What a clumsy guy!",
    },
    [CHAOS_PATCH_GALAXY_SPIN] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 12,

        .frameUpdateFunc    = chs_update_galaxy_twirl,

        .name               = "First Introduced in Galaxy 2",
        .shortDescription   = "Press A while in midair to do a spin jump!",
    },
    [CHAOS_PATCH_GROUND_POUND_DIVE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 20,

        .name               = "Do the Odyssey",
        .shortDescription   = "Press B while ground pounding to do a dive jump!",
    },
    [CHAOS_PATCH_GROUND_POUND_JUMP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 20,

        .name               = "Bounce Back",
        .shortDescription   = "Press A shortly after landing from a ground pound to do a bigger jump!",
    },
    [CHAOS_PATCH_STICKY_WALL_JUMP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 10,

        .conditionalFunc    = chs_check_sticky_walljump,

        .name               = "Sticky Wall Jump",
        .shortDescription   = "Stick to walls! This really makes you FEEL like Spider-Man!",
    },
    [CHAOS_PATCH_BETA] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 8,

        .name               = "Beta",
        .shortDescription   = "YAAHAA!!! Triple Jumps will now cause you to twirl!",
        .longDescription    = "HUD by robichu, sounds from the Internet archive (this will be more credit than any beta hacker will ever give). pls give me bps for betah triple jump"
    },
    [CHAOS_PATCH_HARDER_LONG_JUMPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .conditionalFunc    = chs_cond_harder_longjumps,

        .name               = "Long Jump Lottery",
        .shortDescription   = "Get a Ground Pound instead of a Long Jump at random!",
    },
    [CHAOS_PATCH_FORWARDS_BLJ] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 10,

        .conditionalFunc    = chs_cond_forwards_blj,

        .name               = "Forwards BLJ",
        .shortDescription   = "Uncap the speed of the long jump, even while moving forwards. It's just like a BLJ, but without the B!",
    },

// Object Spawners
    [CHAOS_PATCH_GREEN_DEMON] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 1,
        .durationHard       = 2,
        .durationImpossible = 2,

        .conditionalFunc    = chs_cond_green_demon,
        .activatedInitFunc  = chs_act_green_demon,
        .areaInitFunc       = chs_area_init_green_demon,

        .name               = "Green Demon",
        .shortDescription   = "Of course this one's in the game. Spawn a poison 1-UP mushroom that chases Mario.",
    },
    [CHAOS_PATCH_BULLET_HELL] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 11,

        .areaInitFunc       = chs_area_init_bullet_hell,
        .frameUpdateFunc    = chs_update_bullet_hell,

        .name               = "Bullet Hell",
        .shortDescription   = "Every 30 seconds, spawn a number of Bullet Bills around Mario. Pay attention to hear the warning sound!",
    },
    [CHAOS_PATCH_SPAWN_ON_SHELL] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 5,

        .levelInitFunc     = chs_lvlinit_spawn_on_shell,
        .frameUpdateFunc    = chs_lvlupdate_spawn_on_shell,

        .name               = "Shell Spawn",
        .shortDescription   = "Start riding on a shell upon entering a new course.",
    },
    [CHAOS_PATCH_DOUBLE_CHERRY] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = TRUE,
        .duration           = 6,
        .durationHard       = 6,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_cherry_clone,
        .activatedInitFunc  = chs_create_cherry_clone,
        .deactivationFunc   = chs_remove_cherry_clone,
        .areaInitFunc       = chs_init_cherry_clones_after_warp,

        .name               = "Double Cherry",
        .shortDescription   = "Spawn an additional Mario. While this is largely positive, keeping track of it may be disorienting!",
        .longDescription    = "Each cherry clone has 1 health point, and will take your hits until they run out, after which you will take normal damage. If the \"real\" player gets hit or interacts with anything, its soul will be transferred to a clone. By the end of this, which Mario will be the real Mario?",
    },
    [CHAOS_PATCH_COSMIC_CLONES] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 11,

        .activatedInitFunc  = chs_act_cosmic_clones,
        .conditionalFunc    = chs_cond_cosmic_clones,
        .deactivationFunc   = chs_deact_cosmic_clones,
        .areaInitFunc       = chs_area_init_cosmic_clones,
        .frameUpdateFunc    = chs_update_cosmic_clones,

        .name               = "Cosmic Clones",
        .shortDescription   = "Spawns a trail of cosmic Marios that copy your every move!",
    },

// Visual Modifiers
    [CHAOS_PATCH_NO_Z_BUFFER] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 4,
        .durationImpossible = 5,

        .conditionalFunc    = chs_cond_no_zbuffer,

        .name               = "Geometry Freakout",
        .shortDescription   = "Geometry can now render itself in whatever order it wants (sensory warning for most setups!)",
    },
    [CHAOS_PATCH_INVERTED_Z_BUFFER] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 2,
        .durationHard       = 3,
        .durationImpossible = 4,

        .conditionalFunc    = chs_cond_inverted_zbuffer,

        .name               = "Inside-Out",
        .shortDescription   = "Geometry that is further from the camera will render on top of closer geometry. Quite the perspective!",
    },
    [CHAOS_PATCH_UPSIDE_DOWN_CAMERA] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 2,
        .durationHard       = 3,
        .durationImpossible = 4,

        .name               = "Australia Mode",
        .shortDescription   = "Flips the camera upside-down so you can experience what it's like to be from the land down under.",
    },
    [CHAOS_PATCH_DECREASED_FOV] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_INCREASED_FOV,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_decreased_fov,

        .name               = "Narrow Focus",
        .shortDescription   = "Dramatically decrease Mario's field of view.",
    },
    [CHAOS_PATCH_INCREASED_FOV] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_DECREASED_FOV,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_increased_fov,

        .name               = "Fish Eyes",
        .shortDescription   = "Dramatically increase Mario's field of view.",
    },
    [CHAOS_PATCH_CONFUSED_OBJECTS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .name               = "Confused Entities",
        .shortDescription   = "Most objects are (sometimes) facing the wrong way!",
    },
    [CHAOS_PATCH_UPSIDE_DOWN_OBJECTS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 9,

        .name               = "Upside-Down Entities",
        .shortDescription   = "Most objects are (sometimes) floating upside down!",
    },
    [CHAOS_PATCH_NO_SKYBOX] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 8,
        .durationImpossible = 10,

        .conditionalFunc    = chs_cond_no_skybox,

        .name               = "The Sky is...Gone?",
        .shortDescription   = "Meh, who needed it anyway...",
    },
    [CHAOS_PATCH_20_FPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 2,
        .durationHard       = 3,
        .durationImpossible = 4,

        .conditionalFunc    = chs_cond_20_fps,

        .name               = "20 FPS",
        .shortDescription   = "Alright, who plugged in the 5 dollar capture card?",
    },
    [CHAOS_PATCH_45_FPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_45_fps,

        .name               = "45 FPS",
        .shortDescription   = "My internet's living, I'm adding frames, Grandma's living, she's adding frames...",
        .longDescription    = "This maaaaaaay not run at a consistent 45 FPS while running on hardware. Whether that makes the game easier or harder is for the player to decide.",
    },
    [CHAOS_PATCH_60_FPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 7,

        .conditionalFunc    = chs_cond_60_fps,

        .name               = "60 FPS (Double Time!)",
        .shortDescription   = "It's like playing the game two times at the same time!",
        .longDescription    = "This will probably not be hitting 60 FPS most of the time while running on hardware...hopefully console players enjoy the choppy framerates!",
    },
    [CHAOS_PATCH_TOP_DOWN_CAMERA] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 9,

        .conditionalFunc    = chs_topdown_check,

        .name               = "Top-Down Camera",
        .shortDescription   = "Now you're playing Zelda! (without the items) (without the story) (without the combat) (without the",
    },
    [CHAOS_PATCH_MIRROR_MODE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 9,
        .durationHard       = 12,
        .durationImpossible = 15,

        .activatedInitFunc  = chs_mq_init,
        .deactivationFunc   = chs_mq_deinit,

        .name               = "Master Quest",
        .shortDescription   = "Mirror Mario has broken out! Flip the game horizontally and play from his perspective!",
        .longDescription    = "For a full version of this mod, check out Super Mario 64 Master Quest by mountainflaw on RomHacking.com!",
    },
    [CHAOS_PATCH_LOW_RESOLUTION] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_low_resolution,

        .name               = "Potato Graphics",
        .shortDescription   = "Now you're gaming like it's the 70's!",
    },
    [CHAOS_PATCH_BIG_HEAD] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 10,
        .durationImpossible = 13,

        .name               = "Big Head Mode",
        .shortDescription   = "Mario might have a college degree, but now he looks the part!",
    },
    [CHAOS_PATCH_ORTHO] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 9,

        .conditionalFunc    = chs_cond_ortho,

        .name               = "Orthographic Mode",
        .shortDescription   = "A whole new perspective! Or really a lack of one...",
    },
    [CHAOS_PATCH_DIZZY_OBJECTS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_dizzy_objects,

        .name               = "Dizzy Objects",
        .shortDescription   = "Spins objects around in a circle (visuals only). I hope you don't get motion sick!",
    },
    [CHAOS_PATCH_CAMERA_LAG] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 4,
        .durationImpossible = 5,

        .conditionalFunc    = chs_cond_camera_lag,

        .name               = "Lakitu's Bad Hangover",
        .shortDescription   = "Lakitu had a little too much to drink last night. He may struggle to keep the camera on Mario!",
    },
    [CHAOS_PATCH_DARKNESS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_darkness,

        .name               = "Hello Darkness, My Old Friend",
        .shortDescription   = "Dim the screen substantially.",
    },
    [CHAOS_PATCH_CLOWN_VOMIT] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 7,
        .durationImpossible = 8,

        .name               = "Clown Vomit",
        .shortDescription   = "Makes the world a bit more colorful.",
    },

// Time Limit
#define CHS_TIME_LIMIT_DURATION            10
#define CHS_TIME_LIMIT_DURATION_HARD       13
#define CHS_TIME_LIMIT_DURATION_IMPOSSIBLE 16
    [CHAOS_PATCH_TIME_LIMIT] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = CHS_TIME_LIMIT_DURATION,
        .durationHard       = CHS_TIME_LIMIT_DURATION_HARD,
        .durationImpossible = CHS_TIME_LIMIT_DURATION_IMPOSSIBLE,

        .levelInitFunc      = chs_level_init_time_limit,
        .frameUpdateFunc    = chs_update_time_limit,

        .name               = "Speedy Comet",
        .shortDescription   = "Better hurry up! You now have 3 minutes to collect each star.",
    },
    [CHAOS_PATCH_LOWER_TIME_LIMIT] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = TRUE,
        .duration           = CHS_TIME_LIMIT_DURATION,
        .durationHard       = CHS_TIME_LIMIT_DURATION_HARD,
        .durationImpossible = CHS_TIME_LIMIT_DURATION_IMPOSSIBLE,

        .conditionalFunc    = chs_cond_lower_time_limit,
        .activatedInitFunc  = chs_act_lower_time_limit,
        .deactivationFunc   = chs_deact_lower_time_limit,

        .name               = "Speedy Comet++",
        .shortDescription   = "Reset the active duration for the Speedy Comet patch, and lower the time limit by 30 seconds.",
    },

// Cheats
    [CHAOS_PATCH_L_TO_LEVITATE] = {
        .durationType       = CHAOS_DURATION_USE_COUNT,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = TRUE,
        .duration           = 2,

        .conditionalFunc    = chs_cond_l_to_levitate,
        .frameUpdateFunc    = chs_update_l_to_levitate,

        .name               = "L to Levitate",
        .shortDescription   = "Press L to levitate! Each use lasts until the next time Mario touches the ground.",
    },
    [CHAOS_PATCH_DEBUG_FREE_MOVE] = {
        .durationType       = CHAOS_DURATION_USE_COUNT,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = TRUE,
        .duration           = 1,
        
        .conditionalFunc    = chs_cond_debug_free_move,
        .frameUpdateFunc    = chs_update_debug_free_move,

        .name               = "Debug Free Move",
        .shortDescription   = "Enables one use of debug free move. Check details screen for how to use!",
        .longDescription    = "Press D-Pad Up to activate, and A to exit. While using debug free move, D-Pad Up and D-Pad Down may be used to move up or down. Hold B to increase your movement speed, or Z to decrease it. With enough speed, you may be able to clip through walls! Mario is (mostly) invulnerable while this mode is active.",
    },

// Chaos Modifiers
    [CHAOS_PATCH_REMOVE_NEGATIVE_PATCH] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = TRUE,
        
        .conditionalFunc    = chs_cond_remove_negative_patch,
        .activatedInitFunc  = chs_act_remove_negative_patch,

        .name               = "Show Me Mercy",
        .shortDescription   = "Deactivate one of the currently active negative patches at random.",
    },
    [CHAOS_PATCH_ADD_SELECTABLE_PATCH] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_REMOVE_SELECTABLE_PATCH,
        .severity           = 3,
        .isStackable        = TRUE,
        .affectsPatchSelect = TRUE,
        .duration           = 8,
        
        .conditionalFunc    = chs_cond_add_selectable_patch,

        .name               = "The More, The Merrier",
        .shortDescription   = "Add one more option to the patch selection menu.",
    },
    [CHAOS_PATCH_REMOVE_SELECTABLE_PATCH] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_ADD_SELECTABLE_PATCH,
        .severity           = 3,
        .isStackable        = TRUE,
        .affectsPatchSelect = TRUE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 7,
        
        .conditionalFunc    = chs_cond_remove_selectable_patch,

        .name               = "Little Choice",
        .shortDescription   = "Remove an option from the patch selection menu.",
    },
    [CHAOS_PATCH_LUCKY_CHARM] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .affectsPatchSelect = TRUE,
        .duration           = 7,
        
        .conditionalFunc    = chs_cond_lucky_charm,

        .name               = "Lucky Charm",
        .shortDescription   = "Doubles chances of encountering a Lucky event.",
    },
    [CHAOS_PATCH_UNLUCKY_CHARM] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .affectsPatchSelect = TRUE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 7,
        
        .conditionalFunc    = chs_cond_unlucky_charm,

        .name               = "Unlucky Charm",
        .shortDescription   = "Doubles chances of encountering Bad Luck and Chaos events.",
    },
    [CHAOS_PATCH_UNEVENTFUL] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .affectsPatchSelect = TRUE,
        .duration           = 8,
        
        .conditionalFunc    = chs_cond_uneventful,

        .name               = "Uneventful",
        .shortDescription   = "Bad Luck and Chaos events will no longer show up (except for duplicate stars).",
    },
    [CHAOS_PATCH_POSITIVE_EXTENSION] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .affectsPatchSelect = TRUE,
        .duration           = 6,
        
        .conditionalFunc    = chs_cond_positive_extension,

        .name               = "Positive Extension",
        .shortDescription   = "New positive effects will last 50% longer (rounded up).",
    },
    [CHAOS_PATCH_NEGATIVE_EXTENSION] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .affectsPatchSelect = TRUE,
        .duration           = 6,
        
        .conditionalFunc    = chs_cond_negative_extension,

        .name               = "Negative Extension",
        .shortDescription   = "New negative effects will last 50% longer (rounded up).",
    },

// Speed Modifiers
    [CHAOS_PATCH_PUSH_BACK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = TRUE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .frameUpdateFunc    = chs_update_push_back,
        .conditionalFunc    = chs_cond_push_back,

        .name               = "A Little Push in the Wrong Direction",
        .shortDescription   = "Mario will constantly get pushed backwards a little.",
    },
    [CHAOS_PATCH_WALKIES] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 4,
        .durationImpossible = 5,

        .conditionalFunc    = chs_cond_walkies,

        .name               = "Walkies",
        .shortDescription   = "Slows Mario down to walking speed. This is a Walkies level now.",
        .longDescription    = "Lowers Mario's intended magnitude. This essentially acts as though you are never holding the joystick more than the amount it takes for Mario to go from walking to running, which is more or less the SM64 equivalent of not using the run button.",
    },
    [CHAOS_PATCH_SPEED_TAX] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 9,

        .activatedInitFunc  = chs_act_speed_tax,
        .frameUpdateFunc    = chs_update_speed_tax,
        .conditionalFunc    = chs_cond_speed_tax,

        .name               = "Speed Tax",
        .shortDescription   = "If Mario goes too fast he will start needing to pay coins. If he can't pay, he will take damage!",
    },

// Cap Effects
    [CHAOS_PATCH_WING_CAP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 5,

        .frameUpdateFunc    = chs_update_wing_cap,

        .name               = "I Believe I Can Fly",
        .shortDescription   = "Mario gains an infinite wing cap effect!",
    },
    [CHAOS_PATCH_VANISH_CAP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 18,

        .name               = "Wall Hacks",
        .shortDescription   = "Mario can now walk through all vanish cap walls, even without wearing the cap!",
    },
    [CHAOS_PATCH_REMOVE_CAP] = {
        .durationType       = CHAOS_DURATION_ONCE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = TRUE,

        .activatedInitFunc  = chs_act_remove_cap,
        .conditionalFunc    = chs_cond_remove_cap,

        .name               = "No Cap",
        .shortDescription   = "Remove Mario's cap and place it in a valid stage somewhere (see details screen for more info).",
        .longDescription    = "Mario takes an additional 50% of damage on average without his cap. Possible cap placement locations include Shifting Sand Land (Klepto), Snowman's Land (Snowman near the cannon), and Tall, Tall Mountain (Ukiki by the waterfall). Do you have access to these stages yet?",
    },
    [CHAOS_PATCH_DISABLE_CAPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 11,

        .conditionalFunc    = chs_cond_disable_caps,

        .name               = "Boxing Day",
        .shortDescription   = "Temporarily disables all cap blocks.",
    },

// Input Modifiers
    [CHAOS_PATCH_BUTTON_BROKEN_A] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 7,
        
        .conditionalFunc    = chs_cond_button_broken_a,

        .name               = "The Atrocious A",
        .shortDescription   = "The A button's contact has an unreliable connection. Maybe you should get a better controller...",
    },
    [CHAOS_PATCH_BUTTON_BROKEN_B] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 7,
        .durationImpossible = 8,
        
        .conditionalFunc    = chs_cond_button_broken_b,

        .name               = "The Busted B",
        .shortDescription   = "The B button is finnicky and doesn't work sometimes.",
    },
    [CHAOS_PATCH_BUTTON_BROKEN_Z] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 7,
        
        .conditionalFunc    = chs_cond_button_broken_z,

        .name               = "The Zonked Z",
        .shortDescription   = "The Z button is completely nonfunctional (even for menus!)",
    },
    [CHAOS_PATCH_BUTTON_BROKEN_C] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 7,

        .conditionalFunc    = chs_cond_button_broken_c,

        .name               = "Camera Malfunction",
        .shortDescription   = "All C buttons are completely nonfunctional (even for menus!)",
    },
    [CHAOS_PATCH_SWAPPED_ZR_AB] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 4,
        .durationImpossible = 5,
        
        .conditionalFunc    = chs_cond_swapped_zr_ab,

        .name               = "Bizarro Buttons",
        .shortDescription   = "The A and B buttons will be swapped with Z and R respectively (including for menus!)",
    },
    [CHAOS_PATCH_INVERTED_CAMERA_X] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .conditionalFunc    = chs_cam_invert_x_check,

        .name               = "Camera Reversal",
        .shortDescription   = "Invert the X axis of the camera (i.e. C-Left and C-Right).",
    },
    [CHAOS_PATCH_INVERTED_STICK_X] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 1,
        .durationHard       = 2,
        .durationImpossible = 2,

        .name               = "UNO Reverse Card",
        .shortDescription   = "Invert the X axis of the joystick when controlling Mario (i.e. left and right).",
    },
    [CHAOS_PATCH_INVERTED_STICK_Y] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 2,
        .durationHard       = 2,
        .durationImpossible = 3,

        .name               = "Stick Shift",
        .shortDescription   = "Invert the Y axis of the joystick when controlling Mario (i.e. forward and backward).",
    },
    [CHAOS_PATCH_INPUT_LAG] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 3,
        .durationHard       = 4,
        .durationImpossible = 5,

        .activatedInitFunc  = chs_act_input_lag,

        .name               = "Nintendo Wii Online Mode",
        .shortDescription   = "Truly the most playable and responsive experience!",
    },
    [CHAOS_PATCH_SM64_DS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .name               = "Super Mario 64 DS",
        .shortDescription   = "Locks analog input to 8 directions. It's beans for your mouth, for your floor!",
    },

// Size Modifiers
    [CHAOS_PATCH_MARIO_BIG] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .negationId         = CHAOS_PATCH_MARIO_SMALL,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_mario_big,
        .activatedInitFunc  = chs_act_mario_big,
        .deactivationFunc   = chs_deact_mario_big,

        .name               = "One Makes You Larger",
        .shortDescription   = "Increase Mario's size by 12.5% (additive).",
    },
    [CHAOS_PATCH_MARIO_SMALL] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .negationId         = CHAOS_PATCH_MARIO_BIG,
        .severity           = 1,
        .isStackable        = TRUE,

        .conditionalFunc    = chs_cond_mario_small,
        .activatedInitFunc  = chs_act_mario_small,
        .deactivationFunc   = chs_deact_mario_small,

        .name               = "One Makes You Small",
        .shortDescription   = "Decrease Mario's size by 12.5% (additive).",
    },

// Lethal damages
[CHAOS_PATCH_LETHAL_BONK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .name               = "Realistic Concussions",
        .shortDescription   = "Careful not to hit your head! Bonking now results in severe brain trauma.",
},
[CHAOS_PATCH_LETHAL_FALL_DAMAGE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 8,

        .conditionalFunc    = chs_cond_lethal_fall_damage,

        .name               = "Realistic Fall Damage",
        .shortDescription   = "Falling will now instantly kill Mario.",
},

// Audio Modifiers
    [CHAOS_PATCH_INVERTED_SOUND] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .name               = "Audio Inversion",
        .shortDescription   = "The game will sound upside-down!",
    },
    [CHAOS_PATCH_REVERB] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 12,

        .activatedInitFunc  = chs_act_reverb,
        .deactivationFunc   = chs_deact_reverb,

        .name               = "High-Fidelity Reverb",
        .shortDescription   = "For that more immersive experience (even more epic in caves!)",
    },
    [CHAOS_PATCH_RANDOMIZED_MUSIC] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 15,

        .conditionalFunc    = chs_cond_randomized_music,

        .name               = "Randomize Music",
        .shortDescription   = "Let's listen to something else.",
    },
    [CHAOS_PATCH_AUDIO_DELAY] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 9,

        .name               = "Bluetooth Lag",
        .shortDescription   = "The game's audio response will be substantially delayed.",
    },
    [CHAOS_PATCH_CHILL_OUT] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 5,

        .conditionalFunc    = chs_cond_chill_out,

        .name               = "Chill Out!",
        .shortDescription   = "Reduce your stress levels by slowing/pitching down the background music. That's so chill!",
    },
    [CHAOS_PATCH_MAD_MUSICAL_MESS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 6,

        .conditionalFunc    = chs_cond_mad_musical_mess,

        .name               = "Mad Musical Mess",
        .shortDescription   = "The seasoned musicians behind SM64 want a break. We pulled in some random fellas off the streets to sub in!",
    },

// Miscellaneous Modifiers
    [CHAOS_PATCH_MARIO_INVISIBLE] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 10,

        .conditionalFunc    = chs_cond_invisible,

        .name               = "Potion of Invisibility",
        .shortDescription   = "Mario is now invisible. Not even his shadow can be seen anymore!",
    },
    [CHAOS_PATCH_SIGNREAD_FAR] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 11,

        .name               = "Tutorial Mode",
        .shortDescription   = "New to SM64? We'll make extra sure you don't miss any tutorial signs by increasing their range!",
    },
    [CHAOS_PATCH_ONE_HIT_WONDER] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_one_hit_wonder,

        .name               = "One-Hit Wonder",
        .shortDescription   = "Mario will die instantly upon taking any form of damage (other than from swimming or poison).",
    },
    [CHAOS_PATCH_NO_HUD] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 7,
        .durationHard       = 9,
        .durationImpossible = 11,

        .name               = "Cinematic Mode",
        .shortDescription   = "Gets rid of that pesky HUD so you can appreciate the beauty of Super Mario 64.",
    },
    [CHAOS_PATCH_FORCED_MARIO_CAM] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 7,

        .conditionalFunc    = chs_cond_forced_mario_cam,

        .name               = "Up Close and Personal",
        .shortDescription   = "Forces Mario Cam (close-up view).",
    },
    [CHAOS_PATCH_BOWSER_THROWS] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,

        .name               = "Code That Makes You Miss Bowser Throws",
        .shortDescription   = "They have that in this game, I swear!",
    },
    [CHAOS_PATCH_AD_BREAK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 15,
        .durationImpossible = 18,

        .frameUpdateFunc    = chs_update_serve_ads,
        .activatedInitFunc  = chs_act_serve_ads,

        .name               = "Ad Breaks",
        .shortDescription   = "As if the cash for making this romhack wasn't enough, have some ads too!",
    },
    [CHAOS_PATCH_ALL_STARS_SELECTABLE] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,

        .name               = "Sequence Break",
        .shortDescription   = "All course acts are now selectable, and may be completed in any order.",
    },
    [CHAOS_PATCH_MIRACLE_NORMAL] = {
        .durationType       = CHAOS_DURATION_USE_COUNT,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = TRUE,
        .disableForHardcore = TRUE,
        .duration           = 2,

        .frameUpdateFunc    = chs_update_miracle,
        .conditionalFunc    = chs_cond_miracle_normal,

        .name               = "The Miracle Happened",
        .shortDescription   = "The next two times Mario dies, he is revived as if nothing ever happened.",
        .longDescription    = "On death, Mario will be instantly healed back to full health instead of being thrown out of the level. He will maintain his level and coin progress."
    },
    [CHAOS_PATCH_MIRACLE_HARDCORE] = {
        .durationType       = CHAOS_DURATION_USE_COUNT,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 1,

        .frameUpdateFunc    = chs_update_miracle,
        .conditionalFunc    = chs_cond_miracle_hardcore,

        .name               = "The Miracle Happened",
        .shortDescription   = "The next time Mario dies, he is revived as if nothing ever happened. Essentially a free lifeline for your run!",
        .longDescription    = "On death, Mario will be instantly healed back to full health instead of being thrown out of the level. He will maintain his level and coin progress."
    },
    [CHAOS_PATCH_DISABLE_FADE_WARPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 15,
        .durationHard       = 18,
        .durationImpossible = 20,

        .name               = "Fading Fantasy",
        .shortDescription   = "Fading teleports are all deactivated.",
    },
    [CHAOS_PATCH_LUIGI] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 10,

        .conditionalFunc    = chs_cond_luigi,
        .frameUpdateFunc    = chs_update_luigi,
        .deactivationFunc   = chs_deact_luigi,

        .name               = "L is Real 2401",
        .shortDescription   = "You can now play as Luigi.",
        .longDescription    = "Luigi model originally created by Vl-Tone / Starxxon."
    },
    [CHAOS_PATCH_WEAK_BOSSES] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 8,

        .name               = "Not So Bossy",
        .shortDescription   = "All bosses die in one hit (except for during the final Bowser fight).",
    },
    [CHAOS_PATCH_QUICKTIME] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 10,
        .durationHard       = 12,
        .durationImpossible = 15,

        .conditionalFunc    = chs_cond_quicktime,
        .frameUpdateFunc    = chs_update_quicktime,
        .activatedInitFunc  = chs_act_quicktime,

        .name               = "Quicktime Events",
        .shortDescription   = "Follow the buttons in the order prompted on screen or die. Directed by David Cage.",
    },
    [CHAOS_PATCH_PLATFORM_MISPLACEMENT] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 12,
        .durationHard       = 18,
        .durationImpossible = 24,

        .name               = "Platform Misplacement",
        .shortDescription   = "Moving platforms will move Mario more than usual.",
    },
    [CHAOS_PATCH_CLIMBING_BOOTS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,

        .conditionalFunc    = chs_cond_climbing_boots,

        .name               = "Climbing Boots",
        .shortDescription   = "Mario can run on slippery floors and up steep slopes without sliding.",
    },
    [CHAOS_PATCH_SLIPPERY_FLOORS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 4,
        .durationHard       = 5,
        .durationImpossible = 6,

        .conditionalFunc    = chs_cond_slippery_floors,

        .name               = "Wet Floor",
        .shortDescription   = "All floors will be slippery.",
    },
    [CHAOS_PATCH_MARTH_GRAB] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 8,

        // NOTE: There is a bug with this where Bowser grabs don't work correctly, which makes Bowser 3 almost impossible.
        // It is however hilarious on Bowser 1 and 2, so this will only get star restricted after 60 rather than properly fixed.
        .conditionalFunc    = chs_cond_marth_grab,

        .name               = "Marth Grab",
        .shortDescription   = "I mean, have you SEEN his grab range??",
    },
    [CHAOS_PATCH_NOCLIP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 7,
        .durationImpossible = 9,
        
        .frameUpdateFunc    = chs_update_noclip,

        .name               = "No Clip",
        .shortDescription   = "Walls are just a complete suggestion. Who needs 'em?",
    },
    [CHAOS_PATCH_RAINBOW_STARS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 20,

        .name               = "Rainbow Stars",
        .shortDescription   = "Shoutouts to SimpleFlips.",
    },
    [CHAOS_PATCH_SHUFFLE_OBJECTS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .disableForHardcore = TRUE,
        .duration           = 6,
        .durationHard       = 7,
        .durationImpossible = 8,

        .areaInitFunc       = chs_start_shuffle,
        .frameUpdateFunc    = chs_shuffle_objects,

        .name               = "Shuffle Object Positions",
        .shortDescription   = "Kaze WISHES tornado did this!",
    },
    [CHAOS_PATCH_RANDOMIZE_WARPS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 5,
        .durationHard       = 6,
        .durationImpossible = 8,

        .conditionalFunc    = chs_cond_randomize_warps,

        .name               = "Randomize Warps",
        .shortDescription   = "I'm bored of this level. Can we go to a different one?",
    },
    [CHAOS_PATCH_FROM_BEYOND_THE_GRAVE] = {
        .durationType       = CHAOS_DURATION_INFINITE,
        .effectType         = CHAOS_EFFECT_POSITIVE,
        .severity           = 1,
        .isStackable        = FALSE,

        .name               = "From Beyond the Grave",
        .shortDescription   = "Mario may still recover from coins or collect stars, even when he has no health left (pre-death animation only).",
        .longDescription    = "This may be useful in specific situations, such as flying around with the wing cap while dead."
    },
    [CHAOS_PATCH_DVD] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 6,
        .durationHard       = 8,
        .durationImpossible = 10,

        .activatedInitFunc  = chs_init_dvd,

        .name               = "DVD Logo",
        .shortDescription   = "Maybe this time it will hit the corner?",
    },
    [CHAOS_PATCH_NUMBER_BLINDNESS] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .affectsPatchSelect = TRUE,
        .duration           = 8,
        .durationHard       = 10,
        .durationImpossible = 12,

        .activatedInitFunc  = chs_act_number_blindness,
        .deactivationFunc   = chs_deact_number_blindness,

        .name               = "Number Blindness",
        .shortDescription   = "All numbers will display as '?'. Hope you remember those patch durations!",
    },
};
