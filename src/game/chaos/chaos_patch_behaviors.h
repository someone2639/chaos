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
u8 chs_cond_instant_game_over(void);
u8 chs_cond_life_gambler(void);
void chs_act_lives_increase_lv1(void);
void chs_act_lives_increase_lv2(void);
void chs_act_lives_increase_lv3(void);
void chs_act_lives_decrease_lv1(void);
void chs_act_lives_decrease_lv2(void);
void chs_act_lives_decrease_lv3(void);
s32 chs_life_gambler_get_lives_lost(void);

// Star/Save Modifiers
void add_uncollected_star(void);
void remove_collected_star(void);
void update_any_star(u8 shouldRemove, s8 negativeRetriesBias);
u8 chs_cond_star_shuffle(void);
u8 chs_cond_stars_increase_guarantee(void);
u8 chs_cond_stars_decrease_guarantee(void);
u8 chs_cond_toad_star_restock(void);
void chs_act_star_shuffle(void);
void chs_act_stars_increase_lv2(void);
void chs_act_stars_increase_lv3(void);
void chs_act_stars_increase_guarantee(void);
void chs_act_stars_decrease_lv2(void);
void chs_act_stars_decrease_lv3(void);
void chs_act_stars_decrease_guarantee(void);
void chs_act_toad_star_restock(void);
void chs_act_get_key_1(void);
void chs_deact_get_key_1(void);
u8 chs_cond_get_key_1(void);
void chs_act_get_key_2(void);
void chs_deact_get_key_2(void);
u8 chs_cond_get_key_2(void);
void chs_act_get_wing_cap(void);
void chs_deact_get_wing_cap(void);
u8 chs_cond_get_wing_cap(void);
void chs_act_get_metal_cap(void);
void chs_deact_get_metal_cap(void);
u8 chs_cond_get_metal_cap(void);
void chs_act_get_vanish_cap(void);
void chs_deact_get_vanish_cap(void);
u8 chs_cond_get_vanish_cap(void);
u8 chs_cond_unlock_star_doors(void);
u8 chs_cond_star_cloning_device(void);
s32 chs_get_yellow_star_in_course(s32 courseNum, s32 collectedStarId);
u8 chs_cond_coin_flip(void);
void chs_menuinit_coin_flip(void);
void chs_menuupdate_coin_flip(Gfx **dl);

// Gravity Modifiers
void chs_act_gravity_decrease(void);
void chs_act_gravity_increase(void);
void chs_deact_gravity_decrease(void);
void chs_deact_gravity_increase(void);

// Health Modifiers
u8 chs_calculate_max_heal_counter(void);
void chs_update_health_drain(void);
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
u8 chs_cond_shield(void);
void chs_update_random_invincibility(void);
void chs_act_random_invincibility(void);
u8 chs_cond_mario_dies(void);
void chs_act_mario_dies(void);

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
void chs_collectors_anxiety_load_new_level(void);
void chs_collectors_anxiety_small_koopa_spawned(void);
void chs_collectors_anxiety_coin_collected(s32 coinValue);
u8 chs_collectors_anxiety_can_collect_star(void);
u8 chs_cond_coin_size(void);

// Movement Modifiers
void chs_update_brawl_tripping(void);
void chs_update_galaxy_twirl(void);
void chs_act_cant_stop_wont_stop(void);

// Random Griefing
void chs_act_random_sleep(void);
void chs_update_random_sleep(void);
void chs_act_random_shock(void);
void chs_update_random_shock(void);
void chs_act_random_burn(void);
void chs_update_random_burn(void);
void chs_act_random_blind(void);
void chs_update_random_blind(void);
void chs_act_random_dialogue(void);
void chs_update_random_dialogue(void);
void chs_act_kaizo_blocks(void);
void chs_update_kaizo_blocks(void);
void chs_act_troll_sounds(void);
void chs_update_troll_sounds(void);
void chs_act_red_light(void);
void chs_update_red_light(void);
void chs_act_cosmic_rays(void);
void chs_update_cosmic_rays(void);
void chs_update_random_button_presses(void);

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

// Visual Modifiers
u8 chs_cond_no_zbuffer(void);
u8 chs_cond_increased_fov(void);
u8 chs_cond_low_resolution(void);
u8 chs_cond_no_skybox(void);
u8 chs_cond_dizzy_objects(void);
u8 chs_cond_camera_lag(void);
u8 chs_cond_darkness(void);
void chs_act_sideways_camera(void);
u8 chs_cond_rocking_camera(void);
void chs_act_rocking_camera(void);
void chs_update_rocking_camera(void);
u8 chs_cond_cartridge_tilt(void);
u8 chs_cond_corruption(void);
void chs_update_corruption(void);
void chs_deact_corruption(void);

// Time Limit
void chs_level_init_time_limit(void);
void chs_update_time_limit(void);
void chs_deact_time_limit(void);
u8 chs_cond_lower_time_limit(void);
void chs_act_lower_time_limit(void);
void chs_deact_lower_time_limit(void);

// Cheats
u8 chs_can_mario_levitate(void);
void chs_update_l_to_levitate(void);
u8 chs_cond_l_to_levitate(void);
void chs_update_debug_free_move(void);
u8 chs_cond_debug_free_move(void);

// Patches
u8 chs_cond_remove_negative_patch(void);
void chs_act_remove_negative_patch(void);
enum ChaosPatchID chs_activate_random_pos_neg_patch_of_severity(s32 patchSeverity, enum ChaosPatchEffectType effectType,
            u8 skipSeverityModifier, u32 maxForcedDuration, enum ChaosPatchDurationType durationType);
u8 chs_cond_add_random_buff(void);
void chs_act_add_random_buff(void);
u8 chs_cond_add_selectable_patch(void);
u8 chs_cond_remove_selectable_patch(void);
u8 chs_cond_lucky_charm(void);
u8 chs_cond_unlucky_charm(void);
u8 chs_cond_uneventful(void);
u8 chs_cond_forgiveness(void);
void chs_menuinit_gambling_wheel(void);
void chs_menuupdate_gambling_wheel(Gfx **dl);

// Speed Modifiers
u8 chs_cond_push_back(void);
void chs_update_push_back(void);
void chs_act_speed_tax(void);
void chs_update_speed_tax(void);

// Input Modifiers
void chs_act_input_lag(void);

// Cap Effects
void chs_update_wing_cap(void);
void chs_act_remove_cap(void);
u8 chs_cond_remove_cap(void);
u8 chs_cond_disable_caps(void);
u8 chs_cond_uncapped_caps(void);

// Size Modifiers
u8 chs_cond_mario_big(void);
void chs_act_mario_big(void);
void chs_deact_mario_big(void);
u8 chs_cond_mario_small(void);
void chs_act_mario_small(void);
void chs_deact_mario_small(void);

// Audio
void chs_act_reverb(void);
void chs_deact_reverb(void);
u8 chs_cond_randomized_music(void);
u8 chs_cond_chill_out(void);
u8 chs_cond_mad_musical_mess(void);
void chs_act_sound_effect_shuffle(void);
s32 get_randomized_sound(s32 sound);

// Misc
void chs_act_serve_ads(void);
void chs_update_serve_ads(void);
u8 chs_cond_miracle_normal(void);
u8 chs_cond_miracle_hardcore(void);
void chs_update_miracle(void);
u8 chs_is_miracle_active(void);
void chs_decrement_miracle(void);
void chs_update_luigi(void);
void chs_deact_luigi(void);
void chs_mq_init(void);
void chs_mq_deinit(void);
void chs_shuffle_objects(void);
void chs_start_shuffle(void);
void chs_update_quicktime(void);
void chs_act_quicktime(void);
u8 chs_cond_marth_grab(void);
u8 chs_cond_cherry_clone(void);
void chs_create_cherry_clone(void);
void chs_remove_cherry_clone(void);
void chs_init_cherry_clones_after_warp(void);
void chs_act_45_degree_camera(void);
void chs_deact_45_degree_camera(void);
void chs_update_noclip(void);
u8 chs_cond_randomize_warps(void);
void chs_init_dvd(void);
void chs_act_number_blindness(void);
void chs_deact_number_blindness(void);
void chs_act_tetris(void);
void chs_deact_tetris(void);
void chs_update_tetris(void);
void draw_tetris(void);
void chs_act_star_medallion(void);
void chs_area_init_star_medallion(void);
void chs_lvlinit_star_medallion(void);
u8 chs_star_medallion_can_collect_star(void);
