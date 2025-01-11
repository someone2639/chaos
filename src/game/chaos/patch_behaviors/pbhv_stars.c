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

#define NUM_STARS 120

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
        for (s32 starIndex = 1; starIndex < (1 << 8); starFlags >>= 1, starIndex <<= 1) {
            if (!(starFlags & 1)) {
                if (starToUpdate == 0) {
                    save_file_set_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), starIndex);
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
        for (s32 starIndex = 1; starFlags != 0; starFlags >>= 1, starIndex <<= 1) {
            if (starFlags & 1) {
                if (starToUpdate == 0) {
                    save_file_remove_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), starIndex);
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

        if (shouldRemove) {
            save_file_remove_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), 1 << starToUpdate);
        } else {
            save_file_set_star_flags(gCurrSaveFileNum - 1, COURSE_NUM_TO_INDEX(course), 1 << starToUpdate);
        }
        gMarioState->numStars = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);

        break;
    }
}

u8 chs_cond_stars_increase_guarantee(UNUSED const struct ChaosPatch *patch) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) <= (NUM_STARS - 1);
}
u8 chs_cond_stars_decrease_guarantee(UNUSED const struct ChaosPatch *patch) {
    return save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) >= 2;
}

void chs_act_stars_increase_lv2(UNUSED const struct ChaosPatch *patch) {
    update_any_star(FALSE);
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
void chs_act_stars_increase_lv3(UNUSED const struct ChaosPatch *patch) {
    update_any_star(FALSE);
    update_any_star(FALSE);
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
void chs_act_stars_increase_guarantee(UNUSED const struct ChaosPatch *patch) {
    add_uncollected_star();
    play_sound(SOUND_MENU_STAR_SOUND, gGlobalSoundSource);
}
void chs_act_stars_decrease_lv2(UNUSED const struct ChaosPatch *patch) {
    update_any_star(TRUE);
    update_any_star(TRUE);
}
void chs_act_stars_decrease_lv3(UNUSED const struct ChaosPatch *patch) {
    update_any_star(TRUE);
    update_any_star(TRUE);
    update_any_star(TRUE);
}
void chs_act_stars_decrease_guarantee(UNUSED const struct ChaosPatch *patch) {
    remove_collected_star();
    remove_collected_star();
}
