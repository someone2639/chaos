#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"

#include "course_table.h"
#include "sounds.h"
#include "audio/external.h"
#include "engine/behavior_script.h"
#include "game/debug.h"
#include "game/level_update.h"
#include "game/save_file.h"
#include "buffers/buffers.h"

#define NUM_STARS 120

const char *starNames[COURSE_COUNT][7] = {
    [COURSE_NONE]  = {
        "Talk to Toad (1)",
        "Talk to Toad (2)",
        "Talk to Toad (3)",
        "Catch MIPS (1)",
        "Catch MIPS (2)",
        "",
        "",
    },
    [COURSE_BOB]   = {
        "Big Bob-omb on the Summit",
        "Footrace with Koopa the Quick",
        "Shoot to the Island in the Sky",
        "Find the 8 Red Coins",
        "Mario Wings to the Sky",
        "Behind Chain Chomp's Gate",
        "100 Coins (BoB)",
    },
    [COURSE_WF]    = {
        "Chip Off Whomp's Block",
        "To the Top of the Fortress",
        "Shoot into the Wild Blue",
        "Red Coins on the Floating Isle",
        "Fall onto the Caged Island",
        "Blast Away the Wall",
        "100 Coins (WF)",
    },
    [COURSE_JRB]   = {
        "Plunder in the Sunken Ship",
        "Can the Eel Come Out to Play?",
        "Treasure of the Ocean Cave",
        "Red Coins on the Ship Afloat",
        "Blast to the Stone Pillar",
        "Through the Jet Stream",
        "100 Coins (JRB)",
    },
    [COURSE_CCM]   = {
        "Slip Slidin' Away",
        "Li'l Penguin Lost",
        "Big Penguin Race",
        "Frosty Slide for 8 Red Coins",
        "Snowman's Lost His Head",
        "Wall Kicks Will Work",
        "100 Coins (CCM)",
    },
    [COURSE_BBH]   = {
        "Go on a Ghost Hunt",
        "Ride Big Boo's Merry-Go-Round",
        "Secret of the Haunted Books",
        "Seek the 8 Red Coins",
        "Big Boo's Balcony",
        "Eye to Eye in the Secret Room",
        "100 Coins (BBH)",
    },
    [COURSE_HMC]   = {
        "Swimming Beast in the Cavern",
        "Elevate for 8 Red Coins",
        "Metal-Head Mario Can Move",
        "Navigating the Toxic Maze",
        "A-maze-ing Emergency Exit",
        "Watch for Rolling Rocks",
        "100 Coins (HMC)",
    },
    [COURSE_LLL]   = {
        "Boil the Big Bully",
        "Bully the Bullies",
        "8-Coin Puzzle with 15 Pieces",
        "Red-Hot Log Rolling",
        "Hot-Foot-It into the Volcano",
        "Elevator Tour in the Volcano",
        "100 Coins (LLL)",
    },
    [COURSE_SSL]   = {
        "In the Talons of the Big Bird",
        "Shining Atop the Pyramid",
        "Inside the Ancient Pyramid",
        "Stand Tall on the Four Pillars",
        "Free Flying for 8 Red Coins",
        "Pyramid Puzzle",
        "100 Coins (SSL)",
    },
    [COURSE_DDD]   = {
        "Board Bowser's Sub",
        "Chests in the Current",
        "Pole-Jumping for Red Coins",
        "Through the Jet Stream",
        "The Manta Ray's Reward",
        "Collect the Caps...",
        "100 Coins (DDD)",
    },
    [COURSE_SL]    = {
        "Snowman's Big Head",
        "Chill with the Bully",
        "In the Deep Freeze",
        "Whirl from the Freezing Pond",
        "Shell Shreddin' for Red Coins",
        "Into the Igloo",
        "100 Coins (SL)",
    },
    [COURSE_WDW]   = {
        "Shocking Arrow Lifts!",
        "Top o' the Town",
        "Secrets in the Shallows & Sky",
        "Express Elevator--Hurry Up!",
        "Go to Town for Red Coins",
        "Quick Race Through Downtown!",
        "100 Coins (WDW)",
    },
    [COURSE_TTM]   = {
        "Scale the Mountain",
        "Mystery of the Monkey Cage",
        "Scary 'Shrooms, Red Coins",
        "Mysterious Mountainside",
        "Breathtaking View from Bridge",
        "Blast to the Lonely Mushroom",
        "100 Coins (TTM)",
    },
    [COURSE_THI]   = {
        "Pluck the Piranha Flower",
        "The Tip Top of the Huge Island",
        "Rematch with Koopa the Quick",
        "Five Itty Bitty Secrets",
        "Wiggler's Red Coins",
        "Make Wiggler Squirm",
        "100 Coins (THI)",
    },
    [COURSE_TTC]   = {
        "Roll into the Cage",
        "The Pit and the Pendulums",
        "Get a Hand",
        "Stomp on the Thwomp",
        "Timed Jumps on Moving Bars",
        "Stop Time for Red Coins",
        "100 Coins (TTC)",
    },
    [COURSE_RR]    = {
        "Cruiser Crossing the Rainbow",
        "The Big House in the Sky",
        "Coins Amassed in a Maze",
        "Swingin' in the Breeze",
        "Tricky Triangles!",
        "Somewhere over the Rainbow",
        "100 Coins (RR)",
    },
    [COURSE_BITDW] = {
        "Red Coins (BitDW)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_BITFS] = {
        "Red Coins (BitFS)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_BITS]  = {
        "Red Coins (BitS)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_PSS]   = {
        "Box Star (PSS)",
        "Timer Star (PSS)",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_COTMC] = {
        "Red Coins (CotMC)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_TOTWC] = {
        "Red Coins (TotWC)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_VCUTM] = {
        "Red Coins (VCutM)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_WMOTR] = {
        "Red Coins (WMotR)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
    [COURSE_SA]    = {
        "Red Coins (SA)",
        "",
        "",
        "",
        "",
        "",
        "",
    },
};

static const u8 starsPerCourse[COURSE_COUNT] = {
    [COURSE_NONE]  = 5,
    [COURSE_BOB]   = 7,
    [COURSE_WF]    = 7,
    [COURSE_JRB]   = 7,
    [COURSE_CCM]   = 7,
    [COURSE_BBH]   = 7,
    [COURSE_HMC]   = 7,
    [COURSE_LLL]   = 7,
    [COURSE_SSL]   = 7,
    [COURSE_DDD]   = 7,
    [COURSE_SL]    = 7,
    [COURSE_WDW]   = 7,
    [COURSE_TTM]   = 7,
    [COURSE_THI]   = 7,
    [COURSE_TTC]   = 7,
    [COURSE_RR]    = 7,
    [COURSE_BITDW] = 1,
    [COURSE_BITFS] = 1,
    [COURSE_BITS]  = 1,
    [COURSE_PSS]   = 2,
    [COURSE_COTMC] = 1,
    [COURSE_TOTWC] = 1,
    [COURSE_VCUTM] = 1,
    [COURSE_WMOTR] = 1,
    [COURSE_SA]    = 1,
};

static void print_star_collect_message(u8 shouldRemove, s32 courseNum, s32 starId) {
    s32 possessed = FALSE;
    u8 currentLevelStarFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(courseNum));
    const char *act = starNames[courseNum][starId];

    assert(starId < ARRAY_COUNT(starNames[0]), "print_star_collect_message:\nInvalid star index!");
    if (act == NULL || act[0] == '\0') {
        assert(FALSE, "print_star_collect_message:\nEmpty star name detected!");
        return;
    }

    if (currentLevelStarFlags & (1 << starId)) {
        possessed = TRUE;
    }

    if (shouldRemove) {
        if (possessed) {
            sprintf(gChaosInternalBuffer, "Star revoked: @FFFF00--%s@--------", act);
        } else {
            sprintf(gChaosInternalBuffer, "Star revoked: @1F1FFF--%s@--------", act);
        }
    } else {
        if (possessed) {
            sprintf(gChaosInternalBuffer, "Star obtained: @1F1FFF--%s@--------", act);
        } else {
            sprintf(gChaosInternalBuffer, "Star obtained: @FFFF00--%s@--------", act);
        }
    }

    chaosmsg_print(CHAOS_PATCH_NONE, gChaosInternalBuffer);
}

static void add_uncollected_star(void) {
    s32 totalStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
    if (totalStars == NUM_STARS) {
        assert(FALSE, "add_uncollected_star:\nTried to add uncollected star with 120 stars!");
        return;
    }
    s32 starToUpdate = random_u16() % (NUM_STARS - totalStars);

    for (s32 course = 0; course < ARRAY_COUNT(starsPerCourse); course++) {
        s32 starsInCourse = starsPerCourse[course] - save_file_get_course_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course));
        if (starsInCourse <= starToUpdate) {
            starToUpdate -= starsInCourse;
            continue;
        }

        s32 starFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course));
        for (s32 starIndex = 0; starIndex < 8; starFlags >>= 1, starIndex++) {
            if (!(starFlags & 1)) {
                if (starToUpdate == 0) {
                    print_star_collect_message(FALSE, course, starIndex);
                    save_file_set_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), (1 << starIndex));
                    gMarioState->numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
                    break;
                } else {
                    starToUpdate--;
                }
            }
        }

        break;
    }
}

static void remove_collected_star(void) {
    s32 totalStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
    if (totalStars == 0) {
        assert(FALSE, "remove_collected_star:\nTried to remove collected star with 0 stars!");
        return;
    }
    s32 starToUpdate = random_u16() % totalStars;

    for (s32 course = 0; course < ARRAY_COUNT(starsPerCourse); course++) {
        s32 starsInCourse = save_file_get_course_star_count(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course));
        if (starsInCourse <= starToUpdate) {
            starToUpdate -= starsInCourse;
            continue;
        }

        s32 starFlags = save_file_get_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course));
        for (s32 starIndex = 0; starFlags != 0; starFlags >>= 1, starIndex++) {
            if (starFlags & 1) {
                if (starToUpdate == 0) {
                    print_star_collect_message(TRUE, course, starIndex);
                    save_file_remove_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), (1 << starIndex));
                    gMarioState->numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
                    break;
                } else {
                    starToUpdate--;
                }
            }
        }

        break;
    }
}

static void update_any_star(u8 shouldRemove) {
    s32 starToUpdate = random_u16() % NUM_STARS;

    for (s32 course = 0; course < ARRAY_COUNT(starsPerCourse); course++) {
        s32 starsInCourse = starsPerCourse[course];
        if (starsInCourse <= starToUpdate) {
            starToUpdate -= starsInCourse;
            continue;
        }

        print_star_collect_message(shouldRemove, course, starToUpdate);
        if (shouldRemove) {
            save_file_remove_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), 1 << starToUpdate);
        } else {
            save_file_set_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), 1 << starToUpdate);
        }
        gMarioState->numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);

        break;
    }
}

u8 chs_cond_star_shuffle(void) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) >= 2;
}
u8 chs_cond_stars_increase_guarantee(void) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) <= (NUM_STARS - 1);
}
u8 chs_cond_stars_decrease_guarantee(void) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) >= 1;
}

void chs_act_star_shuffle(void) {
    remove_collected_star();
    remove_collected_star();
    add_uncollected_star();
    add_uncollected_star();
}
void chs_act_stars_increase_lv2(void) {
    update_any_star(FALSE);
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
void chs_act_stars_increase_lv3(void) {
    update_any_star(FALSE);
    update_any_star(FALSE);
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
void chs_act_stars_increase_guarantee(void) {
    add_uncollected_star();
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
void chs_act_stars_decrease_lv2(void) {
    update_any_star(TRUE);
}
void chs_act_stars_decrease_lv3(void) {
    update_any_star(TRUE);
    update_any_star(TRUE);
}
void chs_act_stars_decrease_guarantee(void) {
    remove_collected_star();
}

void chs_act_get_key_1(void) {
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_1);
}

u8 chs_cond_get_key_1(void) {
    return (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_1 | SAVE_FLAG_UNLOCKED_BASEMENT_DOOR)));
}

void chs_act_get_key_2(void) {
    save_file_set_flags(SAVE_FLAG_HAVE_KEY_2);
}

u8 chs_cond_get_key_2(void) {
    return (!(save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR)));
}

void chs_act_unlock_cannons(void) {
    for(int i = 0; i < COURSE_COUNT; i++) {
        gSaveBuffer.files[gCurrSaveFileNum - 1].courseStars[i] |= (1 << 7);
    }
    gSaveFileModified = TRUE;
}

u8 chs_cond_unlock_cannons(void) {
    for(int i = 0; i < COURSE_COUNT; i++) {
        if((gSaveBuffer.files[gCurrSaveFileNum - 1].courseStars[i] & (1 << 7)) == 0){
            return TRUE;
        }
    }

    return FALSE;
}
