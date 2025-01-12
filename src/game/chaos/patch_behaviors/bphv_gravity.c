#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "sounds.h"
#include "audio/external.h"
#include "game/level_update.h"

#define LV1_GRAVITY 0.15f
#define LV2_GRAVITY 0.30f
#define LV3_GRAVITY 0.50f

static f32 get_decreased_gravity_total(void) {
    f32 total = 0.0f;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        switch (gChaosActiveEntries[i].id) {
            case CHAOS_PATCH_GRAVITY_DECREASE_LV1:
                total -= LV1_GRAVITY;
                break;
            case CHAOS_PATCH_GRAVITY_DECREASE_LV2:
                total -= LV2_GRAVITY;
                break;
            case CHAOS_PATCH_GRAVITY_DECREASE_LV3:
                total -= LV3_GRAVITY;
                break;
            default:
                break;
        }
    }

    return total;
}

static f32 get_increased_gravity_total(void) {
    f32 total = 0.0f;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        switch (gChaosActiveEntries[i].id) {
            case CHAOS_PATCH_GRAVITY_INCREASE_LV1:
                total += LV1_GRAVITY;
                break;
            case CHAOS_PATCH_GRAVITY_INCREASE_LV2:
                total += LV2_GRAVITY;
                break;
            case CHAOS_PATCH_GRAVITY_INCREASE_LV3:
                total += LV3_GRAVITY;
                break;
            default:
                break;
        }
    }

    return total;
}

u8 chs_cond_gravity_decrease_lv1(UNUSED const struct ChaosPatch *patch) { return (get_decreased_gravity_total() - LV1_GRAVITY >= -0.5f); }
u8 chs_cond_gravity_decrease_lv2(UNUSED const struct ChaosPatch *patch) { return (get_decreased_gravity_total() - LV2_GRAVITY >= -0.5f); }
u8 chs_cond_gravity_decrease_lv3(UNUSED const struct ChaosPatch *patch) { return (get_decreased_gravity_total() - LV3_GRAVITY >= -0.5f); }
u8 chs_cond_gravity_increase_lv1(UNUSED const struct ChaosPatch *patch) { return (get_increased_gravity_total() + LV1_GRAVITY <= 0.5f); }
u8 chs_cond_gravity_increase_lv2(UNUSED const struct ChaosPatch *patch) { return (get_increased_gravity_total() + LV2_GRAVITY <= 0.5f); }
u8 chs_cond_gravity_increase_lv3(UNUSED const struct ChaosPatch *patch) { return (get_increased_gravity_total() + LV3_GRAVITY <= 0.5f); }

void chs_act_gravity_decrease_lv1(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity -= LV1_GRAVITY; }
void chs_act_gravity_decrease_lv2(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity -= LV2_GRAVITY; }
void chs_act_gravity_decrease_lv3(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity -= LV3_GRAVITY; }
void chs_act_gravity_increase_lv1(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity += LV1_GRAVITY; }
void chs_act_gravity_increase_lv2(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity += LV2_GRAVITY; }
void chs_act_gravity_increase_lv3(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity += LV3_GRAVITY; }

void chs_deact_gravity_decrease_lv1(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity += LV1_GRAVITY; }
void chs_deact_gravity_decrease_lv2(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity += LV2_GRAVITY; }
void chs_deact_gravity_decrease_lv3(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity += LV3_GRAVITY; }
void chs_deact_gravity_increase_lv1(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity -= LV1_GRAVITY; }
void chs_deact_gravity_increase_lv2(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity -= LV2_GRAVITY; }
void chs_deact_gravity_increase_lv3(UNUSED const struct ChaosPatch *patch) { gMarioState->gravity -= LV3_GRAVITY; }
