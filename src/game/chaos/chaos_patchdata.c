
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

        .name              = "---",
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

        .name              = "---",
        .shortDescription  = "@BFBFBF--No negative effect.@--------",
        .longDescription   = NULL,
    },

/**************************************************************************************************************************/

// Lives Modifiers
    [CHAOS_PATCH_LIVES_INCREASE_LV1] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
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
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_decrease_lv3,
        .activatedInitFunc = chs_act_lives_decrease_lv3,

        .name              = "Life Apocalypse",
        .shortDescription  = "Decrease Mario's remaining lives by 10.",
    },

// Star Modifiers
    [CHAOS_PATCH_STARS_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_increase_lv2,

        .name              = "One-Star Offer",
        .shortDescription  = "Receive a random star. This is NOT guaranteed to be a yellow star.",
    },
    [CHAOS_PATCH_STARS_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_increase_lv3,

        .name              = "Two-Star Treat",
        .shortDescription  = "Receive two random stars. These are NOT guaranteed to be yellow stars.",
    },
    [CHAOS_PATCH_STARS_INCREASE_GUARANTEE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
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
        .severity          = 2,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_decrease_lv2,

        .name              = "Two-Star Trick",
        .shortDescription  = "Mark two random stars as uncollected. These are NOT guaranteed to be stars you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_decrease_lv3,

        .name              = "Three-Star Threat",
        .shortDescription  = "Mark three random stars as uncollected. These are NOT guaranteed to be stars you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_GUARANTEE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_stars_decrease_guarantee,
        .activatedInitFunc = chs_act_stars_decrease_guarantee,

        .name              = "Two-Star Tradeoff",
        .shortDescription  = "Lose two random stars currently in your possession.",
    },

// Gravity Modifiers
    [CHAOS_PATCH_GRAVITY_DECREASE_LV1] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
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

        .name              = "Health Drown",
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
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 10,
        .conditionalFunc   = chs_cond_health_drain,
        .frameUpdateFunc   = chs_update_health_drain,
        .negationId        = CHAOS_PATCH_HEALTH_GAIN,

        .name              = "Bleeding Out",
        .shortDescription  = "Mario will slowly lose health over time",
    },
    [CHAOS_PATCH_HEALTH_GAIN] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = FALSE,
        .duration          = 10,
        .conditionalFunc   = chs_cond_health_gain,
        .frameUpdateFunc   = chs_update_health_gain,
        .negationId        = CHAOS_PATCH_HEALTH_DRAIN,

        .name              = "Bleeding In",
        .shortDescription  = "Mario will slowly gain health over time",
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

        .name              = "Pay To Win",
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

// Random Griefing
    [CHAOS_PATCH_RANDOM_SLEEP] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 1,
        .isStackable        = FALSE,
        .duration           = 20,

        .activatedInitFunc  = chs_act_random_sleep,
        .frameUpdateFunc    = chs_update_random_sleep,

        .name               = "Narcolepsy",
        .shortDescription   = "Mario is feeling a little tired today. He may decide on his own to go to sleep."
    },
    [CHAOS_PATCH_RANDOM_SHOCK] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 2,
        .isStackable        = FALSE,
        .duration           = 15,

        .activatedInitFunc  = chs_act_random_shock,
        .frameUpdateFunc    = chs_update_random_shock,

        .name               = "Shocked to My Core",
        .shortDescription   = "Get shocked at random intervals."
    },
    [CHAOS_PATCH_RANDOM_BURN] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 15,

        .activatedInitFunc  = chs_act_random_burn,
        .frameUpdateFunc    = chs_update_random_burn,

        .name               = "Sick Burn",
        .shortDescription   = "Get burned at random intervals."
    },
    [CHAOS_PATCH_RANDOM_BLIND] = {
        .durationType       = CHAOS_DURATION_STARS,
        .effectType         = CHAOS_EFFECT_NEGATIVE,
        .severity           = 3,
        .isStackable        = FALSE,
        .duration           = 9,

        .activatedInitFunc  = chs_act_random_blind,
        .frameUpdateFunc    = chs_update_random_blind,

        .name               = "Blindfolded Speedrun",
        .shortDescription   = "Get blinded for 15 seconds periodically."
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
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 12,

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
        .shortDescription  = "Mario trips randomly sometimes.",
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
        .duration          = 3,

        .areaInitFunc      = chs_area_init_green_demon,

        .name              = "Green Demon",
        .shortDescription  = "Of course this one's in the game. Spawn a poison 1-UP mushroom that chases Mario.",
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
//  Time Limit
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
        .deactivationFunc  = chs_deact_lower_time_limit,

        .name              = "Speedy Comet++",
        .shortDescription  = "Resets the duration for the speedy comet and lowers the time limit by 15 seconds",
    },
// Miscellaneous Modifiers
    [CHAOS_PATCH_MARIO_INVISIBLE] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 12,

        .name              = "Potion of Invisibility",
        .shortDescription  = "Mario is now invisible. His shadow is the only visible part of him that remains.",
    },
    [CHAOS_PATCH_SIGNREAD_FAR] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 15,

        .name              = "Tutorial Mode",
        .shortDescription  = "New to SM64? We'll make extra sure you don't miss any tutorial signs by increasing their range!",
    },
    [CHAOS_PATCH_ONE_HIT_WONDER] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

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
        .shortDescription  = "Forces Mario cam.",
    },
    [CHAOS_PATCH_BOWSER_THROWS] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = FALSE,
        .duration          = 1,

        .name              = "Code That Makes You Miss Bowser Throws",
        .shortDescription  = "They have that in this game, I swear.",
    },

    [CHAOS_PATCH_BLUECOIN_LOTTERY] = {
        .durationType      = CHAOS_DURATION_INFINITE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = FALSE,
        .duration          = 6,

        .name              = "Blue Coin Lottery",
        .shortDescription  = R"(Test your luck! Every blue coin is a 1% chance to win big!)",
    }
};
