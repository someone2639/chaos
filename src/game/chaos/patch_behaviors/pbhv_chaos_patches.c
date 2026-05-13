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

static u8 inRandomBuffActivationFunc = FALSE;

static const enum ChaosPatchID patchBlacklist[] = {
    CHAOS_PATCH_LOWER_TIME_LIMIT,
    CHAOS_PATCH_MARIO_BIG,
    CHAOS_PATCH_MARIO_SMALL,
    CHAOS_PATCH_AD_BREAK,
};

u8 chs_cond_remove_negative_patch(void) {
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const enum ChaosPatchID patchId = entry->id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->effectType != CHAOS_EFFECT_NEGATIVE) {
            continue;
        }

        s32 blacklistedMatchFound = FALSE;
        for (s32 j = 0; j < ARRAY_COUNT(patchBlacklist); j++) {
            if (patchId == patchBlacklist[i]) {
                blacklistedMatchFound = TRUE;
                break;
            }
        }

        if (blacklistedMatchFound) {
            continue;
        }

        return TRUE;
    }

    return FALSE;
}

u8 chs_cond_add_random_buff(void) {
    // This is not an eligible patch if it's already in the process of being activated (recursion moment).
    return (!inRandomBuffActivationFunc);
}

void chs_act_add_random_buff(void) {
    inRandomBuffActivationFunc = TRUE;

    // Generate random severity override, to better balance potential patch distribution
    s32 patchSeverity = (random_u16() % CHAOS_PATCH_SEVERITY_MAX) + 1;

    // Generate new patches, with new rank override and explicitly without any special event
    const struct ChaosPatchSelection *generatedPatches = chaos_roll_for_new_patches(patchSeverity, CHAOS_SPECIAL_NONE);

    // Select first generated patch card's positive entry
    chaos_add_new_entry(generatedPatches[0].positiveId);
    chaosmsg_print(generatedPatches[0].positiveId, "New patch activated: %s");
    
    inRandomBuffActivationFunc = FALSE;
}

void chs_act_remove_negative_patch(void) {
    s32 negativePatchCount = 0;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const enum ChaosPatchID patchId = entry->id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->effectType != CHAOS_EFFECT_NEGATIVE) {
            continue;
        }

        s32 blacklistedMatchFound = FALSE;
        for (s32 j = 0; j < ARRAY_COUNT(patchBlacklist); j++) {
            if (patchId == patchBlacklist[i]) {
                blacklistedMatchFound = TRUE;
                break;
            }
        }

        if (blacklistedMatchFound) {
            continue;
        }

        negativePatchCount++;
    }

    if (negativePatchCount == 0) {
        assert(FALSE, "chs_act_remove_negative_patch:\nNo non-blacklisted negative patches found!");
        return;
    }

    s32 removalIndex = random_float() * negativePatchCount;
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const enum ChaosPatchID patchId = entry->id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->effectType != CHAOS_EFFECT_NEGATIVE) {
            continue;
        }

        s32 blacklistedMatchFound = FALSE;
        for (s32 j = 0; j < ARRAY_COUNT(patchBlacklist); j++) {
            if (patchId == patchBlacklist[i]) {
                blacklistedMatchFound = TRUE;
                break;
            }
        }

        if (blacklistedMatchFound) {
            continue;
        }

        if (removalIndex > 0) {
            removalIndex--;
            continue;
        }

        chaos_remove_expired_entry(i, "Patch deactivated: %s");
        return;
    }
}

u8 chs_cond_add_selectable_patch(void) {
    return (DEFAULT_PATCH_DISPLAY_QUANTITY + chaos_count_active_instances(CHAOS_PATCH_ADD_SELECTABLE_PATCH) < CHAOS_PATCH_MAX_GENERATABLE);
}

u8 chs_cond_remove_selectable_patch(void) {
    return (DEFAULT_PATCH_DISPLAY_QUANTITY - chaos_count_active_instances(CHAOS_PATCH_REMOVE_SELECTABLE_PATCH) > 1);
}

u8 chs_cond_lucky_charm(void) {
    return !(
        gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE ||
        chaos_check_if_patch_active(CHAOS_PATCH_UNLUCKY_CHARM) ||
        save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) < CHAOS_MIN_STARS_FOR_EVENTS
    );
}

u8 chs_cond_unlucky_charm(void) {
    return !(
        gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE ||
        chaos_check_if_patch_active(CHAOS_PATCH_LUCKY_CHARM) ||
        chaos_check_if_patch_active(CHAOS_PATCH_UNEVENTFUL) ||
        save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) < CHAOS_MIN_STARS_FOR_EVENTS
    );
}

u8 chs_cond_uneventful(void) {
    return !(
        gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE ||
        chaos_check_if_patch_active(CHAOS_PATCH_UNLUCKY_CHARM) ||
        save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) < CHAOS_MIN_STARS_FOR_EVENTS
    );
}

u8 chs_cond_positive_extension(void) {
    return !chaos_check_if_patch_active(CHAOS_PATCH_NEGATIVE_EXTENSION);
}

u8 chs_cond_negative_extension(void) {
    return !chaos_check_if_patch_active(CHAOS_PATCH_POSITIVE_EXTENSION);
}
