#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
#include "game/area.h"
#include "game/debug.h"
#include "game/emutest.h"
#include "game/level_update.h"

#define TIMELIMIT (30 * 30)

#define MIN_100C 25
#define LV2_100C 10
#define LV3_100C 25
#define PAY2WIN_REQ 30

u8 chs_double_coins_under_30s(void) {
    struct ChaosActiveEntry *entry;

    if (!chaos_check_if_patch_active(CHAOS_PATCH_DOUBLE_COINS)) {
        return FALSE;
    }

    if (chaos_find_first_active_patch(CHAOS_PATCH_DOUBLE_COINS, &entry) < 0) {
        assert(FALSE, "chs_lvlinit_double_coins:\nActive patch could not be found!");
        return FALSE;
    }

    return (entry->frameTimer < TIMELIMIT);
}

void chs_lvlinit_double_coins(void) {
    struct ChaosActiveEntry *entry;

    if (chaos_find_first_active_patch(CHAOS_PATCH_DOUBLE_COINS, &entry) < 0) {
        assert(FALSE, "chs_lvlinit_double_coins:\nActive patch could not be found!");
        return;
    }

    entry->frameTimer = 0;
}

static s32 get_decreased_100c_total(void) {
    s32 total = 100;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        switch (gChaosActiveEntries[i].id) {
            case CHAOS_PATCH_100C_DECREASE_LV2:
                total -= LV2_100C;
                break;
            case CHAOS_PATCH_100C_DECREASE_LV3:
                total -= LV3_100C;
                break;
            default:
                break;
        }
    }

    return total;
}

u8 chs_cond_100c_decrease_lv2(void) { return (get_decreased_100c_total() - LV2_100C >= MIN_100C); }
u8 chs_cond_100c_decrease_lv3(void) { return (get_decreased_100c_total() - LV3_100C >= MIN_100C); }

void chs_act_100c_decrease_lv2(void) { gMarioState->hundredCoinOffset -= LV2_100C; }
void chs_act_100c_decrease_lv3(void) { gMarioState->hundredCoinOffset -= LV3_100C; }

void chs_deact_100c_decrease_lv2(void) { gMarioState->hundredCoinOffset += LV2_100C; }
void chs_deact_100c_decrease_lv3(void) { gMarioState->hundredCoinOffset += LV3_100C; }

u8 chs_pay2win_can_collect_star(void) {
    return !(
        chaos_check_if_patch_active(CHAOS_PATCH_PAY2WIN)
        && gCurrCourseNum != COURSE_NONE
        && gMarioState->numCoins < PAY2WIN_REQ
    );
}

/**
 * Collector's Anxiety
 */

enum CollectorsAnxietyCoinCourseFlags {
    COLLECTORS_ANXIETY_NONE   = 0u,
    COLLECTORS_ANXIETY_YELLOW = (1u << 0),
    COLLECTORS_ANXIETY_RED    = (1u << 1),
    COLLECTORS_ANXIETY_BLUE   = (1u << 2),

    COLLECTORS_ANXIETY_ALL    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED | COLLECTORS_ANXIETY_BLUE),
};

const enum CollectorsAnxietyCoinCourseFlags sCollectorsAnxietyCourseFlags[COURSE_COUNT] = {
    [COURSE_NONE] = COLLECTORS_ANXIETY_NONE,
    [COURSE_BOB]  = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED), // Note: Koopa has a blue coin...
    [COURSE_WF]   = COLLECTORS_ANXIETY_ALL,
    [COURSE_JRB]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_CCM]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_BBH]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_HMC]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_LLL]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_SSL]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_DDD]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_SL]   = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_WDW]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_TTM]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_THI]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_TTC]  = COLLECTORS_ANXIETY_ALL,
    [COURSE_RR]   = COLLECTORS_ANXIETY_ALL,

    [COURSE_BITDW]    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_BITFS]    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_BITS]     = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_PSS]      = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_BLUE),
    [COURSE_COTMC]    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_TOTWC]    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_VCUTM]    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_WMOTR]    = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
    [COURSE_SA]       = (COLLECTORS_ANXIETY_YELLOW | COLLECTORS_ANXIETY_RED),
};

enum CollectorsAnxietyCoinCourseFlags sCollectorsAnxietyCoinFlags = COLLECTORS_ANXIETY_NONE;
u8 sBoBKoopaSpawned = FALSE;

void chs_collectors_anxiety_load_new_level(void) {
    sCollectorsAnxietyCoinFlags = COLLECTORS_ANXIETY_NONE;
    sBoBKoopaSpawned = FALSE;
}

void chs_collectors_anxiety_small_koopa_spawned(void) {
    sBoBKoopaSpawned = TRUE;
}

void chs_collectors_anxiety_coin_collected(s32 coinValue) {
    switch (coinValue) {
        case 1:
            sCollectorsAnxietyCoinFlags |= COLLECTORS_ANXIETY_YELLOW;
            break;
        case 2:
            sCollectorsAnxietyCoinFlags |= COLLECTORS_ANXIETY_RED;
            break;
        case 5:
        case 100: // CHAOS_PATCH_BLUECOIN_LOTTERY
            sCollectorsAnxietyCoinFlags |= COLLECTORS_ANXIETY_BLUE;
            break;
        default:
            assert_args(FALSE, "chs_collectors_anxiety_coin_collected:\nUnexpected coinValue: %d", coinValue);
            break;
    }
}

u8 chs_collectors_anxiety_can_collect_star(void) {
    if (!chaos_check_if_patch_active(CHAOS_PATCH_COLLECTORS_ANXIETY)) {
        return TRUE;
    }

    // Ignore Bowser fights
    if (gCurrCourseNum >= COURSE_COUNT || gCurrLevelNum == LEVEL_BOWSER_1 || gCurrLevelNum == LEVEL_BOWSER_2 || gCurrLevelNum == LEVEL_BOWSER_3) {
        return TRUE;
    }

    // Set required flags, and update them in BoB as possible
    enum CollectorsAnxietyCoinCourseFlags requiredFlags = sCollectorsAnxietyCourseFlags[gCurrCourseNum];
    if (gCurrCourseNum == COURSE_BOB && sBoBKoopaSpawned) {
        requiredFlags |= COLLECTORS_ANXIETY_BLUE;
    }

    enum CollectorsAnxietyCoinCourseFlags courseFlags = (sCollectorsAnxietyCoinFlags & requiredFlags);

    if (courseFlags == requiredFlags) {
        return TRUE;
    }

    return FALSE;
}

/**
 * Coin Size
 */

u8 chs_cond_coin_size(void) {
    u32 count = chaos_count_active_instances(CHAOS_PATCH_COIN_SIZE);
    return (count < 3 && (count < 2 || !gInstantInputBlacklist));
}

