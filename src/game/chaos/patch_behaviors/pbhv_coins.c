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

    if (!chaos_find_first_active_patch(CHAOS_PATCH_DOUBLE_COINS, &entry)) {
        assert(FALSE, "chs_lvlinit_double_coins:\nActive patch could not be found!");
    }

    return (entry->frameTimer < TIMELIMIT);
}

void chs_lvlinit_double_coins(void) {
    struct ChaosActiveEntry *entry;

    if (!chaos_find_first_active_patch(CHAOS_PATCH_DOUBLE_COINS, &entry)) {
        assert(FALSE, "chs_lvlinit_double_coins:\nActive patch could not be found!");
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

u8 chs_cond_coin_size(void) {
    return (!(gEmulator & EMU_CONSOLE) && chaos_count_active_instances(CHAOS_PATCH_COIN_SIZE) < 3);
}
