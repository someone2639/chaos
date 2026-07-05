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

static u8 inRandomPatchActivationFunc = FALSE;

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

        if (patch->effectType != CHAOS_EFFECT_NEGATIVE || (patch->durationType == CHAOS_DURATION_STARS && entry->remainingDuration <= 1)) {
            continue;
        }

        s32 blacklistedMatchFound = FALSE;
        for (s32 j = 0; j < ARRAY_COUNT(patchBlacklist); j++) {
            if (patchId == patchBlacklist[j]) {
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

/**
 * @brief Randomly generate and activate a random patch based on the provided criteria.
 * 
 * @param patchSeverity Should the patch be forced to any particular severity? (See chaos_roll_for_new_patches() description for more info.)
 * @param effectType Should the returned patch be positive or negative?
 * @param skipSeverityModifier Whether non-Normal difficulties should skip applying severity modifier when generating patches (e.g. if Hard mode and severity 2 patch is generated, should positive patch not become a severity 1?)
 * @param maxForcedDuration Maximum number of stars the patch should last (e.g. if set to 2, something that normally lasts 1 star will still last 1 star, but something that normally lasts 4 stars will only last 2 stars.)
 * @param durationType Type of patch to generate (e.g. star duration, infinite, use count, etc.)
 * @return Generated patch ID
 */
enum ChaosPatchID chs_activate_random_pos_neg_patch_of_severity(s32 patchSeverity, enum ChaosPatchEffectType effectType, u8 skipSeverityModifier, u32 maxForcedDuration, enum ChaosPatchDurationType durationType) {
    // Ideally this doesn't recurse at all, but just in case...
    s32 lastRandomPatch = inRandomPatchActivationFunc;
    inRandomPatchActivationFunc = TRUE;

    // Same here...
    enum ChaosPatchDurationType lastDurType = gChaosForcedDurationType;
    gChaosForcedDurationType = durationType;

    // And here...
    u8 lastSkipSeverityModifier = gChaosSkipSeverityDifficultyModifier;
    gChaosSkipSeverityDifficultyModifier = skipSeverityModifier;

    // And here...
    u32 lastDuration = gChaosForcedDurationMaximum;
    gChaosForcedDurationMaximum = maxForcedDuration;

    enum ChaosPatchID newPatch = CHAOS_PATCH_NONE_POSITIVE;

    // Generate new patches, with new rank override and explicitly without any special event
    const struct ChaosPatchSelection *generatedPatches = chaos_roll_for_new_patches(patchSeverity, CHAOS_SPECIAL_NONE);

    // Select first generated patch card's positive or negative entry
    if (effectType == CHAOS_EFFECT_POSITIVE) {
        newPatch = generatedPatches[0].positiveId;
    } else if (effectType == CHAOS_EFFECT_NEGATIVE) {
        newPatch = generatedPatches[0].negativeId;
    } else {
        assert_args(FALSE, "chs_activate_random_pos_neg_patch_of_severity:\nInvalid effectType: %d", effectType);
        newPatch = CHAOS_PATCH_NONE_POSITIVE;
    }

    chaosmsg_print(newPatch, "New patch activated: %s");
    chaos_add_new_entry(newPatch);

    gChaosForcedDurationMaximum = lastDuration;
    gChaosSkipSeverityDifficultyModifier = lastSkipSeverityModifier;
    gChaosForcedDurationType = lastDurType;
    inRandomPatchActivationFunc = lastRandomPatch;

    return newPatch;
}

u8 chs_cond_add_random_buff(void) {
    // This is not an eligible patch if it's already in the process of being activated (recursion moment).
    return (!inRandomPatchActivationFunc);
}

void chs_act_add_random_buff(void) {
    // Generate random severity override, to better balance potential patch distribution
    s32 patchSeverity = (random_u16() % CHAOS_PATCH_SEVERITY_MAX) + 1;

    // Generate new patches, with new rank override and explicitly without any special event
    chs_activate_random_pos_neg_patch_of_severity(patchSeverity, CHAOS_EFFECT_POSITIVE, TRUE, 0, CHAOS_DURATION_DO_NOT_FORCE);
}

u8 chs_cond_bite_sized_split(void) {
    // This is not an eligible patch if it's already in the process of being activated (recursion moment).
    // Make sure it's also not on easy difficulty, since it may become misleading what a rank 1 patch even is.
    return (!inRandomPatchActivationFunc && gChaosDifficulty != CHAOS_DIFFICULTY_EASY);
}

void chs_act_bite_sized_split(void) {
    // Generate new patches, with rank 1 override and explicitly without any special event
    chs_activate_random_pos_neg_patch_of_severity(1, CHAOS_EFFECT_NEGATIVE, TRUE, 0, CHAOS_DURATION_DO_NOT_FORCE);
    chs_activate_random_pos_neg_patch_of_severity(1, CHAOS_EFFECT_NEGATIVE, TRUE, 0, CHAOS_DURATION_DO_NOT_FORCE);
}

void chs_act_remove_negative_patch(void) {
    s32 negativePatchCount = 0;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const enum ChaosPatchID patchId = entry->id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->effectType != CHAOS_EFFECT_NEGATIVE || (patch->durationType == CHAOS_DURATION_STARS && entry->remainingDuration <= 1)) {
            continue;
        }

        s32 blacklistedMatchFound = FALSE;
        for (s32 j = 0; j < ARRAY_COUNT(patchBlacklist); j++) {
            if (patchId == patchBlacklist[j]) {
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

        if (patch->effectType != CHAOS_EFFECT_NEGATIVE || (patch->durationType == CHAOS_DURATION_STARS && entry->remainingDuration <= 1)) {
            continue;
        }

        s32 blacklistedMatchFound = FALSE;
        for (s32 j = 0; j < ARRAY_COUNT(patchBlacklist); j++) {
            if (patchId == patchBlacklist[j]) {
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

        chaos_remove_expired_entry_deferred(patchId, "Patch deactivated: %s");
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
        save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) < CHAOS_MIN_STARS_FOR_EVENTS
    );
}

u8 chs_cond_unlucky_charm(void) {
    return !(
        gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE ||
        save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) < CHAOS_MIN_STARS_FOR_EVENTS
    );
}

u8 chs_cond_uneventful(void) {
    return !(
        gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE ||
        save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) < CHAOS_MIN_STARS_FOR_EVENTS
    );
}

u8 chs_cond_forgiveness(void) {
    return (gChaosDifficulty != CHAOS_DIFFICULTY_IMPOSSIBLE);
}
