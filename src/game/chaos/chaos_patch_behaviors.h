#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

// Lives Modifiers
u8 chs_cond_lives_increase_lv1(const struct ChaosPatch *patch);
u8 chs_cond_lives_increase_lv2(const struct ChaosPatch *patch);
u8 chs_cond_lives_increase_lv3(const struct ChaosPatch *patch);
u8 chs_cond_lives_decrease_lv1(const struct ChaosPatch *patch);
u8 chs_cond_lives_decrease_lv2(const struct ChaosPatch *patch);
u8 chs_cond_lives_decrease_lv3(const struct ChaosPatch *patch);
void chs_act_lives_increase_lv1(const struct ChaosPatch *patch);
void chs_act_lives_increase_lv2(const struct ChaosPatch *patch);
void chs_act_lives_increase_lv3(const struct ChaosPatch *patch);
void chs_act_lives_decrease_lv1(const struct ChaosPatch *patch);
void chs_act_lives_decrease_lv2(const struct ChaosPatch *patch);
void chs_act_lives_decrease_lv3(const struct ChaosPatch *patch);

// Star Modifiers
u8 chs_cond_stars_increase_guarantee(UNUSED const struct ChaosPatch *patch);
u8 chs_cond_stars_decrease_guarantee(UNUSED const struct ChaosPatch *patch);
void chs_act_stars_increase_lv2(const struct ChaosPatch *patch);
void chs_act_stars_increase_lv3(const struct ChaosPatch *patch);
void chs_act_stars_increase_guarantee(const struct ChaosPatch *patch);
void chs_act_stars_decrease_lv2(const struct ChaosPatch *patch);
void chs_act_stars_decrease_lv3(const struct ChaosPatch *patch);
void chs_act_stars_decrease_guarantee(const struct ChaosPatch *patch);

// Gravity Modifiers
u8 chs_cond_gravity_decrease_lv1(const struct ChaosPatch *patch);
u8 chs_cond_gravity_decrease_lv2(const struct ChaosPatch *patch);
u8 chs_cond_gravity_decrease_lv3(const struct ChaosPatch *patch);
u8 chs_cond_gravity_increase_lv1(const struct ChaosPatch *patch);
u8 chs_cond_gravity_increase_lv2(const struct ChaosPatch *patch);
u8 chs_cond_gravity_increase_lv3(const struct ChaosPatch *patch);
void chs_act_gravity_decrease_lv1(const struct ChaosPatch *patch);
void chs_act_gravity_decrease_lv2(const struct ChaosPatch *patch);
void chs_act_gravity_decrease_lv3(const struct ChaosPatch *patch);
void chs_act_gravity_increase_lv1(const struct ChaosPatch *patch);
void chs_act_gravity_increase_lv2(const struct ChaosPatch *patch);
void chs_act_gravity_increase_lv3(const struct ChaosPatch *patch);
void chs_deact_gravity_decrease_lv1(const struct ChaosPatch *patch);
void chs_deact_gravity_decrease_lv2(const struct ChaosPatch *patch);
void chs_deact_gravity_decrease_lv3(const struct ChaosPatch *patch);
void chs_deact_gravity_increase_lv1(const struct ChaosPatch *patch);
void chs_deact_gravity_increase_lv2(const struct ChaosPatch *patch);
void chs_deact_gravity_increase_lv3(const struct ChaosPatch *patch);