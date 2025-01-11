
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

u8 chs_cond_gravity_decrease_lv1(UNUSED const struct ChaosPatch *patch) { return (gMarioState->gravity - LV1_GRAVITY >= 0.5f); }
u8 chs_cond_gravity_decrease_lv2(UNUSED const struct ChaosPatch *patch) { return (gMarioState->gravity - LV2_GRAVITY >= 0.5f); }
u8 chs_cond_gravity_decrease_lv3(UNUSED const struct ChaosPatch *patch) { return (gMarioState->gravity - LV3_GRAVITY >= 0.5f); }
u8 chs_cond_gravity_increase_lv1(UNUSED const struct ChaosPatch *patch) { return (gMarioState->gravity + LV1_GRAVITY <= 1.5f); }
u8 chs_cond_gravity_increase_lv2(UNUSED const struct ChaosPatch *patch) { return (gMarioState->gravity + LV2_GRAVITY <= 1.5f); }
u8 chs_cond_gravity_increase_lv3(UNUSED const struct ChaosPatch *patch) { return (gMarioState->gravity + LV3_GRAVITY <= 1.5f); }

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
