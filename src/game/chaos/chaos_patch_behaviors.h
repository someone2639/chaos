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

// Star/Save Modifiers
u8 chs_cond_star_shuffle(void);
u8 chs_cond_stars_increase_guarantee(void);
u8 chs_cond_stars_decrease_guarantee(void);
void chs_act_star_shuffle(void);
void chs_act_stars_increase_lv2(void);
void chs_act_stars_increase_lv3(void);
void chs_act_stars_increase_guarantee(void);
void chs_act_stars_decrease_lv2(void);
void chs_act_stars_decrease_lv3(void);
void chs_act_stars_decrease_guarantee(void);
void chs_act_get_key_1(void);
u8 chs_cond_get_key_1(void);
void chs_act_get_key_2(void);
u8 chs_cond_get_key_2(void);
void chs_act_unlock_cannons(void);
u8 chs_cond_unlock_cannons(void);

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
u8 chs_calculate_max_heal_counter(void);
u8 chs_cond_health_drain(void);
void chs_update_health_drain(void);
u8 chs_cond_health_gain(void);
void chs_update_health_gain(void);
u8 chs_cond_health_up(void);
void chs_act_health_up(void);
void chs_deact_health_up(void);
u8 chs_cond_health_down(void);
void chs_act_health_down(void);
void chs_deact_health_down(void);
u8 chs_check_temporary_invincibility(void);
void chs_lvlinit_temporary_invincibility(void);
u8 chs_cond_extradamage_enemies(void);
void chs_act_extradamage_enemies(void);
void chs_deact_extradamage_enemies(void);
u8 chs_cond_extradamage_lava(void);
void chs_act_extradamage_lava(void);
void chs_deact_extradamage_lava(void);

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
u8 chs_cond_coin_size(void);

// Movement Modifiers
void chs_update_brawl_tripping(void);
void chs_update_galaxy_twirl(void);
u8 chs_cond_no_fall_damage(void);
u8 chs_check_sticky_walljump(void);

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
void chs_act_random_dialogue(void);
void chs_update_random_dialogue(void);
void chs_act_kaizo_blocks(void);
void chs_update_kaizo_blocks(void);

// Object Spawners
u8 chs_cond_green_demon(void);
void chs_act_green_demon(void);
void chs_area_init_green_demon(void);
void chs_area_init_bullet_hell(void);
void chs_update_bullet_hell(void);
void chs_lvlinit_spawn_on_shell(void);
void chs_lvlupdate_spawn_on_shell(void);
void chs_deact_cosmic_clones(void);
void chs_act_cosmic_clones(void);
void chs_area_init_cosmic_clones(void);
void chs_update_cosmic_clones(void);
u8 chs_cond_cosmic_clones(void);

// Visual Modifiers
u8 chs_cond_no_zbuffer(void);
u8 chs_cond_inverted_zbuffer(void);
u8 chs_cond_decreased_fov(void);
u8 chs_cond_increased_fov(void);
u8 chs_cond_low_resolution(void);

// Time Limit
void chs_level_init_time_limit(void);
void chs_update_time_limit(void);
void chs_deact_time_limit(void);
u8 chs_cond_lower_time_limit(void);
void chs_act_lower_time_limit(void);

// Cheats
void chs_update_l_to_levitate(void);
u8 chs_cond_l_to_levitate(void);
void chs_update_debug_free_move(void);
u8 chs_cond_debug_free_move(void);
u8 chs_cond_level_reset(void);

// Patches
u8 chs_cond_remove_negative_patch(void);
void chs_act_remove_negative_patch(void);
u8 chs_cond_add_selectable_patch(void);
u8 chs_cond_remove_selectable_patch(void);

// Speed Modifiers
u8 chs_cond_push_back(void);
void chs_update_push_back(void);
u8 chs_cond_speed_limit(void);
void chs_update_speed_limit(void);
u8 chs_cond_walkies(void);
void chs_update_walkies(void);
u8 chs_cond_speed_tax(void);
void chs_update_speed_tax(void);

// Input Modifiers
u8 chs_cond_button_broken_b(void);
u8 chs_cond_button_broken_z(void);
u8 chs_cond_button_broken_c(void);
u8 chs_cond_swapped_zr_ab(void);
void chs_act_input_lag(void);

// Cap Effects
void chs_update_wing_cap(void);
void chs_act_remove_cap(void);
u8 chs_cond_remove_cap(void);
u8 chs_cond_disable_caps(void);

// Size Modifiers
void chs_act_mario_big(void);
u8 chs_cond_mario_big(void);
void chs_act_mario_small(void);
u8 chs_cond_mario_small(void);

// Misc
u8 chs_cond_one_hit_wonder(void);
u8 chs_cond_serve_ads(void);
void chs_update_serve_ads(void);
u8 chs_cond_miracle(void);
void chs_update_miracle(void);
u8 chs_cond_invisible(void);
u8 chs_cond_luigi(void);
void chs_update_luigi(void);
void chs_deact_luigi(void);
u8 chs_cond_45_fps(void);
u8 chs_cond_20_fps(void);
void chs_act_reverb(void);
void chs_deact_reverb(void);
u8 chs_topdown_check(void);
u8 chs_cam_invert_x_check(void);
void chs_shuffle_objects(void);
void chs_start_shuffle(void);
u8 chs_cond_quicktime(void);
void chs_update_quicktime(void);
void chs_act_quicktime(void);
u8 chs_cond_lethal_fall_damage(void);
u8 chs_cond_forced_mario_cam(void);