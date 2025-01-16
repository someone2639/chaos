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

// Health Modifiers
u8 chs_cond_health_drain(void);
void chs_update_health_drain(void);
u8 chs_cond_health_gain(void);
void chs_update_health_gain(void);

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

// Movement Modifiers
void chs_update_brawl_tripping(void);
void chs_update_galaxy_twirl(void);

// Random Griefing
void chs_act_random_sleep(void);
void chs_update_random_sleep(void);
void chs_act_random_shock(void);
void chs_update_random_shock(void);
u8 chs_cond_random_shock(void);
void chs_act_random_burn(void);
void chs_update_random_burn(void);
u8 chs_cond_random_burn(void);
void chs_act_random_blind(void);
void chs_update_random_blind(void);

// Object Spawners
void chs_area_init_green_demon(void);

// Visual Modifiers
u8 chs_cond_no_zbuffer(void);
u8 chs_cond_inverted_zbuffer(void);

// Time Limit
void chs_level_init_time_limit(void);
void chs_update_time_limit(void);
void chs_deact_time_limit(void);
u8 chs_cond_lower_time_limit(void);
void chs_act_lower_time_limit(void);

// Misc
u8 chs_cond_one_hit_wonder(void);