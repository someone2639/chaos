#include "actors/group0.h"
#include "behavior_data.h"
#include "object_fields.h"
#include "sounds.h"
#include "seq_ids.h"
#include "audio/external.h"
#include "game/ingame_menu.h"
#include "game/game_init.h"
#include "game/camera.h"
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
#include "course_table.h"

#define GAMBLING_WHEEL_SLOT_COUNT 16
#define WEDGE_ROTATIONAL_FRAC (360.0f / GAMBLING_WHEEL_SLOT_COUNT)

s32 wheelInitBefore = FALSE;
s32 activeWheelIndex = 0;
f32 wheelRotation = 0.0f;

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

    GWHEEL_OPT_JPT_END,
    GWHEEL_OPT_POS_START     = GWHEEL_OPT_JPT_END,

    /* GWHEEL_TYPE_POSITIVE */
    GWHEEL_OPT_POS_1_STAR    = GWHEEL_OPT_POS_START,
    GWHEEL_OPT_POS_5_LIVES,
    GWHEEL_OPT_POS_EXTEND,
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

struct GamblingWheelSizeProps {
    const Gfx *wedgeDl;
    const Gfx *borderDl;
};

struct GamblingWheelColorProps {
    const u8 idleColor[4];
    const u8 highlightedColor[4];
    const s32 menuSoundId;
    const s32 marioSoundId;
    const s32 seqId;
};

struct GamblingWheelOptionProps {
    const enum GamblingWheelOptionType id;
    const f32 weight;
    const Gfx *textureDlBig;
    const Gfx *textureDlSmall;
    const char *text;

    u8 (*conditionalFunc)(void);
    void (*activationFunc)(void);
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
    update_any_star(FALSE, 0);
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
static void gwheel_type_act_stars_increase_3(void) {
    update_any_star(FALSE, 0);
    update_any_star(FALSE, 0);
    update_any_star(FALSE, 0);
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
static void gwheel_type_act_stars_decrease_1(void) {
    update_any_star(TRUE, 1);
}
static void gwheel_type_act_stars_decrease_2(void) {
    update_any_star(TRUE, 1);
    update_any_star(TRUE, 1);
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
    gMarioState->numLives += 3;
    play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
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
            // Extend patch timer by 1
            gChaosActiveEntries[i].remainingDuration++;
        }
    }
}

static void gwheel_type_act_negative_extend(void) {
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const struct ChaosPatch *patch = &gChaosPatches[gChaosActiveEntries[i].id];
        if (patch->effectType == CHAOS_EFFECT_NEGATIVE && patch->durationType == CHAOS_DURATION_STARS) {
            // Extend patch timer by 1
            gChaosActiveEntries[i].remainingDuration++;
        }
    }
}

// static void gwheel_type_act_rank_1_positive(void) { chs_activate_random_pos_neg_patch_of_severity(1, CHAOS_EFFECT_POSITIVE); } // UNUSED
static void gwheel_type_act_rank_2_positive(void) { chs_activate_random_pos_neg_patch_of_severity(2, CHAOS_EFFECT_POSITIVE); }
static void gwheel_type_act_rank_3_positive(void) { chs_activate_random_pos_neg_patch_of_severity(3, CHAOS_EFFECT_POSITIVE); }
static void gwheel_type_act_rank_1_negative(void) { chs_activate_random_pos_neg_patch_of_severity(1, CHAOS_EFFECT_NEGATIVE); }
static void gwheel_type_act_rank_2_negative(void) { chs_activate_random_pos_neg_patch_of_severity(2, CHAOS_EFFECT_NEGATIVE); }
static void gwheel_type_act_rank_3_negative(void) { chs_activate_random_pos_neg_patch_of_severity(3, CHAOS_EFFECT_NEGATIVE); }

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
        .menuSoundId      = NO_SOUND,
        .seqId            = SEQ_EVENT_SOLVE_PUZZLE,
    },
    [GWHEEL_TYPE_POSITIVE] = {
        .idleColor        = {0x00, 0x7F, 0x00, 0xFF},
        .highlightedColor = {0x3F, 0xBF, 0x3F, 0xFF},
        .marioSoundId     = SOUND_MARIO_HAHA,
        .menuSoundId      = SOUND_GENERAL2_RIGHT_ANSWER,
        .seqId            = SEQ_SOUND_PLAYER,
    },
    [GWHEEL_TYPE_NEUTRAL] = {
        .idleColor        = {0x6F, 0x6F, 0x6F, 0xFF},
        .highlightedColor = {0xAF, 0xAF, 0xAF, 0xFF},
        .marioSoundId     = SOUND_MARIO_MAMA_MIA,
        .menuSoundId      = SOUND_MENU_CLICK_FILE_SELECT,
        .seqId            = SEQ_SOUND_PLAYER,
    },
    [GWHEEL_TYPE_NEGATIVE] = {
        .idleColor        = {0x8F, 0x00, 0x00, 0xFF},
        .highlightedColor = {0xCF, 0x3F, 0x3F, 0xFF},
        .marioSoundId     = SOUND_MARIO_ATTACKED,
        .menuSoundId      = SOUND_GENERAL2_CAMERA_BUZZ,
        .seqId            = SEQ_SOUND_PLAYER,
    },
    [GWHEEL_TYPE_CATASTROPHIC] = {
        .idleColor        = {0x37, 0x00, 0x2F, 0xFF},
        .highlightedColor = {0x67, 0x1F, 0x57, 0xFF},
        .marioSoundId     = SOUND_MARIO_WAAAOOOW,
        .menuSoundId      = NO_SOUND,
        .seqId            = SEQ_EVENT_KOOPA_MESSAGE,
    },
};

const struct GamblingWheelOptionProps gamblingWheelOptionParams[] = {
    [GWHEEL_OPT_ANY] = {
        .id              = GWHEEL_OPT_ANY,
        .weight          = 0.0f,
        .textureDlBig    = NULL,
        .textureDlSmall  = NULL,
        .text            = NULL,
        .conditionalFunc = NULL,
        .activationFunc  = NULL,
    },

    /* GWHEEL_TYPE_JACKPOT */
    [GWHEEL_OPT_JPT_3_STARS] = {
        .id              = GWHEEL_OPT_JPT_3_STARS,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_3_small,
        .text            = "Mark 3 random stars as collected.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_increase_3,
    },
    [GWHEEL_OPT_JPT_12_LIVES] = {
        .id              = GWHEEL_OPT_JPT_12_LIVES,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_12_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_12_small,
        .text            = "Gain 12 lives.",
        .conditionalFunc = gwheel_type_cond_lives_increase_12,
        .activationFunc  = gwheel_type_act_lives_increase_12,
    },

    /* GWHEEL_TYPE_POSITIVE */
    [GWHEEL_OPT_POS_1_STAR] = {
        .id              = GWHEEL_OPT_POS_1_STAR,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_1_small,
        .text            = "Mark a random star as collected.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_increase_1,
    },
    [GWHEEL_OPT_POS_5_LIVES] = {
        .id              = GWHEEL_OPT_POS_5_LIVES,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_5_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_5_small,
        .text            = "Gain 5 lives.",
        .conditionalFunc = gwheel_type_cond_lives_increase_5,
        .activationFunc  = gwheel_type_act_lives_increase_5,
    },
    [GWHEEL_OPT_POS_EXTEND] = {
        .id              = GWHEEL_OPT_POS_EXTEND,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_timer_big,
        .textureDlSmall  = gambling_wheel_dl_icon_timer_small,
        .text            = "Extend all positive patch durations by 1 star.",
        .conditionalFunc = gwheel_type_cond_positive_extend,
        .activationFunc  = gwheel_type_act_positive_extend,
    },
    [GWHEEL_OPT_POS_RANK_2] = {
        .id              = GWHEEL_OPT_POS_RANK_2,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_2_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_2_small,
        .text            = "Gain a positive rank 2 patch for 1 star.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_2_positive,
    },
    [GWHEEL_OPT_POS_RANK_3] = {
        .id              = GWHEEL_OPT_POS_RANK_3,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_3_small,
        .text            = "Gain a positive rank 3 patch for 1 star.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_3_positive,
    },

    /* GWHEEL_TYPE_NEUTRAL */
    [GWHEEL_OPT_NEU_NONE] = {
        .id              = GWHEEL_OPT_NEU_NONE,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_none_big,
        .textureDlSmall  = gambling_wheel_dl_icon_none_small,
        .text            = "No effect.",
        .conditionalFunc = NULL,
        .activationFunc  = NULL,
    },

    /* GWHEEL_TYPE_NEGATIVE */
    [GWHEEL_OPT_NEG_1_STAR] = {
        .id              = GWHEEL_OPT_NEG_1_STAR,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_m1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_m1_small,
        .text            = "Mark a random star as uncollected.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_decrease_1,
    },
    [GWHEEL_OPT_NEG_3_LIVES] = {
        .id              = GWHEEL_OPT_NEG_3_LIVES,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_m3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_m3_small,
        .text            = "Lose 3 lives.",
        .conditionalFunc = gwheel_type_cond_lives_decrease_3,
        .activationFunc  = gwheel_type_act_lives_decrease_3,
    },
    [GWHEEL_OPT_NEG_EXTEND] = {
        .id              = GWHEEL_OPT_NEG_EXTEND,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_timer_big,
        .textureDlSmall  = gambling_wheel_dl_icon_timer_small,
        .text            = "Extend all negative patch durations by 1 star.",
        .conditionalFunc = gwheel_type_cond_negative_extend,
        .activationFunc  = gwheel_type_act_negative_extend,
    },
    [GWHEEL_OPT_NEG_RANK_1] = {
        .id              = GWHEEL_OPT_NEG_RANK_1,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_1_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_1_small,
        .text            = "Gain a negative rank 1 patch for 1 star.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_1_negative,
    },
    [GWHEEL_OPT_NEG_RANK_2] = {
        .id              = GWHEEL_OPT_NEG_RANK_2,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_2_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_2_small,
        .text            = "Gain a negative rank 2 patch for 1 star.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_2_negative,
    },

    /* GWHEEL_TYPE_CATASTROPHIC */
    [GWHEEL_OPT_CAT_2_STARS] = {
        .id              = GWHEEL_OPT_CAT_2_STARS,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_stars_m2_big,
        .textureDlSmall  = gambling_wheel_dl_icon_stars_m2_small,
        .text            = "Mark 2 random stars as uncollected.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_stars_decrease_2,
    },
    [GWHEEL_OPT_CAT_6_LIVES] = {
        .id              = GWHEEL_OPT_CAT_6_LIVES,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_lives_m6_big,
        .textureDlSmall  = gambling_wheel_dl_icon_lives_m6_small,
        .text            = "Lose 6 lives.",
        .conditionalFunc = gwheel_type_cond_lives_decrease_6,
        .activationFunc  = gwheel_type_act_lives_decrease_6,
    },
    [GWHEEL_OPT_CAT_RANK_3] = {
        .id              = GWHEEL_OPT_CAT_RANK_3,
        .weight          = 1.0f,
        .textureDlBig    = gambling_wheel_dl_icon_rank_3_big,
        .textureDlSmall  = gambling_wheel_dl_icon_rank_3_small,
        .text            = "Gain a negative rank 3 patch for 1 star.",
        .conditionalFunc = NULL,
        .activationFunc  = gwheel_type_act_rank_3_negative,
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

void init_gambling_wheel(void) {
    wheelInitBefore = TRUE;
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
    wheelRotation = (WEDGE_ROTATIONAL_FRAC * wheel[0].size) / 2.0f;

    // Update forced types and weights
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        wheel[i].generatedOptionType = GWHEEL_OPT_NEU_NONE;

        if (wheel[i].forcedOptionType != GWHEEL_OPT_ANY && weights[wheel[i].forcedOptionType] > 0.0f) {
            wheel[i].generatedOptionType = wheel[i].forcedOptionType;
            weights[wheel[i].forcedOptionType] *= 0.5f; // Cut odds of duplicate in half (for forced types only)
        }
    }

    // Generate remaining non-forced types
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        if (wheel[i].forcedOptionType == GWHEEL_OPT_ANY || weights[wheel[i].forcedOptionType] <= 0.0f) {
            enum GamblingWheelOptionType start;
            enum GamblingWheelOptionType end;
            f32 totalWeight = 0.0f;

            switch(wheel[i].type) {
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
                assert_args(weights[type] >= 0.0f, "init_gambling_wheel:\nNegative weight: %d", type);
                totalWeight += weights[type];
            }

            assert_args(totalWeight > 0.0f, "init_gambling_wheel:\ntotalWeight is 0: %d", activeWheelIndex);
            if (totalWeight > 0.0f) {
                f32 rand = random_float() * totalWeight;
                for (enum GamblingWheelOptionType type = start; type < end; type++) {
                    if (rand < weights[type]) {
                        wheel[i].generatedOptionType = type;
                        weights[wheel[i].generatedOptionType] *= 0.33f; // Cut odds of duplicate in a third (for generated types only)
                        break;
                    }

                    rand -= weights[type];
                }
            }
        }
        wheel[i].rotationOffsetStart = index * WEDGE_ROTATIONAL_FRAC;
        wheel[i].rotationOffsetEnd = wheel[i].rotationOffsetStart + (WEDGE_ROTATIONAL_FRAC * wheel[i].size);
        wheel[i].iconScale = 1.0f;

        index += wheel[i].size;
    }
    assert_args(index == GAMBLING_WHEEL_SLOT_COUNT, "init_gambling_wheel:\nBad index: %d", index);
}

void render_gambling_wheel(Gfx **dl) {
    u8 normalBorderColor[4] = {0x00, 0x00, 0x00, 0xFF};
    u8 highlightedBorderColor[4] = {0xCF, 0xBF, 0x3F, 0xFF};
    Gfx *dlHead = *dl;

    if (!wheelInitBefore) {
        init_gambling_wheel();
    }

    struct GamblingWheelParams *wheel = gamblingWheels[activeWheelIndex].entries;

    gSPDisplayList(dlHead++, gambling_wheel_dl_start);
    create_dl_ortho_matrix(&dlHead);
    create_dl_translation_matrix(&dlHead, MENU_MTX_PUSH, SCREEN_CENTER_X, SCREEN_CENTER_Y, 0.0f);

    wheelRotation += 2.5f;
    while (wheelRotation >= 360.0f) {
        wheelRotation -= 360.0f;
    }

    // [-180.0f, 180.0f)
    f32 wheelRotationDisplayed = 360.0f - wheelRotation;
    if (wheelRotationDisplayed >= 180.0f) {
        wheelRotationDisplayed -= 360.0f;
    }

    create_dl_rotation_matrix(&dlHead, MENU_MTX_NOPUSH, wheelRotationDisplayed, 0, 0, 1.0f);
    // gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
    // gSPDisplayList(dlHead++, gambling_wheel_dl_wheel_border_quarter);

    // Draw wedges
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        // Skip highlighted entry
        if (wheelRotation >= wheel[i].rotationOffsetStart && wheelRotation < wheel[i].rotationOffsetEnd) {
            continue;
        }

        gSPDisplayList(dlHead++, gambling_wheel_dl_start);
        const f32 rotationCenter = (wheel[i].rotationOffsetStart + wheel[i].rotationOffsetEnd) * 0.5f;
        const u8 *wedgeColor = gamblingWheelColorParams[wheel[i].type].idleColor;
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);

        gDPSetPrimColor(dlHead++, 0, 0, wedgeColor[0], wedgeColor[1], wedgeColor[2], wedgeColor[3]);
        if (gamblingWheelSizeParams[wheel[i].size].wedgeDl) {
            gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheel[i].size].wedgeDl);
        }
        gSPDisplayList(dlHead++, gambling_wheel_dl_end);

        gSPDisplayList(dlHead++, gambling_wheel_dl_icon_start);
        create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, 0, 56, 0.0f);
        wheel[i].iconScale = smoothstop(wheel[i].iconScale, 1.0f, 0.4f);
        create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, wheel[i].iconScale, wheel[i].iconScale, 1.0f);
        gDPSetPrimColor(dlHead++, 0, 0, 0xBF, 0xBF, 0xBF, 0xFF);
        if (wheel[i].size > GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlBig) {
            gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlBig);
        } else if (wheel[i].size == GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlSmall) {
            gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlSmall);
        }
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        gSPDisplayList(dlHead++, gambling_wheel_dl_icon_end);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);
    
    // Draw lines
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_start);
    gDPSetPrimColor(dlHead++, 0, 0, 0x00, 0x00, 0x00, 0xFF);
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheel[i].rotationOffsetStart, 0, 0, 1.0f);
        gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_end);

    // Draw circle borders
    gSPDisplayList(dlHead++, gambling_wheel_dl_start);
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        // Skip highlighted entry
        if (wheelRotation >= wheel[i].rotationOffsetStart && wheelRotation < wheel[i].rotationOffsetEnd) {
            continue;
        }

        const f32 rotationCenter = (wheel[i].rotationOffsetStart + wheel[i].rotationOffsetEnd) * 0.5f;
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);

        gDPSetPrimColor(dlHead++, 0, 0, normalBorderColor[0], normalBorderColor[1], normalBorderColor[2], normalBorderColor[3]);
        if (gamblingWheelSizeParams[wheel[i].size].borderDl) {
            gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheel[i].size].borderDl);
        }

        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }
    gSPDisplayList(dlHead++, gambling_wheel_dl_end);

    // Render highlighted entry last
    for (s32 i = 0; i < gamblingWheels[activeWheelIndex].count; i++) {
        if (wheelRotation < wheel[i].rotationOffsetStart || wheelRotation >= wheel[i].rotationOffsetEnd) {
            continue;
        }

        const u8 *wedgeColor = gamblingWheelColorParams[wheel[i].type].highlightedColor;
        const f32 rotationCenter = (wheel[i].rotationOffsetStart + wheel[i].rotationOffsetEnd) * 0.5f;

        gSPDisplayList(dlHead++, gambling_wheel_dl_start);
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);
        gDPSetPrimColor(dlHead++, 0, 0, wedgeColor[0], wedgeColor[1], wedgeColor[2], wedgeColor[3]);
        if (gamblingWheelSizeParams[wheel[i].size].wedgeDl) {
            gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheel[i].size].wedgeDl);
        }
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        gSPDisplayList(dlHead++, gambling_wheel_dl_end);
        
        gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_start);
        gDPSetPrimColor(dlHead++, 0, 0, highlightedBorderColor[0], highlightedBorderColor[1], highlightedBorderColor[2], highlightedBorderColor[3]);
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheel[i].rotationOffsetStart, 0, 0, 1.0f);
        gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, wheel[i].rotationOffsetEnd, 0, 0, 1.0f);
        gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
        gSPDisplayList(dlHead++, gambling_wheel_dl_border_line_slice_end);

        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);
        wheel[i].iconScale = smoothstop(wheel[i].iconScale, 1.5f, 0.4f);
        create_dl_translation_matrix(&dlHead, MENU_MTX_NOPUSH, 0, 56, 0.0f);
        create_dl_scale_matrix(&dlHead, MENU_MTX_NOPUSH, wheel[i].iconScale, wheel[i].iconScale, 1.0f);
        gSPDisplayList(dlHead++, gambling_wheel_dl_icon_start);
        gDPSetPrimColor(dlHead++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
        if (wheel[i].size > GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlBig) {
            gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlBig);
        } else if (wheel[i].size == GWHEEL_SIZE_SMALL && gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlSmall) {
            gSPDisplayList(dlHead++, gamblingWheelOptionParams[wheel[i].generatedOptionType].textureDlSmall);
        }
        gSPDisplayList(dlHead++, gambling_wheel_dl_icon_end);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

        gSPDisplayList(dlHead++, gambling_wheel_dl_start);
        create_dl_rotation_matrix(&dlHead, MENU_MTX_PUSH, rotationCenter, 0, 0, 1.0f);
        gDPSetPrimColor(dlHead++, 0, 0, highlightedBorderColor[0], highlightedBorderColor[1], highlightedBorderColor[2], highlightedBorderColor[3]);
        if (gamblingWheelSizeParams[wheel[i].size].borderDl) {
            gSPDisplayList(dlHead++, gamblingWheelSizeParams[wheel[i].size].borderDl);
        }
        gSPDisplayList(dlHead++, gambling_wheel_dl_end);
        gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);
    }

    gSPDisplayList(dlHead++, gambling_wheel_dl_end);
    gSPPopMatrix(dlHead++, G_MTX_MODELVIEW);

    *dl = dlHead;
}
