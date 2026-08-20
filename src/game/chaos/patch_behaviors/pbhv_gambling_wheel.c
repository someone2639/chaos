#include "actors/group0.h"
#include "behavior_data.h"
#include "object_fields.h"
#include "sounds.h"
#include "seq_ids.h"
#include "audio/external.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"
#include "game/camera.h"
#include "game/chaos_tutorial.h"
#include "game/debug.h"
#include "game/object_helpers.h"
#include "game/print.h"
#include "game/level_update.h"
#include "game/object_list_processor.h"
#include "engine/behavior_script.h"
#include "game/patch_selection_ui.h"
#include "game/interaction.h"
#include "game/behavior_actions.h"
#include "game/rumble_init.h"
#include "game/save_file.h"
#include "game/fasttext.h"
#include "course_table.h"

#define WHEEL_OFFSET_Y (8.0f)
#define FLIPPER_OFFSET_Y (91.0f)
#define WHEEL_SELECTOR_OFFSET (6.5f)
#define WHEEL_FLIPPER_PEG_COLLISION_OFFSET (2.75f)
#define WHEEL_FLIPPER_ROTATION_MAX (52.5f)

#define FLASH_FRAME_COUNT 10

#define GAMBLING_WHEEL_SLOT_COUNT 16
#define GAMBLING_WHEEL_PEG_COUNT (GAMBLING_WHEEL_SLOT_COUNT * 2)
#define WEDGE_ROTATIONAL_FRAC (360.0f / GAMBLING_WHEEL_SLOT_COUNT)
#define PEG_ROTATIONAL_FRAC (360.0f / GAMBLING_WHEEL_PEG_COUNT)

enum GamblingWheelSize {
    GWHEEL_SIZE_INVALID = 0,

    GWHEEL_SIZE_SMALL   = 1,
    GWHEEL_SIZE_MEDIUM  = 2,
    GWHEEL_SIZE_LARGE   = 3,
    GWHEEL_SIZE_HUGE    = 4,
};

enum GamblingWheelColorType {
    GWHEEL_TYPE_JACKPOT,
    GWHEEL_TYPE_POSITIVE,
    GWHEEL_TYPE_NEUTRAL,
    GWHEEL_TYPE_NEGATIVE,
    GWHEEL_TYPE_CATASTROPHIC,
};

enum GamblingWheelOptionType {
    GWHEEL_OPT_ANY           = 0,

    GWHEEL_OPT_JPT_START     = 1,

    /* GWHEEL_TYPE_JACKPOT */
    GWHEEL_OPT_JPT_3_STARS   = GWHEEL_OPT_JPT_START,
    GWHEEL_OPT_JPT_12_LIVES,
    GWHEEL_OPT_JPT_RANK_3,

    GWHEEL_OPT_JPT_END,
    GWHEEL_OPT_POS_START     = GWHEEL_OPT_JPT_END,

    /* GWHEEL_TYPE_POSITIVE */
    GWHEEL_OPT_POS_1_STAR    = GWHEEL_OPT_POS_START,
    GWHEEL_OPT_POS_5_LIVES,
    GWHEEL_OPT_POS_EXTEND,
    GWHEEL_OPT_POS_RANK_1,
    GWHEEL_OPT_POS_RANK_2,
    GWHEEL_OPT_POS_RANK_3,

    GWHEEL_OPT_POS_END,
    GWHEEL_OPT_NEU_START     = GWHEEL_OPT_POS_END,

    /* GWHEEL_TYPE_NEUTRAL */
    GWHEEL_OPT_NEU_NONE      = GWHEEL_OPT_NEU_START,

    GWHEEL_OPT_NEU_END,
    GWHEEL_OPT_NEG_START     = GWHEEL_OPT_NEU_END,

    /* GWHEEL_TYPE_NEGATIVE */
    GWHEEL_OPT_NEG_1_STAR    = GWHEEL_OPT_NEG_START,
    GWHEEL_OPT_NEG_3_LIVES,
    GWHEEL_OPT_NEG_EXTEND,
    GWHEEL_OPT_NEG_RANK_1,
    GWHEEL_OPT_NEG_RANK_2,

    GWHEEL_OPT_NEG_END,
    GWHEEL_OPT_CAT_START     = GWHEEL_OPT_NEG_END,

    /* GWHEEL_TYPE_CATASTROPHIC */
    GWHEEL_OPT_CAT_2_STARS   = GWHEEL_OPT_CAT_START,
    GWHEEL_OPT_CAT_6_LIVES,
    GWHEEL_OPT_CAT_RANK_3,

    GWHEEL_OPT_CAT_END,

    GWHEEL_OPT_COUNT = GWHEEL_OPT_CAT_END,
};

enum GamblingWheelActions {
    GWHEEL_ACT_ANIMATE_SPAWN,
    GWHEEL_ACT_SPAWN_WAIT_FOR_BUTTON,
    GWHEEL_ACT_START_WHEEL_SPIN,
    GWHEEL_ACT_STOP_WHEEL_SPIN,
    GWHEEL_ACT_ANIMATE_RESULT,
    GWHEEL_ACT_END_ANIMATE,

    GWHEEL_ACT_COUNT,
};

enum GamblingWheelAPrompts {
    GWHEEL_APROMPT_NONE,
    GWHEEL_APROMPT_NEXT,
    GWHEEL_APROMPT_SPIN,
};

struct GamblingWheelSizeProps {
    const Gfx *wedgeDl;
    const Gfx *borderDl;
};

struct GamblingWheelColorProps {
    const u8 idleColor[4];
    const u8 highlightedColor[4];
    const s32 menuSoundId;
    const s32 marioSoundId;
};

struct GamblingWheelOptionProps {
    const enum GamblingWheelOptionType id;
    const f32 weight;
    const Gfx *textureDlBig;
    const Gfx *textureDlSmall;

    u8 (*conditionalFunc)(void);
    void (*activationFunc)(void);

    const char *description;
};

struct GamblingWheelParams {
    const enum GamblingWheelSize size;
    const enum GamblingWheelColorType type;
    const enum GamblingWheelOptionType forcedOptionType;
    enum GamblingWheelOptionType generatedOptionType;
    f32 rotationOffsetStart;
    f32 rotationOffsetEnd;
    f32 iconScale;
};

struct GamblingWheelEntries {
    struct GamblingWheelParams *entries;
    const s32 count;
};

static s32 activeWheelIndex = 0;
static f32 wheelRotation = 0.0f;
static f32 spawnWheelRotation = 0.0f;

static u8 normalBorderColor[4] = {0x2F, 0x2F, 0x1F, 0xFF};
static u8 highlightedBorderColor[4] = {0xCF, 0xBF, 0x3F, 0xFF};
static u8 pegColor[4] = {0x7F, 0x7F, 0x7F, 0xFF};

static enum GamblingWheelActions wheelAction = GWHEEL_ACT_ANIMATE_SPAWN;
static enum GamblingWheelActions prevWheelAction = GWHEEL_ACT_ANIMATE_SPAWN;
static u32 wheelTimer = 0;

static enum GamblingWheelAPrompts aPrompt = GWHEEL_APROMPT_NONE;
static u8 shouldRenderAsHighlighted = FALSE;
static u8 shouldRenderHighlighedColor = TRUE;
static s32 lastPeg = -1;
static f32 lastFlipperRotationPerc = 0.0f;
static f32 lastFlipperRotationMomentum = 0.0f;
static s32 mtxLayers = 0;

static u32 framesToAccelerate = 0;
static u32 framesToSpinAtMax = 0;
static f32 wheelSpinSpeed = 0.0f;
static f32 wheelFrictionFactor = 0.99f;
static f32 wheelLinearDecelerationFactor = 0.5f;

static u8 textTransparency = 0;
static u8 highlightFlashFrames = 0;

static u32 introTextFrames = 0;

static u8 gwheel_type_cond_rank_3_positive_hard(void) { return (gChaosDifficulty >= CHAOS_DIFFICULTY_HARD); }
static u8 gwheel_type_cond_rank_3_positive(void)      { return (gChaosDifficulty <  CHAOS_DIFFICULTY_HARD); }
static u8 gwheel_type_cond_rank_3_negative(void)      { return (gChaosDifficulty >  CHAOS_DIFFICULTY_EASY); }

static u8 gwheel_type_cond_lives_increase_12(void) { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives <= (100 - 12)); }
static u8 gwheel_type_cond_lives_increase_5(void)  { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives <= (100 - 6)); }
static u8 gwheel_type_cond_lives_decrease_3(void)  { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives >= 3); }
static u8 gwheel_type_cond_lives_decrease_6(void)  { return (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gMarioState->numLives >= 6); }

static u8 gwheel_type_cond_positive_extend(void) {
    // Confirm at least one extendable positive patch active
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const struct ChaosPatch *patch = &gChaosPatches[gChaosActiveEntries[i].id];
        if (patch->effectType == CHAOS_EFFECT_POSITIVE && patch->durationType == CHAOS_DURATION_STARS) {
            return TRUE;
        }
    }

    return FALSE;
}

static u8 gwheel_type_cond_negative_extend(void) {
    // Confirm at least one extendable negative patch active
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const struct ChaosPatch *patch = &gChaosPatches[gChaosActiveEntries[i].id];
        if (patch->effectType == CHAOS_EFFECT_NEGATIVE && patch->durationType == CHAOS_DURATION_STARS) {
            return TRUE;
        }
    }

    return FALSE;
}

static void gwheel_type_act_stars_increase_1(void) {
    add_uncollected_star();
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
static void gwheel_type_act_stars_increase_3(void) {
    add_uncollected_star();
    add_uncollected_star();
    add_uncollected_star();
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
static void gwheel_type_act_stars_decrease_1(void) {
    remove_collected_star();
}
static void gwheel_type_act_stars_decrease_2(void) {
    remove_collected_star();
    remove_collected_star();
}

static void gwheel_type_act_lives_increase_12(void) {
    gMarioState->numLives += 12;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
static void gwheel_type_act_lives_increase_5(void) {
    gMarioState->numLives += 5;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
static void gwheel_type_act_lives_decrease_3(void) {
    gMarioState->numLives -= 3;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}
static void gwheel_type_act_lives_decrease_6(void) {
    gMarioState->numLives -= 6;
    save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, TRUE);
}

static void gwheel_type_act_positive_extend(void) {
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const struct ChaosPatch *patch = &gChaosPatches[gChaosActiveEntries[i].id];
        if (patch->effectType == CHAOS_EFFECT_POSITIVE && patch->durationType == CHAOS_DURATION_STARS) {
            // Extend patch timer by 2
            gChaosActiveEntries[i].remainingDuration += 2;
        }
    }

    // Sort all patches
    chaos_sort_active_patches();
}

static void gwheel_type_act_negative_extend(void) {
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const struct ChaosPatch *patch = &gChaosPatches[gChaosActiveEntries[i].id];
        if (patch->effectType == CHAOS_EFFECT_NEGATIVE && patch->durationType == CHAOS_DURATION_STARS) {
            // Extend patch timer by 1
            gChaosActiveEntries[i].remainingDuration += 1;
        }
    }

    // Sort all patches
    chaos_sort_active_patches();
}

static void gwheel_type_act_rank_1_positive(void)      { chs_activate_random_pos_neg_patch_of_severity(1, CHAOS_EFFECT_POSITIVE, TRUE, 3, CHAOS_DURATION_STARS); }
static void gwheel_type_act_rank_2_positive(void)      { chs_activate_random_pos_neg_patch_of_severity(2, CHAOS_EFFECT_POSITIVE, TRUE, 3, CHAOS_DURATION_STARS); }
static void gwheel_type_act_rank_3_positive(void)      { chs_activate_random_pos_neg_patch_of_severity(3, CHAOS_EFFECT_POSITIVE, TRUE, 3, CHAOS_DURATION_STARS); }
static void gwheel_type_act_rank_3_positive_hard(void) { chs_activate_random_pos_neg_patch_of_severity(3, CHAOS_EFFECT_POSITIVE, TRUE, 5, CHAOS_DURATION_STARS); }
static void gwheel_type_act_rank_1_negative(void)      { chs_activate_random_pos_neg_patch_of_severity(1, CHAOS_EFFECT_NEGATIVE, TRUE, 2, CHAOS_DURATION_STARS); }
static void gwheel_type_act_rank_2_negative(void)      { chs_activate_random_pos_neg_patch_of_severity(2, CHAOS_EFFECT_NEGATIVE, TRUE, 2, CHAOS_DURATION_STARS); }
static void gwheel_type_act_rank_3_negative(void)      { chs_activate_random_pos_neg_patch_of_severity(3, CHAOS_EFFECT_NEGATIVE, TRUE, 2, CHAOS_DURATION_STARS); }

const struct GamblingWheelSizeProps gamblingWheelSizeParams[] = {
    [GWHEEL_SIZE_INVALID] = {
        .wedgeDl = NULL,
        .borderDl = NULL,
    },
    [GWHEEL_SIZE_SMALL] = {
        .wedgeDl = gambling_wheel_dl_wedge_6_25,
        .borderDl = gambling_wheel_dl_border_6_25,
    },
    [GWHEEL_SIZE_MEDIUM] = {
        .wedgeDl = gambling_wheel_dl_wedge_12_5,
        .borderDl = gambling_wheel_dl_border_12_5,
    },
    [GWHEEL_SIZE_LARGE] = {
        .wedgeDl = gambling_wheel_dl_wedge_18_75,
        .borderDl = gambling_wheel_dl_border_18_75,
    },
    [GWHEEL_SIZE_HUGE] = {
        .wedgeDl = gambling_wheel_dl_wedge_25,
        .borderDl = gambling_wheel_dl_border_25,
    },
};

const struct GamblingWheelColorProps gamblingWheelColorParams[] = {
    [GWHEEL_TYPE_JACKPOT] = {
        .idleColor        = {0xCF, 0xCF, 0x5F, 0xFF},
        .highlightedColor = {0xFF, 0xFF, 0xAF, 0xFF},
        .marioSoundId     = SOUND_MARIO_YAHOO,
        .menuSoundId      = SOUND_MENU_RIGHT_ANSWER,
    },
    [GWHEEL_TYPE_POSITIVE] = {
        .idleColor        = {0x00, 0x7F, 0x00, 0xFF},
        .highlightedColor = {0x3F, 0xBF, 0x3F, 0xFF},
        .marioSoundId     = SOUND_MARIO_HAHA,
        .menuSoundId      = SOUND_MENU_RIGHT_ANSWER,
    },
    [GWHEEL_TYPE_NEUTRAL] = {
        .idleColor        = {0x6F, 0x6F, 0x6F, 0xFF},
        .highlightedColor = {0xAF, 0xAF, 0xAF, 0xFF},
        .marioSoundId     = SOUND_MARIO_MAMA_MIA,
        .menuSoundId      = SOUND_MENU_CAMERA_BUZZ,
    },
    [GWHEEL_TYPE_NEGATIVE] = {
        .idleColor        = {0x8F, 0x00, 0x00, 0xFF},
        .highlightedColor = {0xCF, 0x3F, 0x3F, 0xFF},
        .marioSoundId     = SOUND_MARIO_ATTACKED,
        .menuSoundId      = SOUND_MENU_CAMERA_BUZZ,
    },
    [GWHEEL_TYPE_CATASTROPHIC] = {
        .idleColor        = {0x37, 0x00, 0x2F, 0xFF},
        .highlightedColor = {0x67, 0x1F, 0x57, 0xFF},
        .marioSoundId     = SOUND_MARIO_WAAAOOOW,
        .menuSoundId      = SOUND_MENU_CAMERA_BUZZ,
    },
};

const struct GamblingWheelOptionProps gamblingWheelOptionParams[] = {
    [GWHEEL_OPT_ANY] = {
        .id              = GWHEEL_OPT_ANY,
        .weight          = 0.0f,
        .textureDlBig    = NULL,
        .textureDlSmall  = NULL,

        .conditionalFunc = NULL,
        .activationFunc  = NULL,

        .description     = NULL,
    },

    /* GWHEEL_TYPE_JACKPOT */
    [GWHEEL_OPT_JPT_3_STARS] = {
        .id              = GWHEEL_OPT_JPT_3_STARS,
        .weight          = 1.25f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_3_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_increase_3,

        .description     = "Receive three random stars not already in your possession.",
    },
    [GWHEEL_OPT_JPT_12_LIVES] = {
        .id              = GWHEEL_OPT_JPT_12_LIVES,
        .weight          = 0.75f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_12_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_12_small,

        .conditionalFunc = gwheel_type_cond_lives_increase_12,
        .activationFunc  = gwheel_type_act_lives_increase_12,

        .description     = "Gain 12 lives.",
    },
    [GWHEEL_OPT_JPT_RANK_3] = {
        .id              = GWHEEL_OPT_JPT_RANK_3,
        .weight          = 1.125f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_3_small,

        .conditionalFunc = gwheel_type_cond_rank_3_positive_hard,
        .activationFunc  = gwheel_type_act_rank_3_positive_hard,

        .description     = "Gain a positive rank 3 patch that will last at most five stars.",
    },

    /* GWHEEL_TYPE_POSITIVE */
    [GWHEEL_OPT_POS_1_STAR] = {
        .id              = GWHEEL_OPT_POS_1_STAR,
        .weight          = 1.5f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_1_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_increase_1,

        .description     = "Receive a random star not already in your possession.",
    },
    [GWHEEL_OPT_POS_5_LIVES] = {
        .id              = GWHEEL_OPT_POS_5_LIVES,
        .weight          = 1.125f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_5_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_5_small,

        .conditionalFunc = gwheel_type_cond_lives_increase_5,
        .activationFunc  = gwheel_type_act_lives_increase_5,

        .description     = "Gain 5 lives.",
    },
    [GWHEEL_OPT_POS_EXTEND] = {
        .id              = GWHEEL_OPT_POS_EXTEND,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_timer_big,
        .textureDlSmall  = gambling_wheel_dl_icon_timer_small,

        .conditionalFunc = gwheel_type_cond_positive_extend,
        .activationFunc  = gwheel_type_act_positive_extend,

        .description     = "Extend all positive patch durations by two stars.",
    },
    [GWHEEL_OPT_POS_RANK_1] = {
        .id              = GWHEEL_OPT_POS_RANK_1,
        .weight          = 0.67f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_1_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_1_positive,

        .description     = "Gain a positive rank 1 patch that will last at most three stars.",
    },
    [GWHEEL_OPT_POS_RANK_2] = {
        .id              = GWHEEL_OPT_POS_RANK_2,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_2_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_2_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_2_positive,

        .description     = "Gain a positive rank 2 patch that will last at most three stars.",
    },
    [GWHEEL_OPT_POS_RANK_3] = {
        .id              = GWHEEL_OPT_POS_RANK_3,
        .weight          = 0.85f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_3_small,

        .conditionalFunc = gwheel_type_cond_rank_3_positive,
        .activationFunc  = gwheel_type_act_rank_3_positive,

        .description     = "Gain a positive rank 3 patch that will last at most three stars.",
    },

    /* GWHEEL_TYPE_NEUTRAL */
    [GWHEEL_OPT_NEU_NONE] = {
        .id              = GWHEEL_OPT_NEU_NONE,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_none_big,
        .textureDlSmall  = gambling_wheel_dl_icon_none_small,

        .conditionalFunc = NULL,
        .activationFunc  = NULL,

        .description     = "No effect.",
    },

    /* GWHEEL_TYPE_NEGATIVE */
    [GWHEEL_OPT_NEG_1_STAR] = {
        .id              = GWHEEL_OPT_NEG_1_STAR,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_m1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_m1_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_decrease_1,

        .description     = "Lose one random star currently in your possession.",
    },
    [GWHEEL_OPT_NEG_3_LIVES] = {
        .id              = GWHEEL_OPT_NEG_3_LIVES,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_m3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_m3_small,

        .conditionalFunc = gwheel_type_cond_lives_decrease_3,
        .activationFunc  = gwheel_type_act_lives_decrease_3,

        .description     = "Lose 3 lives.",
    },
    [GWHEEL_OPT_NEG_EXTEND] = {
        .id              = GWHEEL_OPT_NEG_EXTEND,
        .weight          = 0.75f,
        .textureDlBig    = gambling_wheel_dl_icon_timer_big,
        .textureDlSmall  = gambling_wheel_dl_icon_timer_small,

        .conditionalFunc = gwheel_type_cond_negative_extend,
        .activationFunc  = gwheel_type_act_negative_extend,

        .description     = "Extend all negative patch durations by one star.",
    },
    [GWHEEL_OPT_NEG_RANK_1] = {
        .id              = GWHEEL_OPT_NEG_RANK_1,
        .weight          = 0.85f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_1_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_1_negative,

        .description     = "Gain a negative rank 1 patch that will last at most two stars.",
    },
    [GWHEEL_OPT_NEG_RANK_2] = {
        .id              = GWHEEL_OPT_NEG_RANK_2,
        .weight          = 1.15f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_2_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_2_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_2_negative,

        .description     = "Gain a negative rank 2 patch that will last at most two stars.",
    },

    /* GWHEEL_TYPE_CATASTROPHIC */
    [GWHEEL_OPT_CAT_2_STARS] = {
        .id              = GWHEEL_OPT_CAT_2_STARS,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_m2_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_m2_small,

        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_decrease_2,

        .description     = "Lose two random stars currently in your possession.",
    },
    [GWHEEL_OPT_CAT_6_LIVES] = {
        .id              = GWHEEL_OPT_CAT_6_LIVES,
        .weight          = 0.85f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_m6_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_m6_small,

        .conditionalFunc = gwheel_type_cond_lives_decrease_6,
        .activationFunc  = gwheel_type_act_lives_decrease_6,

        .description     = "Lose 6 lives.",
    },
    [GWHEEL_OPT_CAT_RANK_3] = {
        .id              = GWHEEL_OPT_CAT_RANK_3,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_3_small,

        .conditionalFunc = gwheel_type_cond_rank_3_negative,
        .activationFunc  = gwheel_type_act_rank_3_negative,

        .description     = "Gain a negative rank 3 patch that will last at most two stars.",
    },
};

struct GamblingWheelParams gamblingWheel_0[] = {
    { /* 0 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_JACKPOT,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 1 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 2 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 3 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 4 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 5 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 6 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 7 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 8 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_CATASTROPHIC,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 9 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 10 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 11 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 12 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 13 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 14 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 15 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
};

struct GamblingWheelParams gamblingWheel_1[] = {
    { /* 0 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_JACKPOT,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 1 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 2 - 3 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 4 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 5 - 7 */
        .size             = GWHEEL_SIZE_LARGE,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 8 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_CATASTROPHIC,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 9 - 12 */
        .size             = GWHEEL_SIZE_HUGE,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 13 - 14 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 15 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
};

struct GamblingWheelParams gamblingWheel_2[] = {
    { /* 0 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_JACKPOT,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 1 - 3 */
        .size             = GWHEEL_SIZE_LARGE,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 4 - 7 */
        .size             = GWHEEL_SIZE_HUGE,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 8 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_CATASTROPHIC,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 9 - 12 */
        .size             = GWHEEL_SIZE_HUGE,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 13 - 15 */
        .size             = GWHEEL_SIZE_LARGE,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
};

struct GamblingWheelParams gamblingWheel_3[] = {
    { /* 0 - 1 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_JACKPOT,
        .forcedOptionType = GWHEEL_OPT_JPT_3_STARS,
    },
    { /* 2 - 3 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_NEG_1_STAR,
    },
    { /* 4 - 5 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_NEU_NONE,
    },
    { /* 6 - 7 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_POS_1_STAR,
    },
    { /* 8 - 9 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_CATASTROPHIC,
        .forcedOptionType = GWHEEL_OPT_CAT_2_STARS,
    },
    { /* 10 - 11 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_POS_1_STAR,
    },
    { /* 12 - 13 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_NEU_NONE,
    },
    { /* 14 - 15 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_NEG_1_STAR,
    },
};

struct GamblingWheelParams gamblingWheel_4[] = {
    { /* 0 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_JACKPOT,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 1 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 2 - 3 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 4 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 5 - 6 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 7 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 8 */
        .size             = GWHEEL_SIZE_SMALL,
        .type             = GWHEEL_TYPE_CATASTROPHIC,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 9 - 10 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEUTRAL,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 11 - 13 */
        .size             = GWHEEL_SIZE_LARGE,
        .type             = GWHEEL_TYPE_POSITIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
    { /* 14 - 15 */
        .size             = GWHEEL_SIZE_MEDIUM,
        .type             = GWHEEL_TYPE_NEGATIVE,
        .forcedOptionType = GWHEEL_OPT_ANY,
    },
};

const struct GamblingWheelEntries gamblingWheels[] = {
    {.entries = gamblingWheel_0, .count = ARRAY_COUNT(gamblingWheel_0)},
    {.entries = gamblingWheel_1, .count = ARRAY_COUNT(gamblingWheel_1)},
    {.entries = gamblingWheel_2, .count = ARRAY_COUNT(gamblingWheel_2)},
    {.entries = gamblingWheel_3, .count = ARRAY_COUNT(gamblingWheel_3)},
    {.entries = gamblingWheel_4, .count = ARRAY_COUNT(gamblingWheel_4)},
};

void chs_menuinit_gambling_wheel(void) {
    activeWheelIndex = random_u16() % ARRAY_COUNT(gamblingWheels);

    struct GamblingWheelParams *wheel = gamblingWheels[activeWheelIndex].entries;

    // Copy constant weights to modifiable weights
    f32 weights[GWHEEL_OPT_COUNT];
    for (enum GamblingWheelOptionType type = 0; type < GWHEEL_OPT_COUNT; type++) {
        weights[type] = gamblingWheelOptionParams[type].weight;

        if (gamblingWheelOptionParams[type].conditionalFunc != NULL && !gamblingWheelOptionParams[type].conditionalFunc()) {
            weights[type] = 0.0f;
        }
    }

    s32 index = 0;
    wheelRotation = ((WEDGE_ROTATIONAL_FRAC * wheel[0].size) / 2.0f) - WHEEL_SELECTOR_OFFSET - WHEEL_FLIPPER_PEG_COLLISION_OFFSET;
    if (wheelRotation < 0) {
        wheelRotation += 360.0f;
    }
    spawnWheelRotation = wheelRotation;

    // Update forced types and weights
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        struct GamblingWheelParams *wheelEntry = &wheel[i];
        wheelEntry->generatedOptionType = GWHEEL_OPT_NEU_NONE;

        if (wheelEntry->forcedOptionType != GWHEEL_OPT_ANY && weights[wheelEntry->forcedOptionType] > 0.0f) {
            wheelEntry->generatedOptionType = wheelEntry->forcedOptionType;
            weights[wheelEntry->forcedOptionType] *= 0.5f; // Cut odds of duplicate in half (for forced types only)
        }
    }

    // Generate remaining non-forced types
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        struct GamblingWheelParams *wheelEntry = &wheel[i];
        if (wheelEntry->forcedOptionType == GWHEEL_OPT_ANY || weights[wheelEntry->forcedOptionType] <= 0.0f) {
            enum GamblingWheelOptionType start;
            enum GamblingWheelOptionType end;
            f32 totalWeight = 0.0f;

            switch(wheelEntry->type) {
                case GWHEEL_TYPE_JACKPOT:
                    start = GWHEEL_OPT_JPT_START;
                    end = GWHEEL_OPT_JPT_END;
                    break;
                case GWHEEL_TYPE_POSITIVE:
                    start = GWHEEL_OPT_POS_START;
                    end = GWHEEL_OPT_POS_END;
                    break;
                case GWHEEL_TYPE_NEUTRAL:
                default:
                    start = GWHEEL_OPT_NEU_START;
                    end = GWHEEL_OPT_NEU_END;
                    break;
                case GWHEEL_TYPE_NEGATIVE:
                    start = GWHEEL_OPT_NEG_START;
                    end = GWHEEL_OPT_NEG_END;
                    break;
                case GWHEEL_TYPE_CATASTROPHIC:
                    start = GWHEEL_OPT_CAT_START;
                    end = GWHEEL_OPT_CAT_END;
                    break;
            }
            
            for (enum GamblingWheelOptionType type = start; type < end; type++) {
                assert_args(weights[type] >= 0.0f, "chs_menuinit_gambling_wheel:\nNegative weight: %d", type);
                totalWeight += weights[type];
            }

            assert_args(totalWeight > 0.0f, "chs_menuinit_gambling_wheel:\ntotalWeight is 0: %d", activeWheelIndex);
            if (totalWeight > 0.0f) {
                f32 rand = random_float() * totalWeight;
                for (enum GamblingWheelOptionType type = start; type < end; type++) {
                    if (rand < weights[type]) {
                        wheelEntry->generatedOptionType = type;
                        weights[wheelEntry->generatedOptionType] *= 0.33f; // Cut odds of duplicate in a third (for generated types only)
                        break;
                    }

                    rand -= weights[type];
                }
            }
        }
        wheelEntry->rotationOffsetStart = index * WEDGE_ROTATIONAL_FRAC;
        wheelEntry->rotationOffsetEnd = wheelEntry->rotationOffsetStart + (WEDGE_ROTATIONAL_FRAC * wheelEntry->size);
        wheelEntry->iconScale = 1.0f;

        index += wheelEntry->size;
    }
    assert_args(index == GAMBLING_WHEEL_SLOT_COUNT, "chs_menuinit_gambling_wheel:\nBad index: %d", index);

    wheelAction = GWHEEL_ACT_ANIMATE_SPAWN;
    prevWheelAction = GWHEEL_ACT_ANIMATE_SPAWN;
    wheelTimer = 0;

    shouldRenderAsHighlighted = FALSE;
    shouldRenderHighlighedColor = TRUE;
    lastPeg = -1;
    lastFlipperRotationPerc = 0.0f;
    lastFlipperRotationMomentum = 0.0f;
    mtxLayers = 0;

    framesToAccelerate = 0;
    framesToSpinAtMax = 0;
    wheelSpinSpeed = 0.0f;
    wheelFrictionFactor = 0.99f;
    wheelLinearDecelerationFactor = 0.5f;

    textTransparency = 0;
    highlightFlashFrames = 0;

    introTextFrames = 0;
}

static struct GamblingWheelParams *get_currently_highlighted_wheel_entry(struct GamblingWheelParams *wheel) {
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        struct GamblingWheelParams *wheelEntry = &wheel[i];
        if (wheelRotation >= wheelEntry->rotationOffsetStart && wheelRotation < wheelEntry->rotationOffsetEnd) {
            return &wheel[i];
        }
    }

    assert_args(FALSE, "get_currently_highlighted_wheel_entry:\nNo highlighted wheel entry found: %d", activeWheelIndex);
    return NULL;
}

static void render_nonhighlighted_wedges_and_icons(Gfx **dl, struct GamblingWheelParams *wheel) {
    Gfx *dlHead = *dl;

    // Draw non-highlighted wedges and icons
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        struct GamblingWheelParams *wheelEntry = &wheel[i];

        // Skip highlighted entry
        if (shouldRenderAsHighlighted && (wheelRotation >= wheelEntry->rotationOffsetStart && wheelRotation < wheelEntry->rotationOffsetEnd)) {
            continue;
        }

        // Wedge
        gSPDisplayList(dlHead++, gambling_wheel_dl_start);
        const f32 rotationCenter = (wheelEntry->rotationOffsetStart + wheelEntry->rotationOffsetEnd) * 0.5f;
        const u8 *wedgeColor = gamblingWheelColorParams[wheelEntry->type].idleColor;
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);

        gDPSetPrimColor(dlHead++, 0, 0, wedgeColor[0], wedgeColor[1], wedgeColor[2], wedgeColor[3]);
        if (gamblingWheelSizeParams[wheelEntry->size].wedgeDl) {
            gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheelEntry->size].wedgeDl);
        }
        gSPDisplayList(dlHead++, gambling_wheel_dl_end);

        // Icon
        gSPDisplayList(dlHead++, gambling_wheel_dl_icon_start);
        create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, 0, 56, 0.0f);
        wheelEntry->iconScale = smoothstop(wheelEntry->iconScale, 1.0f, 0.4f);
        create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, wheelEntry->iconScale, wheelEntry->iconScale, 1.0f);
        gDPSetPrimColor(dlHead++, 0, 0, 0xBF, 0xBF, 0xBF, 0xFF);
        if (wheelEntry->size > GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlBig) {
            gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlBig);
        } else if (wheelEntry->size == GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlSmall) {
            gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlSmall);
        }
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        gSPDisplayList(dlHead++, gambling_wheel_dl_icon_end);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);

    *dl = dlHead;
}

static void render_nonhighlighted_border_lines(Gfx **dl, struct GamblingWheelParams *wheel) {
    Gfx *dlHead = *dl;

    // Draw lines
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_start);
    gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        struct GamblingWheelParams *wheelEntry = &wheel[i];

        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheelEntry->rotationOffsetStart, 0, 0, 1.0f);
        gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_end);

    *dl = dlHead;
}

static void render_nonhighlighted_circle_borders(Gfx **dl, struct GamblingWheelParams *wheel) {
    Gfx *dlHead = *dl;

    // Draw circle borders
    gSPDisplayList(dlHead++, gambling_wheel_dl_start);
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        struct GamblingWheelParams *wheelEntry = &wheel[i];

        // Skip highlighted entry
        if (shouldRenderAsHighlighted && (wheelRotation >= wheelEntry->rotationOffsetStart && wheelRotation < wheelEntry->rotationOffsetEnd)) {
            continue;
        }

        const f32 rotationCenter = (wheelEntry->rotationOffsetStart + wheelEntry->rotationOffsetEnd) * 0.5f;
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);

        gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
        if (gamblingWheelSizeParams[wheelEntry->size].borderDl) {
            gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheelEntry->size].borderDl);
        }

        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);

    *dl = dlHead;
}

static void render_highlighted_wedge(Gfx **dl, struct GamblingWheelParams *wheelEntry) {
    Gfx *dlHead = *dl;

    // Draw highlighted wedge
    const u8 *wedgeColor = gamblingWheelColorParams[wheelEntry->type].highlightedColor;
    const f32 rotationCenter = (wheelEntry->rotationOffsetStart + wheelEntry->rotationOffsetEnd) * 0.5f;

    gSPDisplayList(dlHead++, gambling_wheel_dl_start);
    create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);
    gDPSetPrimColor(dlHead++, 0, 0, wedgeColor[0], wedgeColor[1], wedgeColor[2], wedgeColor[3]);
    if (gamblingWheelSizeParams[wheelEntry->size].wedgeDl) {
        gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheelEntry->size].wedgeDl);
    }
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);

    *dl = dlHead;
}

static void render_highlighted_border_lines(Gfx **dl, struct GamblingWheelParams *wheelEntry) {
    Gfx *dlHead = *dl;

    // Draw highlighted lines
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_start);
    if (shouldRenderHighlighedColor) {
        gDPSetPrimColor(dlHead++, 0, 0, highlightedBorderColor[0], highlightedBorderColor[1], highlightedBorderColor[2], highlightedBorderColor[3]);
    } else {
        gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
    }
    create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheelEntry->rotationOffsetStart, 0, 0, 1.0f);
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheelEntry->rotationOffsetEnd, 0, 0, 1.0f);
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_end);

    *dl = dlHead;
}

static void render_highlighted_icon(Gfx **dl, struct GamblingWheelParams *wheelEntry) {
    Gfx *dlHead = *dl;

    // Draw highlighted icon
    const f32 rotationCenter = (wheelEntry->rotationOffsetStart + wheelEntry->rotationOffsetEnd) * 0.5f;
    create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);
    wheelEntry->iconScale = smoothstop(wheelEntry->iconScale, 1.5f, 0.4f);
    create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, 0, 56, 0.0f);
    create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, wheelEntry->iconScale, wheelEntry->iconScale, 1.0f);
    gSPDisplayList(dlHead++, gambling_wheel_dl_icon_start);
    gDPSetPrimColor(dlHead++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    if (wheelEntry->size > GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlBig) {
        gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlBig);
    } else if (wheelEntry->size == GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlSmall) {
        gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheelEntry->generatedOptionType].textureDlSmall);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_icon_end);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

static void render_highlighted_circle_border(Gfx **dl, struct GamblingWheelParams *wheelEntry) {
    Gfx *dlHead = *dl;

    // Draw circle border
    const f32 rotationCenter = (wheelEntry->rotationOffsetStart + wheelEntry->rotationOffsetEnd) * 0.5f;
    gSPDisplayList(dlHead++, gambling_wheel_dl_start);
    create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);
    if (shouldRenderHighlighedColor) {
        gDPSetPrimColor(dlHead++, 0, 0, highlightedBorderColor[0], highlightedBorderColor[1], highlightedBorderColor[2], highlightedBorderColor[3]);
    } else {
        gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
    }
    if (gamblingWheelSizeParams[wheelEntry->size].borderDl) {
        gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheelEntry->size].borderDl);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}

static void render_highlighted_wheel_section(Gfx **dl, struct GamblingWheelParams *wheel) {
    Gfx *dlHead = *dl;

    // Render highlighted entry last
    struct GamblingWheelParams *wheelEntry = get_currently_highlighted_wheel_entry(wheel);

    if (wheelEntry) {
        if (shouldRenderAsHighlighted) {
            render_highlighted_wedge(&dlHead, wheelEntry);
            render_highlighted_border_lines(&dlHead, wheelEntry);
            render_highlighted_icon(&dlHead, wheelEntry);
            render_highlighted_circle_border(&dlHead, wheelEntry);
        }
    }

    *dl = dlHead;
}

static void render_pegs(Gfx **dl) {
    Gfx *dlHead = *dl;

    // Draw pegs
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_start);
    for (s32 i = 0; i < GAMBLING_WHEEL_PEG_COUNT; i++) {
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, (i * PEG_ROTATIONAL_FRAC), 0.0f, 0.0f, 1.0f);
        gDPSetPrimColor(dlHead++, 0, 0, pegColor[0], pegColor[1], pegColor[2], pegColor[3]);
        gSPDisplayList(dlHead++, gambling_wheel_dl_peg);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_end);

    *dl = dlHead;
}

static void render_flipper(Gfx **dl) {
    Gfx *dlHead = *dl;

    f32 flipperRotationPerc = 0.0f;
    s32 nearestPeg = (s32) (wheelRotation / PEG_ROTATIONAL_FRAC) + 1;
    f32 pegRotation = nearestPeg * PEG_ROTATIONAL_FRAC;

    f32 pegDiff = (WHEEL_FLIPPER_PEG_COLLISION_OFFSET + WHEEL_SELECTOR_OFFSET) - (pegRotation - wheelRotation);
    flipperRotationPerc = pegDiff / (WHEEL_FLIPPER_PEG_COLLISION_OFFSET + WHEEL_SELECTOR_OFFSET);

    lastFlipperRotationMomentum = (lastFlipperRotationMomentum + lastFlipperRotationPerc) * 0.8f;
    f32 computedFromLast = (lastFlipperRotationPerc - lastFlipperRotationMomentum) * 0.25f;

    if (computedFromLast >= flipperRotationPerc) {
        flipperRotationPerc = computedFromLast;
    } else {
        lastFlipperRotationMomentum = 0.0f;
    }

    // Draw circle border
    gSPDisplayList(dlHead++, gambling_wheel_dl_start);
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, 0, FLIPPER_OFFSET_Y, 0.0f);
    create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, WHEEL_FLIPPER_ROTATION_MAX * sins(0x4000 * flipperRotationPerc), 0.0f, 0.0f, 1.0f);
    gDPSetPrimColor(dlHead++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    gSPDisplayList(dlHead++, gambling_wheel_dl_wheel_flipper);
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    if (lastPeg >= 0 && lastPeg != nearestPeg % GAMBLING_WHEEL_PEG_COUNT) {
        play_sound(SOUND_GENERAL2_GAMBLING_WHEEL_FLIPPER, gGlobalSoundSource);
    }
    lastPeg = nearestPeg % GAMBLING_WHEEL_PEG_COUNT;
    lastFlipperRotationPerc = flipperRotationPerc;

    *dl = dlHead;
}

static void render_gambling_wheel(Gfx **dl) {
    Gfx *dlHead = *dl;

    struct GamblingWheelParams *wheel = gamblingWheels[activeWheelIndex].entries;

    gSPDisplayList(dlHead++, gambling_wheel_dl_start);

    // [-180.0f, 180.0f)
    f32 wheelRotationDisplayed = (360.0f - wheelRotation) - WHEEL_SELECTOR_OFFSET;
    if (wheelRotationDisplayed >= 180.0f) {
        wheelRotationDisplayed -= 360.0f;
    }

    create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheelRotationDisplayed, 0, 0, 1.0f);
    // gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
    // gSPDisplayList(dlHead++, gambling_wheel_dl_wheel_border_quarter);

    render_nonhighlighted_wedges_and_icons(&dlHead, wheel);
    render_nonhighlighted_border_lines(&dlHead, wheel);
    render_nonhighlighted_circle_borders(&dlHead, wheel);

    render_highlighted_wheel_section(&dlHead, wheel);
    render_pegs(&dlHead);

    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    render_flipper(&dlHead);
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);

    struct GamblingWheelParams *wheelEntry = get_currently_highlighted_wheel_entry(wheel);
    if (textTransparency > 0 && gamblingWheelOptionParams[wheelEntry->generatedOptionType].description) {
        slowtext_setup_ortho_rendering(&dlHead, FT_FONT_SMALL_BOLD);
        slowtext_draw_ortho_text_linebreaks(&dlHead, 0, -SCREEN_CENTER_Y + 12, SCREEN_WIDTH - (64 * 2), gamblingWheelOptionParams[wheelEntry->generatedOptionType].description, FT_FLAG_ALIGN_CENTER, 191, 191, 191, textTransparency);
        slowtext_finished_rendering(&dlHead);
    }

    *dl = dlHead;
}

static void render_wheel_intro_text(Gfx **dl) {
    static u8 colorBase[3];
    static u8 colorAdd[3];

    const u32 idleDuration = 10;
    const u32 animDuration = 22;
    const u32 freezeDuration = 36;
    const u32 totalDuration = (animDuration * 2) + freezeDuration;

    const u32 precalculatedWidth = 82;
    const u32 precalculatedHeight = 28;
    const s32 dumbStupidYOffset = 3;

    const u32 oscillationFrames = 10;

    Gfx *dlHead = *dl;
    
    if (introTextFrames < idleDuration) {
        if (introTextFrames == 0) {
            for (s32 i = 0; i < ARRAY_COUNT(colorBase); i++) {
                if ((random_u16() % 2) == 0) {
                    colorBase[i] = 0x2F;
                    colorAdd[i] = 0x40;
                } else {
                    colorBase[i] = 0x5F;
                    colorAdd[i] = 0x60;
                }
            }
        }

        introTextFrames++;
        return;
    }

    if (introTextFrames >= totalDuration) {
        return;
    }

    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);
    create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, 2.0f, 2.0f, 1.0f);

    u32 animFrames = introTextFrames - idleDuration;

    if (animFrames < animDuration) {
        s32 framesRemaining = animDuration - animFrames;
        f32 offsetX = sins(0x4000 * ((f32) framesRemaining / (f32) animDuration));
        offsetX = sqr(offsetX) * SCREEN_WIDTH;
        create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, offsetX, 0.0f, 0.0f);

        f32 rotation = sins(0x4000 * ((f32) framesRemaining / (f32) animDuration));
        rotation = sqr(rotation) * -90.0f;
        create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, rotation, 0.0f, 0.0f, 1.0f);
    } else if (animFrames >= freezeDuration + animDuration) {
        s32 framesPassed = animFrames - (freezeDuration + animDuration);
        f32 offsetX = sins(0x4000 * ((f32) framesPassed / (f32) animDuration));
        offsetX = sqr(offsetX) * -SCREEN_WIDTH;
        create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, offsetX, 0.0f, 0.0f);

        f32 rotation = sins(0x4000 * ((f32) framesPassed / (f32) animDuration));
        rotation = sqr(rotation) * 90.0f;
        create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, rotation, 0.0f, 0.0f, 1.0f);
    }

    u8 finalColor[3];
    f32 colorPerc = (1.0f + sins((gGlobalTimer % oscillationFrames) * 0x10000 / oscillationFrames)) / 2.0f;

    finalColor[0] = (f32) colorBase[0] + (((f32) colorAdd[0] * colorPerc) + 0.5f);
    finalColor[1] = (f32) colorBase[1] + (((f32) colorAdd[1] * colorPerc) + 0.5f);
    finalColor[2] = (f32) colorBase[2] + (((f32) colorAdd[2] * colorPerc) + 0.5f);
    
    chstut_draw_shaded_background(&dlHead, -precalculatedWidth / 2, precalculatedWidth / 2, (-precalculatedHeight / 2) + dumbStupidYOffset, (precalculatedHeight / 2) + dumbStupidYOffset, 0x17, 0x17, 0x00, 0xBF);

    slowtext_setup_ortho_rendering(&dlHead, FT_FONT_MEDIUM);
    slowtext_draw_ortho_text(&dlHead, 0, 0, "OH YEAH, IT'S\nWHEEL TIME!", FT_FLAG_ALIGN_CENTER, finalColor[0], finalColor[1], finalColor[2], 0xFF);
    slowtext_finished_rendering(&dlHead);
    
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    introTextFrames++;

    *dl = dlHead;
}

static void gambling_wheel_act_animate_spawn(Gfx **dl) {
    const u32 actionFrames = 32;
    const f32 animProgress = ((f32) wheelTimer) / ((f32) actionFrames);
    const f32 linearRotation = 2.5f;
    const f32 totalLinearRotation = (actionFrames * linearRotation);
    const f32 rotationSub = sins(0x2000);
    const f32 rotationMult = 1.0f / (1.0f - rotationSub);
    Gfx *dlHead = *dl;

    f32 scale = 1.0f;
    f32 extraRotation = rotationMult * (sins(0x2000 + (0x4000 * animProgress)) - rotationSub);
    if (wheelTimer >= (actionFrames / 2)) {
        extraRotation = 1.0f + (1.0f - extraRotation);
        scale = 1.0f + (0.5f * sins(0x8000 * animProgress));
    } else {
        scale = 1.5f * sins(0x8000 * animProgress);
    }

    // Some very yummy, totally digestable M A T H E M A T I C S :D
    wheelRotation = spawnWheelRotation + ((((180.0f * extraRotation) * ((360.0f - totalLinearRotation) / 360.0f)) + (wheelTimer * linearRotation)) * 2.0f);
    while (wheelRotation >= 360.0f) {
        wheelRotation -= 360.0f;
    }

    // Scale entire wheel
    create_dl_scale_matrix(&dlHead, MENU_MTX_PUSH, scale, scale, 1.0f);
    mtxLayers++;

    play_sound(SOUND_ENV_BOAT_ROCKING1, gGlobalSoundSource);

    // New action if applicable
    if (wheelTimer >= (actionFrames - 1)) {
        wheelAction++;
    }

    *dl = dlHead;
}

static void gambling_wheel_act_spawn_wait_for_button(Gfx **dl) {
    Gfx *dlHead = *dl;

    const u32 actionFrames = 8;
    f32 translationTotal = 0;
    if (wheelTimer < actionFrames) {
        translationTotal = 7.5f * ((f32) (actionFrames - wheelTimer) / (f32) actionFrames);
    }

    s32 rotation = random_u16();
    s32 translationX = sins(rotation) * translationTotal;
    s32 translationY = coss(rotation) * translationTotal;

    // translate entire wheel
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, translationX, translationY, 0.0f);
    mtxLayers++;

    if (wheelTimer == 0) {
        play_sound(SOUND_OBJ_POUNDING1, gGlobalSoundSource);
        play_sound(SOUND_GENERAL_GAMBLING_WHEEL_INTRO, gGlobalSoundSource);
        shouldRenderAsHighlighted = TRUE;
        shouldRenderHighlighedColor = TRUE;
        wheelRotation = spawnWheelRotation;
    }

    aPrompt = GWHEEL_APROMPT_SPIN;
    if (gPlayer1Controller->buttonPressed & A_BUTTON) {
        wheelAction++;
    }

    *dl = dlHead;
}

static void gambling_wheel_act_start_wheel_spin(Gfx **dl) {
    // Note: This spins the wheel for a randomly generated period of time at a fixed speed, in a way that mathematically equalizes each possibility on the wheel.
    // From here, we can slow the wheel down any way we want, and the odds will not be impacted later because they have already been equalized here (barring problems with the RNG algorithm itself).
    const s32 pegWholeDisplacement = 9; // Should not be a multiple of GAMBLING_WHEEL_PEG_COUNT (i.e. must be odd)
    const s32 extraSpinFraction = 2;
    const f32 pegOffset = (f32) pegWholeDisplacement / (f32) extraSpinFraction; // Must spin peg count * peg offset * whatever number to make peg offset a whole number, in order to cover each slot equally
    const s32 equalRequiredFrameCount = GAMBLING_WHEEL_PEG_COUNT * extraSpinFraction; // 0 - 63 frames of spinning at same speed, plus some hardcoded offset
    const f32 maxSpinRate = (PEG_ROTATIONAL_FRAC * pegOffset);

    Gfx *dlHead = *dl;

    if (wheelTimer == 0) {
        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);
        framesToAccelerate = (random_u16() % 10) + 25;
        framesToSpinAtMax = (random_u16() % equalRequiredFrameCount) + 5; // Constant added here doesn't make a difference
    }

    if (wheelTimer < framesToAccelerate) {
        wheelSpinSpeed = ((f32) wheelTimer / (f32) framesToAccelerate) * maxSpinRate;
    } else {
        wheelSpinSpeed = maxSpinRate;
        if (--framesToSpinAtMax <= 0) {
            wheelAction++;
        }
    }

    wheelRotation += wheelSpinSpeed;
    while (wheelRotation >= 360.0f) {
        wheelRotation -= 360.0f;
    }

    *dl = dlHead;
}

static void gambling_wheel_act_stop_wheel_spin(Gfx **dl) {
    Gfx *dlHead = *dl;

    if (wheelTimer == 0) {
        const f32 minFrictionFactor = 0.01f;
        const f32 maxFrictionFactor = 0.0175f;
        const f32 minLinearFactor = 0.004f;
        const f32 maxLinearFactor = 0.012f;
        wheelFrictionFactor = 1.0f - ((random_float() * (maxFrictionFactor - minFrictionFactor)) + minFrictionFactor);
        wheelLinearDecelerationFactor = (random_float() * (maxLinearFactor - minLinearFactor)) + minLinearFactor;
    }

    wheelSpinSpeed *= wheelFrictionFactor;
    wheelSpinSpeed -= wheelLinearDecelerationFactor;

    if (wheelSpinSpeed <= 0.0f) {
        wheelSpinSpeed = 0.0f;
        wheelAction++;
    }

    wheelRotation += wheelSpinSpeed;
    while (wheelRotation >= 360.0f) {
        wheelRotation -= 360.0f;
    }

    *dl = dlHead;
}

static void gambling_wheel_act_animate_result(Gfx **dl) {
    Gfx *dlHead = *dl;
    const u32 marioSoundTime = 10;
    const u32 totalFrameCount = 45;
    textTransparency = MIN(textTransparency + 10, U8_MAX);
    if (wheelTimer == 0 || wheelTimer == marioSoundTime) {
        struct GamblingWheelParams *wheel = gamblingWheels[activeWheelIndex].entries;
        struct GamblingWheelParams *wheelEntry = get_currently_highlighted_wheel_entry(wheel);
        if (wheelEntry) {
            const struct GamblingWheelColorProps *params = &gamblingWheelColorParams[wheelEntry->type];

            if (wheelTimer == 0) {
                highlightFlashFrames = 0;
                if (params->menuSoundId != NO_SOUND) {
                    play_sound(params->menuSoundId, gGlobalSoundSource);
                }
            } else if (wheelTimer == marioSoundTime) {
                if (params->marioSoundId != NO_SOUND) {
                    play_sound(params->marioSoundId, gGlobalSoundSource);
                }
            }
        }
    }

    if (highlightFlashFrames % FLASH_FRAME_COUNT >= (FLASH_FRAME_COUNT / 2)) {
        shouldRenderHighlighedColor = TRUE;
    } else {
        shouldRenderHighlighedColor = FALSE;
    }
    highlightFlashFrames = (highlightFlashFrames + 1) % FLASH_FRAME_COUNT;

    if (wheelTimer >= (totalFrameCount - 1)) {
        aPrompt = GWHEEL_APROMPT_NEXT;
        if (gPlayer1Controller->buttonPressed & A_BUTTON) {
            wheelAction++;
        }
    }

    *dl = dlHead;
}

static void gambling_wheel_act_end_animate(Gfx **dl) {
    static f32 fallSpeedTotalX = 0.0f;
    static f32 fallSpeedTotalY = 0.0f;
    static f32 fallRotTotal = 0.0f;
    static f32 fallSpeedNextX = 0.0f;
    static f32 fallSpeedNextY = 0.0f;
    static f32 fallRotNext = 0.0f;
    static f32 fallSpeedIncX = 0.0f;
    static f32 fallSpeedIncY = 0.0f;
    static f32 fallRotInc = 0.0f;

    Gfx *dlHead = *dl;
    textTransparency = MIN(textTransparency + 10, U8_MAX);

    if (highlightFlashFrames % FLASH_FRAME_COUNT >= (FLASH_FRAME_COUNT / 2)) {
        shouldRenderHighlighedColor = TRUE;
    } else {
        shouldRenderHighlighedColor = FALSE;
    }
    highlightFlashFrames = (highlightFlashFrames + 1) % FLASH_FRAME_COUNT;

    if (wheelTimer == 0) {
        shouldRenderHighlighedColor = TRUE;

        fallSpeedTotalX = 0.0f;
        fallSpeedTotalY = 0.0f;
        fallRotTotal = 0.0f;

        fallSpeedNextX = 0.0f;
        fallSpeedNextY = 0.0f;
        fallRotNext = 0.0f;

        fallSpeedIncX = (random_float() * 1.0f) - 0.5f;
        fallSpeedIncY = -1.15f;

        fallRotInc = fallSpeedIncX * -2.0f;
        if (fallRotInc > 0.0f) {
            fallRotInc = sqrtf(fallRotInc);
        } else if (fallRotInc < 0.0f) {
            fallRotInc = -sqrtf(-fallRotInc);
        }
        fallRotInc *= 1.5f;

        play_sound(SOUND_MENU_CLICK_FILE_SELECT, gGlobalSoundSource);

        // Find highlighted entry
        struct GamblingWheelParams *wheel = gamblingWheels[activeWheelIndex].entries;
        struct GamblingWheelParams *wheelEntry = get_currently_highlighted_wheel_entry(wheel);
        if (wheelEntry) {
            const struct GamblingWheelOptionProps *props = &gamblingWheelOptionParams[wheelEntry->generatedOptionType];
            if (props->activationFunc) {
                props->activationFunc();
            }
        }
    }

    fallRotNext = (fallRotNext * 0.5f + fallRotInc);
    fallSpeedNextX += fallSpeedIncX;
    fallSpeedNextY += fallSpeedIncY;

    fallRotTotal += fallRotNext;
    fallSpeedTotalX += fallSpeedNextX;
    fallSpeedTotalY += fallSpeedNextY;

    play_sound(SOUND_ENV_BOAT_ROCKING1, gGlobalSoundSource);

    // Translate and rotate entire wheel
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, fallSpeedTotalX, fallSpeedTotalY, 1.0f);
    create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, fallRotTotal, 0.0f, 0.0f, 1.0f);
    mtxLayers++;

    if (fallSpeedTotalY < -(SCREEN_HEIGHT * 1.5f)) {
        wheelAction++;
    }

    *dl = dlHead;
}

void chs_menuupdate_gambling_wheel(Gfx **dl) {
    void (*actionCallbacks[GWHEEL_ACT_COUNT])(Gfx **dl) = {
        [GWHEEL_ACT_ANIMATE_SPAWN]         = gambling_wheel_act_animate_spawn,
        [GWHEEL_ACT_SPAWN_WAIT_FOR_BUTTON] = gambling_wheel_act_spawn_wait_for_button,
        [GWHEEL_ACT_START_WHEEL_SPIN]      = gambling_wheel_act_start_wheel_spin,
        [GWHEEL_ACT_STOP_WHEEL_SPIN]       = gambling_wheel_act_stop_wheel_spin,
        [GWHEEL_ACT_ANIMATE_RESULT]        = gambling_wheel_act_animate_result,
        [GWHEEL_ACT_END_ANIMATE]           = gambling_wheel_act_end_animate,
    };

    Gfx *dlHead = *dl;
    mtxLayers = 0;
    aPrompt = GWHEEL_APROMPT_NONE;

    create_dl_ortho_matrix(&dlHead);
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, SCREEN_CENTER_X, WHEEL_OFFSET_Y + SCREEN_CENTER_Y, 0.0f);
    mtxLayers++;

    if (prevWheelAction != wheelAction) {
        prevWheelAction = wheelAction;
        wheelTimer = 0;
    }

    if (wheelAction < GWHEEL_ACT_COUNT && actionCallbacks[wheelAction]) {
        actionCallbacks[wheelAction](&dlHead);
        render_gambling_wheel(&dlHead);
    } else {
        // Event has concluded
        chaos_menuevent_finish_event();
    }

    while (mtxLayers > 0) {
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        mtxLayers--;
    }

    if (wheelTimer < 0xFFFFFFFF) {
        wheelTimer++;
    }

    render_wheel_intro_text(&dlHead);

    switch(aPrompt) {
        case GWHEEL_APROMPT_NEXT:
            menu_single_button_prompt(&dlHead, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 23, MENU_PROMPT_A_BUTTON, "Next", FALSE);
            break;
        case GWHEEL_APROMPT_SPIN:
            menu_single_button_prompt(&dlHead, SCREEN_WIDTH - 32, SCREEN_HEIGHT - 23, MENU_PROMPT_A_BUTTON, "Spin the Wheel!", FALSE);
            break;
        case GWHEEL_APROMPT_NONE:
        default:
            break;
    }

    *dl = dlHead;
}
