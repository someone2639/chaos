
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

        .name              = "-",
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

        .name              = "-",
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

        .name              = "Increase Lives (+5)",
        .shortDescription  = "Increase Mario's total lives by 5.",
    },
    [CHAOS_PATCH_LIVES_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_increase_lv2,
        .activatedInitFunc = chs_act_lives_increase_lv2,

        .name              = "Increase Lives (+10)",
        .shortDescription  = "Increase Mario's total lives by 10.",
    },
    [CHAOS_PATCH_LIVES_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_increase_lv3,
        .activatedInitFunc = chs_act_lives_increase_lv3,

        .name              = "Increase Lives (+15)",
        .shortDescription  = "Increase Mario's total lives by 15.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV1] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = TRUE,
    
        .conditionalFunc   = chs_cond_lives_decrease_lv1,
        .activatedInitFunc = chs_act_lives_decrease_lv1,

        .name              = "Decrease Lives (+5)",
        .shortDescription  = "Decrease Mario's remaining lives by 5.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_decrease_lv2,
        .activatedInitFunc = chs_act_lives_decrease_lv2,

        .name              = "Decrease Lives (+10)",
        .shortDescription  = "Decrease Mario's remaining lives by 10.",
    },
    [CHAOS_PATCH_LIVES_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_lives_decrease_lv3,
        .activatedInitFunc = chs_act_lives_decrease_lv3,

        .name              = "Decrease Lives (+15)",
        .shortDescription  = "Decrease Mario's remaining lives by 15.",
    },

// Star Modifiers
    [CHAOS_PATCH_STARS_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_increase_lv2,

        .name              = "+1 Random Star",
        .shortDescription  = "Receive a random star. This is NOT guaranteed to be a yellow star.",
    },
    [CHAOS_PATCH_STARS_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_increase_lv3,

        .name              = "+2 Random Stars",
        .shortDescription  = "Receive two random stars. These are NOT guaranteed to be yellow stars.",
    },
    [CHAOS_PATCH_STARS_INCREASE_GUARANTEE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_stars_increase_guarantee,
        .activatedInitFunc = chs_act_stars_increase_guarantee,

        .name              = "+1 Uncollected Star",
        .shortDescription  = "Receive a random star not already in your possession.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_decrease_lv2,

        .name              = "-1 Random Star",
        .shortDescription  = "Mark a random star as uncollected. This is NOT guaranteed to be a star you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = TRUE,

        .activatedInitFunc = chs_act_stars_decrease_lv3,

        .name              = "-2 Random Stars",
        .shortDescription  = "Mark two random stars as uncollected. These are NOT guaranteed to be stars you have.",
    },
    [CHAOS_PATCH_STARS_DECREASE_GUARANTEE] = {
        .durationType      = CHAOS_DURATION_ONCE,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = TRUE,

        .conditionalFunc   = chs_cond_stars_decrease_guarantee,
        .activatedInitFunc = chs_act_stars_decrease_guarantee,

        .name              = "-1 Collected Star",
        .shortDescription  = "Lose a random star currently in your possession.",
    },

// Gravity Modifiers
    [CHAOS_PATCH_GRAVITY_DECREASE_LV1] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 10,

        .activatedInitFunc = NULL, // TODO:
        .deactivationFunc  = NULL, // TODO:

        .name              = "Decreased Gravity (-15%)",
        .shortDescription  = "Decrease Mario's gravity by 15%.",
    },
    [CHAOS_PATCH_GRAVITY_DECREASE_LV2] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 10,

        .activatedInitFunc = NULL, // TODO:
        .deactivationFunc  = NULL, // TODO:

        .name              = "Decreased Gravity (-30%)",
        .shortDescription  = "Decrease Mario's gravity by 30%.",
    },
    [CHAOS_PATCH_GRAVITY_DECREASE_LV3] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_POSITIVE,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,

        .activatedInitFunc = NULL, // TODO:
        .deactivationFunc  = NULL, // TODO:

        .name              = "Decreased Gravity (-50%)",
        .shortDescription  = "Decrease Mario's gravity by 50%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE_LV1] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 1,
        .isStackable       = TRUE,
        .duration          = 10,

        .activatedInitFunc = NULL, // TODO:
        .deactivationFunc  = NULL, // TODO:

        .name              = "Increased Gravity (+15%)",
        .shortDescription  = "Increase Mario's gravity by 15%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE_LV2] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 2,
        .isStackable       = TRUE,
        .duration          = 10,

        .activatedInitFunc = NULL, // TODO:
        .deactivationFunc  = NULL, // TODO:

        .name              = "Increased Gravity (+30%)",
        .shortDescription  = "Increase Mario's gravity by 30%.",
    },
    [CHAOS_PATCH_GRAVITY_INCREASE_LV3] = {
        .durationType      = CHAOS_DURATION_STARS,
        .effectType        = CHAOS_EFFECT_NEGATIVE,
        .severity          = 3,
        .isStackable       = TRUE,
        .duration          = 10,

        .activatedInitFunc = NULL, // TODO:
        .deactivationFunc  = NULL, // TODO:

        .name              = "Increased Gravity (+50%)",
        .shortDescription  = "Increase Mario's gravity by 50%.",
    },
};
