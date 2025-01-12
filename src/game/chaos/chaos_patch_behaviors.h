#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

// Lives Modifiers
u8 chs_cond_lives_increase_lv1(void);
u8 chs_cond_lives_increase_lv2(void);
u8 chs_cond_lives_increase_lv3(void);
u8 chs_cond_lives_decrease_lv1(void);
u8 chs_cond_lives_decrease_lv2(void);
u8 chs_cond_lives_decrease_lv3(void);
void chs_act_lives_increase_lv1(void);
void chs_act_lives_increase_lv2(void);
void chs_act_lives_increase_lv3(void);
void chs_act_lives_decrease_lv1(void);
void chs_act_lives_decrease_lv2(void);
void chs_act_lives_decrease_lv3(void);

// Star Modifiers
u8 chs_cond_stars_increase_guarantee(void);
u8 chs_cond_stars_decrease_guarantee(void);
void chs_act_stars_increase_lv2(void);
void chs_act_stars_increase_lv3(void);
void chs_act_stars_increase_guarantee(void);
void chs_act_stars_decrease_lv2(void);
void chs_act_stars_decrease_lv3(void);
void chs_act_stars_decrease_guarantee(void);

// Gravity Modifiers
u8 chs_cond_gravity_decrease_lv1(void);
u8 chs_cond_gravity_decrease_lv2(void);
u8 chs_cond_gravity_decrease_lv3(void);
u8 chs_cond_gravity_increase_lv1(void);
u8 chs_cond_gravity_increase_lv2(void);
u8 chs_cond_gravity_increase_lv3(void);
void chs_act_gravity_decrease_lv1(void);
void chs_act_gravity_decrease_lv2(void);
void chs_act_gravity_decrease_lv3(void);
void chs_act_gravity_increase_lv1(void);
void chs_act_gravity_increase_lv2(void);
void chs_act_gravity_increase_lv3(void);
void chs_deact_gravity_decrease_lv1(void);
void chs_deact_gravity_decrease_lv2(void);
void chs_deact_gravity_decrease_lv3(void);
void chs_deact_gravity_increase_lv1(void);
void chs_deact_gravity_increase_lv2(void);
void chs_deact_gravity_increase_lv3(void);

// Coin Modifiers
u8 chs_double_coins_under_30s(void); // Check whether double coins should remain in effect
void chs_lvlinit_double_coins(void);
u8 chs_cond_100c_decrease_lv2(void);
u8 chs_cond_100c_decrease_lv3(void);
void chs_act_100c_decrease_lv2(void);
void chs_act_100c_decrease_lv3(void);
void chs_deact_100c_decrease_lv2(void);
void chs_deact_100c_decrease_lv3(void);
u8 chs_pay2win_can_collect_star(void);
