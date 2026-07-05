#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "behavior_data.h"
#include "course_table.h"
#include "object_constants.h"
#include "sounds.h"
#include "audio/external.h"
#include "engine/behavior_script.h"
#include "game/debug.h"
#include "game/level_update.h"
#include "game/object_helpers.h"
#include "game/object_list_processor.h"
#include "game/save_file.h"
#include "buffers/buffers.h"
#include "game/segment2.h"
#include "game/patch_selection_ui.h"
#include "game/ingame_menu.h"

#define STAR_MEDALLION_ENTRY(entry) {.entries = entry, .entryCount = ARRAY_COUNT(entry)}

struct StarMedallionSpawnLocation {
    const s8 areaNum;
    const u8 checkpointOnly;
    const s16 x;
    const s16 y;
    const s16 z;
    u8 (*conditionalFunc)(void);
};

struct StarMedallionCourse {
    const struct StarMedallionSpawnLocation *entries;
    const u32 entryCount;
};

static u8 sm_bob_spawn_on_island(void)         { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS) && (gCurrActNum >= 2 || chaos_check_if_patch_active(CHAOS_PATCH_UNLOCK_CANNONS))); }
static u8 sm_wf_wall_kick_below_platform(void) { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_wf_spawn_on_tower(void)           { return (gCurrActNum >= 2); }
static u8 sm_jrb_cannon_pole(void)             { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS) && (gCurrActNum >= 2 || chaos_check_if_patch_active(CHAOS_PATCH_UNLOCK_CANNONS))); }
static u8 sm_jrb_rocking_ship(void)            { return (gCurrActNum >= 2); }
static u8 sm_jrb_whirlpool(void)               { return (gCurrActNum >= 2); }
static u8 sm_ccm_a1_broken_bridge(void)        { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_LONG_JUMP)); }
static u8 sm_ccm_a1_cannon_unlocked(void)      { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS)); }
static u8 sm_ccm_a2_hidden_path(void)          { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_bbh_shack_at_start(void)          { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_bbh_can_go_upstairs(void)         { return ((gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)) || gCurrActNum >= 2); }
static u8 sm_bbh_can_go_extra_upstairs(void)   { return ((gChaosDifficulty > CHAOS_DIFFICULTY_EASY || gCurrActNum >= 2) && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_lll_a1_lava_check(void)           { return ((gCurrActNum >= 5 && !chaos_check_if_patch_active(CHAOS_PATCH_NO_RIDING_SHELLS)) || (!chaos_check_if_patch_active(CHAOS_PATCH_ONE_HIT_WONDER) && chaos_check_if_patch_active(CHAOS_PATCH_INSTAKILL_LAVA))); }
static u8 sm_ddd_a2_bowser_sub_active(void)    { return (gCurrActNum == 1); }
static u8 sm_ddd_a2_poles_active(void)         { return (gCurrActNum != 1); }
static u8 sm_sl_a1_shell_check(void)           { return !chaos_check_if_patch_active(CHAOS_PATCH_NO_RIDING_SHELLS); }
static u8 sm_wdw_a2(void)                      { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS) && gCurrActNum >= 5); }
static u8 sm_wdw_a2_vanish_cap(void)           { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS) && gCurrActNum >= 5 && (save_file_get_flags() & SAVE_FLAG_HAVE_VANISH_CAP) && !chaos_check_if_patch_active(CHAOS_PATCH_DISABLE_CAPS)); }
static u8 sm_ttm_a1_can_warp(void)             { return (!chaos_check_if_patch_active(CHAOS_PATCH_DISABLE_FADE_WARPS)); }
static u8 sm_ttm_a1_can_wall_kick(void)        { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_thi_a1_can_wall_kick(void)        { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_thi_a2_can_wall_kick(void)        { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_thi_a3_can_wall_kick(void)        { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_ttc_can_kick(void)                { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_KICK)); }
static u8 sm_ttc_moving_or_wall_kick(void)     { return (gTTCSpeedSetting != TTC_SPEED_STOPPED || !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_rr_cannon_unlocked(void)          { return (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS)); }
static u8 sm_rr_not_easy(void)                 { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY); }
static u8 sm_bitdw_not_easy(void)              { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY); }
static u8 sm_bitfs_not_easy(void)              { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY); }
static u8 sm_bits_can_long_jump(void)          { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY) && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_LONG_JUMP); }
static u8 sm_pss_can_wall_kick(void)           { return (gChaosDifficulty > CHAOS_DIFFICULTY_EASY && !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK)); }
static u8 sm_cotmc_can_double_jump(void)       { return !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_DOUBLE_JUMP); }
static u8 sm_cotmc_can_wall_kick(void)         { return !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK); }
static u8 sm_vcutm_can_wall_kick(void)         { return !chaos_check_if_patch_active(CHAOS_PATCH_LOSEMOVE_WALL_KICK); }

static const struct StarMedallionSpawnLocation starMedallionList_NONE[] = {};
static const struct StarMedallionSpawnLocation starMedallionList_BOB[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  7400, .y =  2321, .z = -7400, .conditionalFunc = NULL                          }, // Back corner halfway up stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2800, .y =  2600, .z = -2450, .conditionalFunc = NULL                          }, // In a break in the path up the mountain
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1550, .y =  4547, .z = -3750, .conditionalFunc = NULL                          }, // The top of the mountain
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4350, .y =  3322, .z =   500, .conditionalFunc = sm_bob_spawn_on_island        }, // On the floating island (cannon involved)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -200, .y =  1750, .z = -6650, .conditionalFunc = NULL                          }, // Sliding down the mountain (by the red coin)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1000, .y =  1073, .z = -1000, .conditionalFunc = NULL                          }, // Sliding down the mountain (with the boulders)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3583, .y =  1492, .z =  2560, .conditionalFunc = NULL                          }, // Above a tree in the fenced area
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5400, .y =  1198, .z = -1000, .conditionalFunc = NULL                          }, // Above a small rock by the chain chomp area
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2283, .y =   257, .z = -3682, .conditionalFunc = NULL                          }, // Above the purple switch in the cage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -650, .y =    25, .z =  5668, .conditionalFunc = NULL                          }, // Underneath the bridge near the start
};
static const struct StarMedallionSpawnLocation starMedallionList_WF[]    = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3800, .y =   700, .z =  -775, .conditionalFunc = sm_wf_wall_kick_below_platform}, // Under one of the moving platforms above the void
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -255, .y =  2910, .z =  1800, .conditionalFunc = NULL                          }, // Above the triangle thingy by the rotating platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -255, .y =  2050, .z =  1800, .conditionalFunc = NULL                          }, // Below the triangle thingy by the rotating platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2495, .y =  2165, .z =  -256, .conditionalFunc = NULL                          }, // At the top of the pole above the wild blue star
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   450, .y =  3800, .z = -1850, .conditionalFunc = NULL                          }, // Above the kickable board (when knocked down)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3462, .y =  2203, .z = -3314, .conditionalFunc = NULL                          }, // Above the lower Thwomp
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2300, .y =  2176, .z = -1900, .conditionalFunc = NULL                          }, // Slide by the Thwomp stairs
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2304, .y =  3834, .z = -2303, .conditionalFunc = NULL                          }, // Above one of the rotating platforms
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -897, .y =  3834, .z =  1413, .conditionalFunc = NULL                          }, // At the corner of the King Thwomp area
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -700, .y =  4735, .z =     0, .conditionalFunc = sm_wf_spawn_on_tower          }, // Near the top of the tower
};
static const struct StarMedallionSpawnLocation starMedallionList_JRB[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   659, .y =  3194, .z =  3314, .conditionalFunc = sm_jrb_cannon_pole            }, // Above the tallest blast star pole (cannon involved)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  7145, .y = -3300, .z =  2128, .conditionalFunc = NULL                          }, // Where the eel normally is on Act 2
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1600, .y = -2437, .z = -5550, .conditionalFunc = NULL                          }, // Above a tall rock in the front of the cave
    {.areaNum = 1, .checkpointOnly = FALSE, .x =    50, .y = -2435, .z = -6600, .conditionalFunc = NULL                          }, // Above a tall rock in the side of the cave
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2000, .y = -2441, .z = -3500, .conditionalFunc = NULL                          }, // Above a tall rock in the back of the cave
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5700, .y = -5050, .z =  3800, .conditionalFunc = NULL                          }, // Near the bottom of the abyss, next to the shipwreck
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1059, .y =   537, .z =  7072, .conditionalFunc = NULL                          }, // Under the floating bridge platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4855, .y =  1786, .z =  6320, .conditionalFunc = NULL                          }, // Over the coin path towards the ship
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4900, .y =  2785, .z =   600, .conditionalFunc = sm_jrb_rocking_ship           }, // Over back of rocking ship
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4979, .y = -4222, .z =  2482, .conditionalFunc = sm_jrb_whirlpool              }, // Over whirlpool
};
static const struct StarMedallionSpawnLocation starMedallionList_CCM[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   850, .y =  3438, .z =   -75, .conditionalFunc = NULL                          }, // Near a pebble by the snowman's body at the top
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   480, .y =  -261, .z =  2650, .conditionalFunc = NULL                          }, // On the ridge by the ice block with the red coin
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3300, .y = -1200, .z =  2075, .conditionalFunc = sm_ccm_a1_broken_bridge       }, // In between the broken bridge by the red coin star
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2300, .y =  -423, .z =  1550, .conditionalFunc = NULL                          }, // Tucked away by the blue coin switch
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2375, .y = -3280, .z =  3650, .conditionalFunc = NULL                          }, // By the wrong baby penguin
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3748, .y = -3883, .z =  4464, .conditionalFunc = sm_ccm_a1_cannon_unlocked     }, // Above the tree on the way to Wall Kicks Will Work (cannon involved)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5700, .y = -4357, .z = -2800, .conditionalFunc = NULL                          }, // In a corner near the tucked away red coin by the warp bridge at the bottom (not the actual bridge coin)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1000, .y = -4620, .z = -3150, .conditionalFunc = sm_ccm_a1_cannon_unlocked     }, // At the bottom of the Wall Kicks Will Work area (cannon involved)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4900, .y = -1745, .z = -3500, .conditionalFunc = NULL                          }, // At the midpoint section hidden by the 1-up box
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1942, .y =  1395, .z =  1678, .conditionalFunc = NULL                          }, // On the slide near the top, on the coin bridge (over the coins)
    // Area 2
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -6200, .y =  6906, .z = -5700, .conditionalFunc = NULL                          }, // Hidden behind the default camera at the start of the slide in a corner
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1200, .y =  4315, .z =  -250, .conditionalFunc = NULL                          }, // At the end of the coin jump on the slide
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  6640, .y =  -938, .z =   118, .conditionalFunc = NULL                          }, // Above the steep coin slope near the end of the slide
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -6150, .y = -4586, .z =  -175, .conditionalFunc = NULL                          }, // Before the ice section at the end of the slide
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -7025, .y = -5561, .z = -5925, .conditionalFunc = NULL                          }, // On the side of the ice blocks at the bottom of the slide
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -6550, .y = -4562, .z = -7525, .conditionalFunc = sm_ccm_a2_hidden_path         }, // At the end of the secret tunnel path
};
static const struct StarMedallionSpawnLocation starMedallionList_BBH[] = {
    // Outside
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3000, .y =  1376, .z =  5425, .conditionalFunc = sm_bbh_shack_at_start         }, // On top of the shack near spawn
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2661, .y =    46, .z =  5781, .conditionalFunc = NULL                          }, // At the top of the elevator in the shack
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   700, .y =    46, .z = -2000, .conditionalFunc = NULL                          }, // Behind the house by the 10 coin box
    // Main Floor
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1550, .y =   250, .z =  -900, .conditionalFunc = NULL                          }, // Inside the boo room behind the painting on the first floor
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -350, .y =   250, .z =  1800, .conditionalFunc = NULL                          }, // Inside the piano room by the door and window
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   725, .y =   250, .z = -1225, .conditionalFunc = NULL                          }, // Inside the small eyeball room
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3350, .y =   250, .z =  1550, .conditionalFunc = NULL                          }, // Inside the room with the scary hole, boo, and red coin
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2500, .y =   250, .z = -1385, .conditionalFunc = NULL                          }, // Inside the breakable bridge room, before breakable bridge (can't put it over bridge due to rooms)
    // Upstairs
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1068, .y =  1069, .z =  1408, .conditionalFunc = sm_bbh_can_go_upstairs        }, // Inside the viewing room by the red coin star
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1970, .y =  1274, .z =   775, .conditionalFunc = sm_bbh_can_go_upstairs        }, // Behind the moveable bookshelf
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1799, .y =  1069, .z =  1818, .conditionalFunc = sm_bbh_can_go_upstairs        }, // By the vanish cap box upstairs
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3320, .y =  1153, .z = -1185, .conditionalFunc = sm_bbh_can_go_upstairs        }, // On one of the caskets in the back
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2866, .y =  1069, .z =  2360, .conditionalFunc = sm_bbh_can_go_upstairs        }, // Next to the tilting floor trap (can't put it over trap due to rooms)
    // Even More Upstairs
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -325, .y =  1991, .z =  -475, .conditionalFunc = sm_bbh_can_go_extra_upstairs  }, // Before the vanish cap painting with the giant eyeball
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   660, .y =  3527, .z =   250, .conditionalFunc = sm_bbh_can_go_extra_upstairs  }, // On top of the mansion
    // Basement
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   975, .y = -2207, .z =  1800, .conditionalFunc = NULL                          }, // By the grate blocking the door
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y = -2207, .z = -1330, .conditionalFunc = NULL                          }, // Path right of the merry-go-round
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3250, .y = -2944, .z =  2075, .conditionalFunc = NULL                          }, // Underwater in the deepest point
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -200, .y = -2310, .z =  -600, .conditionalFunc = NULL                          }, // In the merry-go-round on the right side
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2661, .y = -2207, .z =  4394, .conditionalFunc = NULL                          }, // At the bottom of the elevator
};
static const struct StarMedallionSpawnLocation starMedallionList_HMC[] = {
    // Left Side Main Area
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6550, .y =  1074, .z =  3450, .conditionalFunc = NULL                          }, // Hidden below the long jump spot at the start of the level
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6775, .y =  1836, .z = -5750, .conditionalFunc = NULL                          }, // In that little hideout by the rolling rocks (next to the coins)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5275, .y =  3060, .z = -8000, .conditionalFunc = NULL                          }, // At the top of the wrong side of Watch For Rolling Rocks
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5292, .y =  2708, .z =    87, .conditionalFunc = NULL                          }, // By the 1-UP box in the hanging section after the upper toxic maze exit
    // Red Coin Area
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   672, .y =  2298, .z =  5120, .conditionalFunc = NULL                          }, // Near the start of the upper moving platform with red coins
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5755, .y =  2298, .z =  5940, .conditionalFunc = NULL                          }, // On one of the thin ledges. on the way through the upper red coins moving platform section
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6925, .y =  1274, .z =  7025, .conditionalFunc = NULL                          }, // At the end corner of the lower red coin moving platform section with the eyeball
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2400, .y =  1377, .z =  4150, .conditionalFunc = NULL                          }, // On top of the grate triangle thing, accessible from lower red coins moving platform
    // Toxic Maze
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2015, .y =   455, .z =   470, .conditionalFunc = NULL                          }, // On the way to the intended entrance of the toxic maze
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5911, .y =    46, .z =  1170, .conditionalFunc = NULL                          }, // Outside the normal exit of the toxic maze, at the bottom of the stairs
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6600, .y =  -202, .z = -1575, .conditionalFunc = NULL                          }, // In the alcove with no exit, by metal cap box
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2275, .y =  2605, .z = -3045, .conditionalFunc = NULL                          }, // At the top of the elevator after the upper toxic maze exit
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -450, .y =  2093, .z = -7350, .conditionalFunc = NULL                          }, // At the top of the elevator after the lower toxic maze exit
    // Basement
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4900, .y = -6000, .z =  4900, .conditionalFunc = NULL                          }, // In the water behind the star island
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -250, .y = -4439, .z =  3275, .conditionalFunc = NULL                          }, // On the ledge in the basement by the metal cap switch
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1315, .y = -4234, .z =  5815, .conditionalFunc = NULL                          }, // On the steel ledge before the metal cap area
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4125, .y = -4029, .z =  4025, .conditionalFunc = NULL                          }, // Behind the metal cap stage warp
};
static const struct StarMedallionSpawnLocation starMedallionList_LLL[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2349, .y =   889, .z =   950, .conditionalFunc = NULL                          }, // On top of the 1-up hill by the spinning volcano platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6300, .y =   886, .z = -6580, .conditionalFunc = NULL                          }, // On top of the 1-up hill in the back righthand corner of the stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =   557, .z = -2050, .conditionalFunc = NULL                          }, // On top of the flamethrower by the spinning volcano platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4150, .y =   813, .z =  4790, .conditionalFunc = NULL                          }, // At the end of the log rolling section
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6673, .y =   404, .z = -3060, .conditionalFunc = NULL                          }, // Above the eyeball on the right side of the stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2124, .y =   315, .z = -4607, .conditionalFunc = NULL                          }, // On the moving platform path by the big bullies
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6700, .y =   396, .z = -6700, .conditionalFunc = NULL                          }, // On the tilted ramp at the lefthand corner of the stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5119, .y =   762, .z = -4095, .conditionalFunc = NULL                          }, // On the rotating fire bar platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  7168, .y =   315, .z =  4614, .conditionalFunc = NULL                          }, // On the moving platform path by the rolling log
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1725, .y =    15, .z =  3650, .conditionalFunc = sm_lll_a1_lava_check          }, // Below the opening and closing bridge (shell involved)
    // Area 2 (checkpoint)
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  -200, .y =  1225, .z = -1350, .conditionalFunc = NULL                          }, // Jump in between the elevator platforms
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =   417, .y =  2605, .z =   583, .conditionalFunc = NULL                          }, // On rotating fire bar after elevators
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  -500, .y =  1303, .z =  2650, .conditionalFunc = NULL                          }, // Under crushing platform
    {.areaNum = 2, .checkpointOnly = TRUE,  .x = -2075, .y =  2155, .z =  -275, .conditionalFunc = NULL                          }, // On floating platform before flamethrowers halfway up edge path
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  1078, .y =  4322, .z = -2269, .conditionalFunc = NULL                          }, // On top of second to last pole on edge path, by the star
};
static const struct StarMedallionSpawnLocation starMedallionList_SSL[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5760, .y =   175, .z =  5750, .conditionalFunc = NULL                          }, // Above first sand pit at start of level
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5900, .y =   250, .z = -4865, .conditionalFunc = NULL                          }, // On isolated concrete block by wing cap box
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5650, .y =  -250, .z = -5900, .conditionalFunc = NULL                          }, // In small pond by cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5989, .y =   724, .z = -4850, .conditionalFunc = NULL                          }, // Above palm tree by cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2875, .y =  1018, .z = -2170, .conditionalFunc = NULL                          }, // On path to top of pyramid by fly guy
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6450, .y =   864, .z =  2250, .conditionalFunc = NULL                          }, // On top of flat stone platform by shell box
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3584, .y =   800, .z =  2934, .conditionalFunc = NULL                          }, // Above pit with tornado by pyramid entrance (tornado needed here)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1800, .y =  1374, .z =   800, .conditionalFunc = NULL                          }, // Above Pillar 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1805, .y =  1374, .z = -2536, .conditionalFunc = NULL                          }, // Above Pillar 2
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5869, .y =  1374, .z = -2535, .conditionalFunc = NULL                          }, // Above Pillar 3
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5896, .y =  1374, .z =   773, .conditionalFunc = NULL                          }, // Above Pillar 4

    // Area 2 (checkpoint)
    {.areaNum = 2, .checkpointOnly = TRUE,  .x = -1395, .y =   169, .z = -1435, .conditionalFunc = NULL                          }, // On isolated platform on left side, just above sand pit
    {.areaNum = 2, .checkpointOnly = TRUE,  .x = -3362, .y =   927, .z = -1250, .conditionalFunc = NULL                          }, // On jumping grindel at the bottom
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =     0, .y =  1146, .z = -2250, .conditionalFunc = NULL                          }, // Over back of entrance to Eyerok
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  2600, .y =  1228, .z = -2800, .conditionalFunc = NULL                          }, // On top of hanging platform in corner with amp and circle coins
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =     0, .y =  2124, .z =  2875, .conditionalFunc = NULL                          }, // On top of hanging platform with straight line coins and flowing sand
    {.areaNum = 2, .checkpointOnly = TRUE,  .x = -2450, .y =  2095, .z =   200, .conditionalFunc = NULL                          }, // By rolling spindel
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  1240, .y =  4192, .z =  1240, .conditionalFunc = NULL                          }, // On high platform after last pole, before jumping grindel
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  -870, .y =  4873, .z =     0, .conditionalFunc = NULL                          }, // On jumping grindel at the top
};
static const struct StarMedallionSpawnLocation starMedallionList_DDD[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  -300, .z =     0, .conditionalFunc = NULL                          }, // Hidden behind camera at start of level (at the top)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3174, .y = -3915, .z =   102, .conditionalFunc = NULL                          }, // Above whirlpool
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2880, .y = -5070, .z = -1850, .conditionalFunc = NULL                          }, // In seaweed
    // Area 2
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  2000, .y = -3168, .z = -2750, .conditionalFunc = NULL                          }, // Above Vanish Cap star's cage
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1600, .y = -3987, .z =  2800, .conditionalFunc = NULL                          }, // Between two clam shells
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  3355, .y = -3000, .z =  -515, .conditionalFunc = NULL                          }, // Above whirlpool
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  5950, .y = -2750, .z =  -700, .conditionalFunc = NULL                          }, // Inside coral (or whatever that is)
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  5760, .y =  1179, .z =   575, .conditionalFunc = NULL                          }, // On platform with Red Coin star
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  4275, .y =   289, .z =  3795, .conditionalFunc = sm_ddd_a2_bowser_sub_active   }, // On back right fin of Bowser Sub
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  3915, .y =  1235, .z =  -150, .conditionalFunc = sm_ddd_a2_bowser_sub_active   }, // On high fin of Bowser Sub
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1830, .y =  1100, .z =  -480, .conditionalFunc = sm_ddd_a2_poles_active        }, // On far pole by Blue Coin switch
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  4200, .y =  1100, .z = -2250, .conditionalFunc = sm_ddd_a2_poles_active        }, // On far pole behind two grates
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  2120, .y =  1100, .z =  3584, .conditionalFunc = sm_ddd_a2_poles_active        }, // On pole that moves past red coin platform left of Blue Coin switch
};
static const struct StarMedallionSpawnLocation starMedallionList_SL[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4375, .y =  2093, .z =  4565, .conditionalFunc = NULL                          }, // Above ice block area
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4050, .y =   809, .z = -2950, .conditionalFunc = NULL                          }, // Bottom of windy path
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   500, .y =   972, .z = -1600, .conditionalFunc = NULL                          }, // Tucked behind a ledge on hardened ice
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  1024, .z = -4900, .conditionalFunc = sm_sl_a1_shell_check          }, // Underneath ice bully area (shell involved)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6900, .y =  2298, .z = -6700, .conditionalFunc = NULL                          }, // In top corner by elevated red coins area (past star and shell boxes)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -700, .y =   844, .z =  2600, .conditionalFunc = NULL                          }, // In cold water on the right side of the moving snow triangles
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1025, .y =  1832, .z =  2750, .conditionalFunc = NULL                          }, // Past moving snow triangles, on slippery slope to the right
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1766, .y =  3540, .z =  -942, .conditionalFunc = NULL                          }, // Above tree before penguin bridge
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1200, .y =  3353, .z =  1225, .conditionalFunc = NULL                          }, // Halfway across penguin bridge
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  5588, .z =     0, .conditionalFunc = NULL                          }, // Above tree on top of snowman head in center of the stage
    // Area 2
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  -420, .y =   250, .z = -1740, .conditionalFunc = NULL                          }, // In between two yellow boxes by cannon bob-omb
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1600, .y =   250, .z =  1450, .conditionalFunc = NULL                          }, // On right side of area before the maze section
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -1403, .y =   250, .z =  -175, .conditionalFunc = NULL                          }, // Hidden on left side behind a turn
};
static const struct StarMedallionSpawnLocation starMedallionList_WDW[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2687, .y =  3132, .z =  3790, .conditionalFunc = NULL                          }, // On angled slope before cannon
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2687, .y =  2054, .z =  3650, .conditionalFunc = NULL                          }, // Below angled slope before cannon
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4408, .y =  2260, .z =  4408, .conditionalFunc = NULL                          }, // Top southeast corner of stage (max raisable water level not requiring level entry)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   909, .y =   455, .z = -1727, .conditionalFunc = NULL                          }, // Below elevator star, at the bottom of cage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1215, .y =  1607, .z =  3391, .conditionalFunc = NULL                          }, // Above a hidden platform after purple switch by the spawn area (assuming mid-height water)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   194, .y =  3450, .z =  3782, .conditionalFunc = NULL                          }, // By cannon bob-omb at the top of the stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -937, .y =  3841, .z =  1138, .conditionalFunc = NULL                          }, // Inside weird angled cylinder white brick thing at top of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3689, .y =    53, .z =  3205, .conditionalFunc = NULL                          }, // Hidden inside pushable box near the bottom of the stage (non-encased one)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1843, .y =  2357, .z = -3694, .conditionalFunc = NULL                          }, // Hidden inside pushable box near the top of the stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1537, .y =  1997, .z = -3582, .conditionalFunc = NULL                          }, // On top of spawn
    // Area 2
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  -465, .y =   -82, .z =  2094, .conditionalFunc = sm_wdw_a2                     }, // By water triangle in top center of area (cannon involved)
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  2048, .y = -2309, .z = -1279, .conditionalFunc = sm_wdw_a2                     }, // By two trees and 1-UP box (cannon involved)
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -3589, .y = -2309, .z =  3584, .conditionalFunc = sm_wdw_a2                     }, // Tucked in corner next to vanish cap box (cannon involved)
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1592, .y = -2309, .z =  3640, .conditionalFunc = sm_wdw_a2_vanish_cap          }, // Behind vanish cap cage (cannon involved)
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  2137, .y = -1357, .z =  1020, .conditionalFunc = sm_wdw_a2                     }, // Hidden behind breakable box on white building (cannon involved)
};
static const struct StarMedallionSpawnLocation starMedallionList_TTM[] = {
    // Area 1
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1500, .y = -4601, .z =  4350, .conditionalFunc = NULL                          }, // Hidden in water at the start
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   650, .y = -2418, .z =  3050, .conditionalFunc = NULL                          }, // Next to cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2000, .y = -1872, .z =  1500, .conditionalFunc = NULL                          }, // Hidden in water by first monkey that steals your cap
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3540, .y = -1250, .z =  3340, .conditionalFunc = NULL                          }, // Over rolling log (after it's been moved as much as it can)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1000, .y = -3423, .z = -4250, .conditionalFunc = sm_ttm_a1_can_warp            }, // By exit fading warp on way to cannon
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2250, .y =  -998, .z = -2200, .conditionalFunc = NULL                          }, // By four moles next to hanging coin bridge in red coin section
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1275, .y =  1447, .z =     0, .conditionalFunc = NULL                          }, // By sand vines floor or whatever with three goombas near top
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1030, .y =  2557, .z =   955, .conditionalFunc = NULL                          }, // Top of the stage by star
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4540, .y = -4357, .z =  1060, .conditionalFunc = NULL                          }, // Center of coin ring at the bottom of the stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4540, .y = -3500, .z = -1137, .conditionalFunc = sm_ttm_a1_can_wall_kick       }, // Gap over a void near the start of the stage (needs wall kicks)
    // Area 2 (checkpoint, not strictly checkpoint only but would be hell to find otherwise since this slide is super optional)
    {.areaNum = 2, .checkpointOnly = TRUE,  .x =  -375, .y =  2700, .z =  -675, .conditionalFunc = NULL                          }, // Jump near start of slide with yellow coin lines
    // Area 3 (checkpoint extension from Area 2)
    {.areaNum = 3, .checkpointOnly = TRUE,  .x = -7050, .y =  1745, .z =   900, .conditionalFunc = NULL                          }, // In between bouncing blue coins
    {.areaNum = 3, .checkpointOnly = TRUE,  .x = -1450, .y = -3463, .z =     0, .conditionalFunc = NULL                          }, // On wood plank section to the right of the skull death thing
    // Area 4 (checkpoint extension from Area 2)
    {.areaNum = 4, .checkpointOnly = TRUE,  .x =   100, .y =  1125, .z = -4800, .conditionalFunc = NULL                          }, // Final jump of the slide
    {.areaNum = 4, .checkpointOnly = TRUE,  .x = -7289, .y = -1513, .z = -5225, .conditionalFunc = NULL                          }, // Right side of slide exit
};
static const struct StarMedallionSpawnLocation starMedallionList_THI[] = {
    // Area 1 (Huge Island)
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4400, .y = -2509, .z = -6850, .conditionalFunc = NULL                          }, // By piranha flower area, next to where purple switch would be in tiny area
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5900, .y = -3533, .z = -1000, .conditionalFunc = NULL                          }, // Underwater, by carnivorous fish before the piranha flower section
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6800, .y = -2309, .z =  1000, .conditionalFunc = NULL                          }, // On rocky surface between spawn and piranha flower area
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -520, .y = -2304, .z =  6660, .conditionalFunc = NULL                          }, // Above wooden post on hexagonal platform requiring drop down or shell to get to
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  7168, .y = -1200, .z = -1500, .conditionalFunc = NULL                          }, // Above void, in vertical wind section
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4400, .y =   309, .z = -2150, .conditionalFunc = NULL                          }, // In bowling ball spawner square thing
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2309, .y =  -461, .z =  1581, .conditionalFunc = NULL                          }, // Hidden behind corner where koopa the quick normally is
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   199, .y =  2483, .z =   433, .conditionalFunc = NULL                          }, // Above horizontal coin line on bridge before top of mountain
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5000, .y = -1285, .z =  3500, .conditionalFunc = NULL                          }, // Lower section before tree where you cannon to for Area 3
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4250, .y = -1650, .z = -4807, .conditionalFunc = sm_thi_a1_can_wall_kick       }, // Next to windy bridge, wall kick to back wall and back
    // Area 2 (Tiny Island)
    {.areaNum = 2, .checkpointOnly = FALSE, .x =   -50, .y =  -520, .z = -2420, .conditionalFunc = NULL                          }, // Over hidden purple switch bridge
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1444, .y =   571, .z =   676, .conditionalFunc = NULL                          }, // Above tree by where you'd normally fire cannon to on huge island to get to subarea
    {.areaNum = 2, .checkpointOnly = FALSE, .x =     0, .y =  1417, .z =  -462, .conditionalFunc = NULL                          }, // Top of tiny island where you ground pound the water
    {.areaNum = 2, .checkpointOnly = FALSE, .x = -1850, .y =  -895, .z = -2250, .conditionalFunc = NULL                          }, // Hidden behind pipe by piranha flower area, above water
    {.areaNum = 2, .checkpointOnly = FALSE, .x =  1844, .y =  -750, .z = -1181, .conditionalFunc = sm_thi_a2_can_wall_kick       }, // By pipe where koopa race would normally end, wall kick between tiny ramp and edge where wind would be
    // Area 3 (Red Coin Cave)
    {.areaNum = 3, .checkpointOnly = FALSE, .x = -1915, .y =  1586, .z = -1100, .conditionalFunc = NULL                          }, // Top of high pillar next to red coin spawn
    {.areaNum = 3, .checkpointOnly = FALSE, .x = -1808, .y =  1634, .z =  1596, .conditionalFunc = NULL                          }, // Next to blue coin switch
    {.areaNum = 3, .checkpointOnly = FALSE, .x =   400, .y =  1562, .z =     0, .conditionalFunc = NULL                          }, // Above void near center of area, requires hanging to get to
    {.areaNum = 3, .checkpointOnly = FALSE, .x = -1847, .y =   700, .z =   819, .conditionalFunc = sm_thi_a3_can_wall_kick       }, // Wall kick right before ledge with blue coin switch

};
static const struct StarMedallionSpawnLocation starMedallionList_TTC[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1375, .y = -2600, .z =  1375, .conditionalFunc = NULL                          }, // Top of red coin section
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y = -2183, .z =     0, .conditionalFunc = NULL                          }, // Above center of rotating hand in center of stage (near the bottom)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   925, .y = -1725, .z =  -225, .conditionalFunc = NULL                          }, // Jump backward from rotating platform before first cage star back to platform at top of red coin section
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1065, .y = -1403, .z = -1550, .conditionalFunc = NULL                          }, // In first cage with star
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1845, .y = -1428, .z =  -775, .conditionalFunc = sm_ttc_can_kick               }, // Next to first cage with star, tucked below path forward
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1100, .y =   514, .z = -1025, .conditionalFunc = NULL                          }, // Above floating platform by blue coin switch / star
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   660, .y =  1601, .z =  1875, .conditionalFunc = sm_ttc_moving_or_wall_kick    }, // Across three moving bars after non-moving chokepoint (without wall kick anyway)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1883, .y =  3885, .z =   550, .conditionalFunc = sm_ttc_moving_or_wall_kick    }, // Beneath 1-UP box approaching top of stage, past three rotating platforms
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1550, .y =  5534, .z =  1550, .conditionalFunc = sm_ttc_moving_or_wall_kick    }, // Beneath thwomp section at top of stage, opposite side of 10-coin box
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1919, .y =  6216, .z =  1919, .conditionalFunc = sm_ttc_moving_or_wall_kick    }, // Directly beneath thwomp at top of stage, in crushing zone
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  6316, .z =     0, .conditionalFunc = sm_ttc_moving_or_wall_kick    }, // Above center of rotating hand in center of stage (top of stage)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  4500, .z =     0, .conditionalFunc = sm_ttc_moving_or_wall_kick    }, // Beneath 10-coin box near top of stage, in center of stage
};
static const struct StarMedallionSpawnLocation starMedallionList_RR[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6775, .y =  -628, .z =  -675, .conditionalFunc = NULL                          }, // Top of flamethrower along first carpet section
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   648, .y =  -900, .z =   -61, .conditionalFunc = NULL                          }, // Center of spinny platforms (above void)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2980, .y =   450, .z =   295, .conditionalFunc = NULL                          }, // Pole jump above slippery wood platform thing to return to spinny platforms
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2680, .y =   800, .z =   -30, .conditionalFunc = NULL                          }, // Red coin section, along lowering platform route by cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6931, .y =   800, .z =   -30, .conditionalFunc = NULL                          }, // Red coin section, wall kick section to get to cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4834, .y =  2614, .z =   -87, .conditionalFunc = NULL                          }, // On top of red coin section, above cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -100, .y =  3565, .z = -2345, .conditionalFunc = NULL                          }, // Tip of the windy ship
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3554, .y =  5045, .z = -2327, .conditionalFunc = NULL                          }, // Above top of pole on windy ship
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5325, .y =  4450, .z =   600, .conditionalFunc = sm_rr_cannon_unlocked         }, // In cannon ring firing to pole (cannon involved)
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5043, .y =  2196, .z =   300, .conditionalFunc = NULL                          }, // On top of rotating bridge platform heading toward ship
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6700, .y =  3173, .z = -2559, .conditionalFunc = NULL                          }, // End of bridge by house
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5275, .y =  3322, .z = -6225, .conditionalFunc = NULL                          }, // Inside back corner of big house
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4967, .y =  6701, .z = -5128, .conditionalFunc = NULL                          }, // Top of house, opposite side of 1-UP box
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5649, .y = -4348, .z =  6622, .conditionalFunc = NULL                          }, // Bottom of stage by 1-UP box
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6575, .y = -3200, .z =  6565, .conditionalFunc = NULL                          }, // Lower section of stage, below tilting platform before donuts
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5800, .y = -1706, .z =  5640, .conditionalFunc = sm_rr_not_easy                }, // Lower section of stage, on top of flamethrower after donuts
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  6785, .y = -1221, .z =  6565, .conditionalFunc = NULL                          }, // Back of Tricky Triangles section
};
static const struct StarMedallionSpawnLocation starMedallionList_BITDW[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -7810, .y = -2910, .z =  4900, .conditionalFunc = NULL                          }, // Above red coin next to spawn, above the void
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3089, .y = -2655, .z =  2845, .conditionalFunc = sm_bitdw_not_easy             }, // Top of flamethrower near spawn
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -204, .y = -1973, .z =  3682, .conditionalFunc = NULL                          }, // Center of rotating platforms near start of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4900, .y =  1400, .z = -1300, .conditionalFunc = NULL                          }, // Above void, jump from above moving yellow platform to spike platform with 1-UP box and amp
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4600, .y =  1581, .z =  -312, .conditionalFunc = NULL                          }, // Above spike thing on separate platform with amp near center of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   350, .y =   546, .z =   512, .conditionalFunc = NULL                          }, // Below first tilting platform near end of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   595, .y =  1074, .z =  -185, .conditionalFunc = NULL                          }, // On side of tilting platforms before last red coin, in different alcove
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1106, .y =  2298, .z =   512, .conditionalFunc = NULL                          }, // Above highest platform above tilting platforms near stage end
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2357, .y =  1500, .z = -2454, .conditionalFunc = NULL                          }, // Above three yellow coins by cycling pyramid platforms
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3350, .y =   894, .z =   725, .conditionalFunc = NULL                          }, // On thin wood path between center spike platform and last area with tilting platforms
};
static const struct StarMedallionSpawnLocation starMedallionList_BITFS[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3073, .y = -2892, .z =  -922, .conditionalFunc = NULL                          }, // First tilty platform near spawn
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2150, .y = -1950, .z = -1250, .conditionalFunc = NULL                          }, // Jump over lava between grates by first red coin at base of level
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4875, .y =   306, .z =  1090, .conditionalFunc = sm_bitfs_not_easy             }, // In back corner after elevator to second floor
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1230, .y =   460, .z =  -405, .conditionalFunc = NULL                          }, // Top of flamethrower on second floor
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1900, .y =   507, .z =   320, .conditionalFunc = NULL                          }, // Inside of coin ring in hanging section above lava
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -5705, .y =  1991, .z =     0, .conditionalFunc = NULL                          }, // Above hole in raisy-lowery ramp to third floor, maximally raised
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   614, .y =  4788, .z =    96, .conditionalFunc = NULL                          }, // Above first raisy-lowery pole on third floor, before Bowser entrance
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1686, .y =  3507, .z =   -60, .conditionalFunc = NULL                          }, // Corner of second raisy-lowery pole platform on third floor, before Bowser entrance
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1843, .y =  5273, .z =  -236, .conditionalFunc = NULL                          }, // On thin wall before red coin star spawn location
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2458, .y =  3581, .z = -1138, .conditionalFunc = sm_bitfs_not_easy             }, // On left side of edge of third floor, behind wall with raisy-lowery poles
};
static const struct StarMedallionSpawnLocation starMedallionList_BITS[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1015, .y = -4495, .z =   625, .conditionalFunc = NULL                          }, // Below tilting bridge near beginning
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4148, .y = -1200, .z =  -725, .conditionalFunc = NULL                          }, // Jump over void between rotating platform and path before it
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4750, .y = -2207, .z =  -800, .conditionalFunc = NULL                          }, // Below thwomp area nobody ever goes to
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -6425, .y =  -671, .z =  -787, .conditionalFunc = NULL                          }, // By flamespitter dude after thwomp nobody ever goes to
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -1762, .y =  3168, .z =  -904, .conditionalFunc = NULL                          }, // Above diamond thing with moving platform before bombs, pole, and spinny platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4769, .y =  2445, .z =  -904, .conditionalFunc = NULL                          }, // Above flamespitter dude above spike on spinny platform next to red coin before tall pole
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1450, .y =  6275, .z = -1850, .conditionalFunc = sm_bits_can_long_jump         }, // Above void, long jump from top spinny platform to windy platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   964, .y =  6549, .z = -3568, .conditionalFunc = NULL                          }, // On top of back right pillar in windy section before Bowser pipe
};
static const struct StarMedallionSpawnLocation starMedallionList_PSS[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5894, .y =  6394, .z = -4345, .conditionalFunc = NULL                          }, // Hidden behind camera at spawn
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3172, .y =  7060, .z = -5645, .conditionalFunc = sm_pss_can_wall_kick          }, // Above start of slide
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  3325, .z =  2280, .conditionalFunc = NULL                          }, // Jump where slide opens up, before steep slope with coins
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1866, .y =  -713, .z =  1690, .conditionalFunc = NULL                          }, // Above first gate thing where 1-UP normally is halfway down the slide
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4741, .y = -3007, .z = -2985, .conditionalFunc = NULL                          }, // Above pole thing near bottom of the slide
};
static const struct StarMedallionSpawnLocation starMedallionList_COTMC[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  -128, .z =  2700, .conditionalFunc = NULL                          }, // Before waterfall, requires metal cap
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  1150, .z = -1300, .conditionalFunc = sm_cotmc_can_wall_kick        }, // Wall kick up high halfway down corridor
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   100, .y =    15, .z = -3500, .conditionalFunc = NULL                          }, // Under tiny bridge by two red coins
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   912, .y =   281, .z = -3638, .conditionalFunc = NULL                          }, // Between two red coins in alcove
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  -975, .y =   885, .z = -6875, .conditionalFunc = sm_cotmc_can_double_jump      }, // Triple jump over collisionless crystal on left side of cap switch
};
static const struct StarMedallionSpawnLocation starMedallionList_TOTWC[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  2360, .z =     0, .conditionalFunc = NULL                          }, // Middle of second ring, right at start of level
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  5086, .y =  1025, .z = -5055, .conditionalFunc = NULL                          }, // Behind tall spire after first set of red coins
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2657, .y =     0, .z = -2742, .conditionalFunc = NULL                          }, // Below yellow coin, on way to third set of red coins
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2500, .y = -2500, .z =     0, .conditionalFunc = NULL                          }, // Below star spawn area
};
static const struct StarMedallionSpawnLocation starMedallionList_VCUTM[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -4705, .y =  1644, .z = -2460, .conditionalFunc = NULL                          }, // Isolated center platform halfway down slidey section
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2662, .y = -2177, .z =   670, .conditionalFunc = NULL                          }, // Slippery wall at end of slidey section at start of level
    {.areaNum = 1, .checkpointOnly = FALSE, .x =   750, .y = -2267, .z = -6250, .conditionalFunc = NULL                          }, // Beneath second tilty platform
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  1900, .y =  -650, .z = -6250, .conditionalFunc = NULL                          }, // Between second and third set of rotating platforms
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  4740, .y =   947, .z = -5100, .conditionalFunc = sm_vcutm_can_wall_kick        }, // Above grate surrounding star
};
static const struct StarMedallionSpawnLocation starMedallionList_WMOTR[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3290, .y =  3386, .z = -4477, .conditionalFunc = NULL                          }, // Below one of the grabbable vines at top of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3050, .y =  2100, .z = -4200, .conditionalFunc = NULL                          }, // On cloud with no red coins, inside horizontal coin ring
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  3996, .y = -1695, .z =  5477, .conditionalFunc = NULL                          }, // Above the grabbable vine at bottom of stage by cannon bob-omb
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -3575, .y =  -510, .z =  3125, .conditionalFunc = NULL                          }, // Inside cloud next to middle-height cannon
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2350, .y =  2432, .z =   240, .conditionalFunc = NULL                          }, // Inside vertical coin ring near spawn, highest of the bunch
};
static const struct StarMedallionSpawnLocation starMedallionList_SA[] = {
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y = -3568, .z =     0, .conditionalFunc = NULL                          }, // Inside horizontal coin ring in center of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y =  -375, .z =     0, .conditionalFunc = NULL                          }, // At very top of stage, centered
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2400, .y =  -750, .z =  2400, .conditionalFunc = NULL                          }, // Top northwest corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2400, .y =  -750, .z = -2400, .conditionalFunc = NULL                          }, // Top northeast corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2400, .y =  -750, .z =  2400, .conditionalFunc = NULL                          }, // Top southwest corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2400, .y =  -750, .z = -2400, .conditionalFunc = NULL                          }, // Top southeast corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =  2400, .y = -4582, .z =     0, .conditionalFunc = NULL                          }, // Bottom north corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y = -4582, .z =  2400, .conditionalFunc = NULL                          }, // Bottom west corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x = -2400, .y = -4582, .z =     0, .conditionalFunc = NULL                          }, // Bottom south corner of stage
    {.areaNum = 1, .checkpointOnly = FALSE, .x =     0, .y = -4582, .z = -2400, .conditionalFunc = NULL                          }, // Bottom east corner of stage
};

// NOTE: Some of these locations are too difficult to fairly throw conditionals at.
// Because of this, we'll need to disable this patch in hardcore mode and will need to regenerate these positions each time at random.
// This unfortunately negates the idea of using fixed seeds after each patch, meaning a player will be able to just farm for more convenient medallion spawns.
// Try to difficulty balance these where possible (at least enough to make farming deterring but not impossible if the game gets too hard).
static const struct StarMedallionCourse starMedallionCourseTable[COURSE_COUNT] = {
    [COURSE_NONE]  = STAR_MEDALLION_ENTRY(starMedallionList_NONE),
    [COURSE_BOB]   = STAR_MEDALLION_ENTRY(starMedallionList_BOB),
    [COURSE_WF]    = STAR_MEDALLION_ENTRY(starMedallionList_WF),
    [COURSE_JRB]   = STAR_MEDALLION_ENTRY(starMedallionList_JRB),
    [COURSE_CCM]   = STAR_MEDALLION_ENTRY(starMedallionList_CCM),
    [COURSE_BBH]   = STAR_MEDALLION_ENTRY(starMedallionList_BBH),
    [COURSE_HMC]   = STAR_MEDALLION_ENTRY(starMedallionList_HMC),
    [COURSE_LLL]   = STAR_MEDALLION_ENTRY(starMedallionList_LLL),
    [COURSE_SSL]   = STAR_MEDALLION_ENTRY(starMedallionList_SSL),
    [COURSE_DDD]   = STAR_MEDALLION_ENTRY(starMedallionList_DDD),
    [COURSE_SL]    = STAR_MEDALLION_ENTRY(starMedallionList_SL),
    [COURSE_WDW]   = STAR_MEDALLION_ENTRY(starMedallionList_WDW),
    [COURSE_TTM]   = STAR_MEDALLION_ENTRY(starMedallionList_TTM),
    [COURSE_THI]   = STAR_MEDALLION_ENTRY(starMedallionList_THI),
    [COURSE_TTC]   = STAR_MEDALLION_ENTRY(starMedallionList_TTC),
    [COURSE_RR]    = STAR_MEDALLION_ENTRY(starMedallionList_RR),
    [COURSE_BITDW] = STAR_MEDALLION_ENTRY(starMedallionList_BITDW),
    [COURSE_BITFS] = STAR_MEDALLION_ENTRY(starMedallionList_BITFS),
    [COURSE_BITS]  = STAR_MEDALLION_ENTRY(starMedallionList_BITS),
    [COURSE_PSS]   = STAR_MEDALLION_ENTRY(starMedallionList_PSS),
    [COURSE_COTMC] = STAR_MEDALLION_ENTRY(starMedallionList_COTMC),
    [COURSE_TOTWC] = STAR_MEDALLION_ENTRY(starMedallionList_TOTWC),
    [COURSE_VCUTM] = STAR_MEDALLION_ENTRY(starMedallionList_VCUTM),
    [COURSE_WMOTR] = STAR_MEDALLION_ENTRY(starMedallionList_WMOTR),
    [COURSE_SA]    = STAR_MEDALLION_ENTRY(starMedallionList_SA),
};

// Globals
s32 gStarMedallionCollected = FALSE;
s32 gStarMedallionActiveIndex = -1;

static void spawn_star_medallion(s32 index) {
    const struct StarMedallionSpawnLocation *entries = starMedallionCourseTable[gCurrCourseNum].entries;
    const u32 entryCount = starMedallionCourseTable[gCurrCourseNum].entryCount;

    if (index < 0) {
        return;
    } else if (index >= (s32) entryCount) {
        assert_args(index < (s32) entryCount, "spawn_star_medallion:\nIndex out of bounds: %d", index);
        return;
    }

    const struct StarMedallionSpawnLocation *entry = &entries[index];
    if (entry->areaNum != gCurrAreaIndex) {
        return;
    }

    struct Object *obj = spawn_object_at_origin(gMarioState->marioObj, 0, MODEL_STAR_MEDALLION, bhvStarMedallion);
    if (obj) {
        obj->oPosX = entry->x;
        obj->oPosY = entry->y;
        obj->oPosZ = entry->z;
        obj->oHomeX = obj->oPosX;
        obj->oHomeY = obj->oPosY;
        obj->oHomeZ = obj->oPosZ;
    }
}

static s32 generate_star_medallion_spawn(u8 isCheckpoint) {
    const struct StarMedallionSpawnLocation *entries = starMedallionCourseTable[gCurrCourseNum].entries;
    const u32 entryCount = starMedallionCourseTable[gCurrCourseNum].entryCount;
    s32 count = 0;
    s32 generatedIndex = 0;

    for (u32 i = 0; i < entryCount; i++) {
        const struct StarMedallionSpawnLocation *entry = &entries[i];
        if (isCheckpoint != entry->checkpointOnly || (entry->conditionalFunc && !entry->conditionalFunc())) {
            continue;
        }

        count++;
    }

    if (count == 0) {
        return -1;
    }

    generatedIndex = random_float() * count;

    for (u32 i = 0; i < entryCount; i++) {
        const struct StarMedallionSpawnLocation *entry = &entries[i];
        if (isCheckpoint != entry->checkpointOnly || (entry->conditionalFunc && !entry->conditionalFunc())) {
            continue;
        }

        if (generatedIndex <= 0) {
            return i;
        }

        generatedIndex--;
    }

    assert(FALSE, "generate_star_medallion_spawn:\nFailed to generate coin at index!");
    return -1;
}

void chs_act_star_medallion(void) {
    if (gCurrCourseNum >= COURSE_COUNT || gCurrCourseNum == COURSE_NONE) {
        return;
    }

    chs_lvlinit_star_medallion();
    chs_area_init_star_medallion();
}

void chs_area_init_star_medallion(void) {
    if (gStarMedallionCollected) {
        return;
    }

    spawn_star_medallion(gStarMedallionActiveIndex);
}

void chs_instwarp_post_star_medallion(UNUSED struct InstantWarp *warp) {
    chs_area_init_star_medallion();
}

void chs_lvlinit_star_medallion(void) {
    s32 isCheckpoint = FALSE;
    gStarMedallionCollected = FALSE;
    gStarMedallionActiveIndex = -1;

    if (gCurrLevelNum == LEVEL_BOWSER_1 || gCurrLevelNum == LEVEL_BOWSER_2 || gCurrLevelNum == LEVEL_BOWSER_3) {
        return;
    }

    // NOTE: If you enter a one-way warp into a checkpointable area without a star medallion collected, you have to either die or exit course (this is intentional).
    // Respawning with a checkpoint however will instead spawn a coin in the checkpointed area so that you can still resume from there and not have to go to a different course.
    if (gCurrCourseNum == gWarpCheckpoint.courseNum && gWarpCheckpoint.courseNum != COURSE_NONE) {
        isCheckpoint = TRUE;
    }

    gStarMedallionActiveIndex = generate_star_medallion_spawn(isCheckpoint);
}

u8 chs_star_medallion_can_collect_star(void) {
    if (!chaos_check_if_patch_active(CHAOS_PATCH_STAR_MEDALLION)
                || gStarMedallionActiveIndex < 0
                || gCurrCourseNum == COURSE_NONE
                || (gCurrLevelNum == LEVEL_BOWSER_1 || gCurrLevelNum == LEVEL_BOWSER_2 || gCurrLevelNum == LEVEL_BOWSER_3)
    ) {
        return TRUE;
    }

    return gStarMedallionCollected;
}
