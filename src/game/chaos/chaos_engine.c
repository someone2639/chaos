#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

#include "string.h"
#include "buffers/buffers.h"
#include "engine/behavior_script.h"
#include "game/debug.h"
#include "game/object_list_processor.h"
#include "game/level_update.h"

#define WEIGHT_OFFSET 1.25f // Must be > 0!
#define RETRY_ATTEMPTS_DUPLICATES 5
#define DUPLICATE_ALLOWANCE 0.8f

#define EVENT_ODDS_LUCKY   0.175f
#define EVENT_ODDS_UNLUCKY 0.175f
#define EVENT_ODDS_CHAOS   0.125f

static u32 activePatchCounts[CHAOS_PATCH_COUNT];
static u8 availablePatches[CHAOS_PATCH_COUNT];
static struct ChaosPatchSelection generatedPatches[CHAOS_PATCH_MAX_GENERATABLE];
static enum ChaosPatchID deferredPatchesForRemoval[CHAOS_PATCH_DEFERRED_QUEUE_SIZE];
static u32 deferredPatchCount = 0;
static s32 unsafeDeactivationFunc = FALSE;

char gChaosInternalBuffer[0x1000];

s32 *gChaosActiveEntryCount = NULL;
struct ChaosActiveEntry *gChaosActiveEntries = NULL;
u8 gChaosLevelWarped = FALSE;
u8 gChaosBlueStarLastCollected = FALSE;
u8 gChaosImmediateActDeact = FALSE;
u8 gChaosCancelOutLostDuration = FALSE;
u8 gChaosSkipSeverityDifficultyModifier = FALSE;
s32 gChaosLastForcedSeverity = -2;
enum ChaosPatchSpecialEvent gChaosLastEventType = CHAOS_SPECIAL_NONE;

enum ChaosDifficulty gChaosDifficulty = CHAOS_DIFFICULTY_NORMAL;
enum ChaosGameMode gChaosGameMode = CHAOS_GAMEMODE_CLASSIC;
enum ChaosPatchDurationType gChaosForcedDurationType = CHAOS_DURATION_DO_NOT_FORCE;
u32 gChaosForcedDurationMaximum = 0;
enum ChaosPatchID gChaosNegativePatchCompare = CHAOS_PATCH_NONE;

static const f32 difficultyWeights[CHAOS_DIFFICULTY_COUNT][CHAOS_PATCH_SEVERITY_COUNT - 1] = {
    [CHAOS_DIFFICULTY_EASY      ] = { 0.12f, 0.25f, 0.38f }, // Difficulty offset should make highest level more common
    [CHAOS_DIFFICULTY_NORMAL    ] = { 0.25f, 0.25f, 0.25f }, // Difficulty probability is balanced across the board
    [CHAOS_DIFFICULTY_HARD      ] = { 0.15f, 0.25f, 0.35f }, // Difficulty offset doesn't really matter as much for hard, so just make highest level more common
    [CHAOS_DIFFICULTY_IMPOSSIBLE] = { 0.15f, 0.25f, 0.35f }, // Difficulty offset doesn't really matter as much for impossible either, so just make highest level more common
};

static void chaos_recompute_active_patch_counts(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    bzero(activePatchCounts, sizeof(activePatchCounts));
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        activePatchCounts[gChaosActiveEntries[i].id]++;
    }
}

// NOTE: Function duplicated in chaos_check_conditional_func (for optimization)
u8 chaos_check_if_patch_active(const enum ChaosPatchID patchId) {
    return (activePatchCounts[patchId] > 0 || patchId == gChaosNegativePatchCompare);
}

static u8 chaos_check_conditional_func(const struct ChaosPatch *patch) {
    if (patch->disableForHardcore && (gChaosGameMode == CHAOS_GAMEMODE_HARDCORE || chaos_check_if_patch_active(CHAOS_PATCH_INSTANT_GAME_OVER))) {
        return FALSE;
    }
    if (gChaosForcedDurationType != CHAOS_DURATION_DO_NOT_FORCE && gChaosForcedDurationType != patch->durationType) {
        return FALSE;
    }

    assert(!(patch->incompatibleCount > 0 && patch->incompatible == NULL), "chaos_check_conditional_func:\nincompatible array is NULL, despite incompatibleCount being nonzero!");
    for (u32 i = 0; i < patch->incompatibleCount; i++) {
        // NULL pointer exception if incompatibleCount is wrongly non-zero for some reason
        const enum ChaosPatchID patchId = patch->incompatible[i];

        // NOTE: Inline of chaos_check_if_patch_active (for optimization)
        if (activePatchCounts[patchId] > 0 || patchId == gChaosNegativePatchCompare) {
            return FALSE;
        }
    }

    if (patch->conditionalFunc) {
        return patch->conditionalFunc();
    }

    return TRUE;
}

static void chaos_swap_active_entry_indexes(struct ChaosActiveEntry *first, struct ChaosActiveEntry *second) {
    struct ChaosActiveEntry tmp = *first;
    *first = *second;
    *second = tmp;
}

#define DO_SWAP { \
    chaos_swap_active_entry_indexes(entry1, entry2); \
    continue; \
}
#define NO_SWAP { \
    continue; \
}
void chaos_sort_active_patches(void) {
    s32 count = *gChaosActiveEntryCount;
    for (s32 i = 0; i < count - 1; i++) {
        for (s32 j = i + 1; j < count; j++) {
            struct ChaosActiveEntry *entry1 = &gChaosActiveEntries[i];
            struct ChaosActiveEntry *entry2 = &gChaosActiveEntries[j];
            const struct ChaosPatch *patch1 = &gChaosPatches[entry1->id];
            const struct ChaosPatch *patch2 = &gChaosPatches[entry2->id];

            // Sort first by duration type, favor showing temporary patches in general
            if (patch1->durationType != patch2->durationType) {
                if (patch1->durationType < patch2->durationType) {
                    DO_SWAP;
                }
                NO_SWAP;
            }

            // Sort next by duration remaining, if applicable
            if ((patch1->durationType == CHAOS_DURATION_STARS 
                        || patch1->durationType == CHAOS_DURATION_USE_COUNT)
                        && entry1->remainingDuration != entry2->remainingDuration
            ) {
                if (entry1->remainingDuration > entry2->remainingDuration) {
                    DO_SWAP;
                }
                NO_SWAP;
            }

            // Sort next by effect type, favoring positive over negative here
            if (patch1->effectType != patch2->effectType) {
                if (patch2->effectType == CHAOS_EFFECT_POSITIVE) {
                    DO_SWAP;
                }
                NO_SWAP;
            }

            // Sort next by severity, favoring high severity
            if (patch1->severity != patch2->severity) {
                if (patch1->severity < patch2->severity) {
                    DO_SWAP;
                }
                NO_SWAP;
            }

            // Sort last by patch id, favoring a lower index
            if (entry1->id > entry2->id) {
                DO_SWAP;
            }

            // Lower or equal patch id, do not swap
            NO_SWAP;
        }
    }
}
#undef DO_SWAP
#undef NO_SWAP

s32 chaos_find_first_active_patch(const enum ChaosPatchID patchId, struct ChaosActiveEntry **firstFoundMatch) {
    if (firstFoundMatch) {
        *firstFoundMatch = NULL;
    }

    if (!gChaosActiveEntryCount) {
        return -1;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == patchId) {
            if (firstFoundMatch) {
                *firstFoundMatch = &gChaosActiveEntries[i];
            }

            return i;
        }
    }

    return -1;
}

u32 chaos_count_active_instances(const enum ChaosPatchID patchId) {
    u32 count = 0;

    if (!gChaosActiveEntryCount) {
        return 0;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == patchId) {
            count++;
        }
    }

    return count;
}

u32 chaos_calculate_patch_duration(const struct ChaosPatch *patch) {
    assert(patch != NULL, "chaos_calculate_patch_duration: patch is NULL!");

    s32 duration = patch->duration;
    if (gChaosDifficulty == CHAOS_DIFFICULTY_HARD) {
        if (patch->durationHard > 0) {
            duration = patch->durationHard;
        } else if (patch->durationType == CHAOS_DURATION_STARS) {
            if (patch->effectType == CHAOS_EFFECT_POSITIVE) {
                duration += HARD_DURATION_DEFAULT_OFFSET_POSITIVE;
            } else if (patch->effectType == CHAOS_EFFECT_NEGATIVE) {
                duration += HARD_DURATION_DEFAULT_OFFSET_NEGATIVE;
            }
        }
    } else if (gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE) {
        if (patch->durationImpossible > 0) {
            duration = patch->durationImpossible;
        } else if (patch->durationHard > 0) {
            duration = patch->durationHard;
        } else if (patch->durationType == CHAOS_DURATION_STARS) {
            if (patch->effectType == CHAOS_EFFECT_POSITIVE) {
                duration += IMPOSSIBLE_DURATION_DEFAULT_OFFSET_POSITIVE;
            } else if (patch->effectType == CHAOS_EFFECT_NEGATIVE) {
                duration += IMPOSSIBLE_DURATION_DEFAULT_OFFSET_NEGATIVE;
            }
        }
    }

    if (patch->durationType == CHAOS_DURATION_STARS && (
                (patch->effectType == CHAOS_EFFECT_POSITIVE && chaos_check_if_patch_active(CHAOS_PATCH_POSITIVE_EXTENSION)) ||
                (patch->effectType == CHAOS_EFFECT_NEGATIVE && chaos_check_if_patch_active(CHAOS_PATCH_NEGATIVE_EXTENSION))
    )) {
        duration += (duration + 1) / 2;
    }

    // Override
    if (gChaosForcedDurationMaximum > 0 && ((s32) gChaosForcedDurationMaximum) < duration) {
        duration = gChaosForcedDurationMaximum;
    }

    if (duration <= 0) {
        duration = 1;
    }

    if (gChaosCancelOutLostDuration && patch->affectsPatchSelect) {
        duration++;
    }

    // (duration == __UINT8_MAX__) not currently used for anything, but just in case...
    if (duration > (__UINT8_MAX__ - 1)) {
        duration = (__UINT8_MAX__ - 1);
    }

    return (u32) duration;
}

// NOTE: Do not attempt to remove entries inside of deactivation functions, even for separate patches!
void chaos_remove_expired_entry(const s32 patchIndex, const char *msg) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    const enum ChaosPatchID patchId = gChaosActiveEntries[patchIndex].id;
    const struct ChaosPatch *patch = &gChaosPatches[patchId];

    if (patchIndex >= *gChaosActiveEntryCount || patchIndex < 0) {
        // May as well make these aggressions instead of asserts, since they'll infiniloop in practice anyway if we don't.
        aggress(*gChaosActiveEntryCount > 0, "chaos_remove_expired_entry:\nNo patches are currently active!");
        aggress(FALSE, "chaos_remove_expired_entry:\nPatch index out of range!");

        return;
    }

    // We CANNOT safely deactivate other patches within deactivation functions because of stale references in the chaos engine when decrementing star timers and such.
    // (There is a recursive assertion check in place to watch out for this, even though this function on its own is actually safe to call recursively).
    assert_args(unsafeDeactivationFunc == FALSE, "chaos_remove_expired_entry:\nNot safe to invoke recursively or from inside non-activation callbacks:\n0x%08X", patchIndex);
    unsafeDeactivationFunc = TRUE;

    gSaveFileModified = TRUE;

    // Decrease active patch counts for patch.
    assert(activePatchCounts[patchId] > 0, "chaos_remove_expired_entry:\nactivePatchCounts mismatch!");
    if (activePatchCounts[patchId] > 0) {
        activePatchCounts[patchId]--;
    }

    // Decrease number of active patches.
    (*gChaosActiveEntryCount)--;

    // Shift all patches following the deactivated patch left one, overwriting the slot with the deactivated patch.
    for (s32 i = patchIndex; i < *gChaosActiveEntryCount; i++) {
        gChaosActiveEntries[i] = gChaosActiveEntries[i+1];
    }

    // Print chaos message, if it exists
    if (msg) {
        chaosmsg_print(patchId, msg);
    }

    // Invoke deactivation function, now that the entry has been fully removed
    if (patch->deactivationFunc) {
        patch->deactivationFunc();
    }

    unsafeDeactivationFunc = FALSE;
}

// NOTE: This is the safer alternative for use inside callbacks and for USE_COUNT patches
void chaos_remove_expired_entry_deferred(const enum ChaosPatchID patchId, const char *msg) {
    if (!gChaosActiveEntryCount) {
        return;
    }
    
    assert(deferredPatchCount < ARRAY_COUNT(deferredPatchesForRemoval), "chaos_remove_expired_entry_deferred:\nOut of defer slots!")
    if (deferredPatchCount < ARRAY_COUNT(deferredPatchesForRemoval)) {
        deferredPatchesForRemoval[deferredPatchCount++] = patchId;

        // Print chaos message, if it exists
        if (msg) {
            chaosmsg_print(patchId, msg);
        }
    }
}

void chaos_add_new_entry(const enum ChaosPatchID patchId) {
    if (!gChaosActiveEntryCount) {
        return;
    }
    
    if(patchId != CHAOS_PATCH_NONE_POSITIVE && patchId != CHAOS_PATCH_NONE_NEGATIVE) {
        save_file_update_total_patches(patchId);
    }

    const struct ChaosPatch *patch = &gChaosPatches[patchId];
    s32 matchingIndex = -1;

    gSaveFileModified = TRUE;

    // If an active patch is negatable, remove that instead of adding a new one.
    // This is the means of combatting theoretically infinite memory and save file requirements.
    if (patch->negationId && (patch->durationType == CHAOS_DURATION_INFINITE || patch->durationType == CHAOS_DURATION_ONCE)) {
        for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
            if (gChaosActiveEntries[i].id != patch->negationId) {
                continue;
            }

            // Create message string
            s32 size;
            if (patch->effectType == CHAOS_EFFECT_POSITIVE) {
                size = sprintf(gChaosInternalBuffer, "%%s: Cancels out with @05DF15--%s@--------", patch->name);
            } else if (patch->effectType == CHAOS_EFFECT_NEGATIVE) {
                size = sprintf(gChaosInternalBuffer, "%%s: Cancels out with @FF1525--%s@--------", patch->name);
            } else {
                size = sprintf(gChaosInternalBuffer, "%%s: Cancels out with @9F9F9F--%s@--------", patch->name);
            }
            assert_args(size < ARRAY_COUNT(gChaosInternalBuffer), "chaos_add_new_entry:\nString too long:\n 0x%08X", size);
            (void) size; // Remove compiler warning

            // Deactivate negated action, and also instantly activate and deactivate the new function (deactivate will not ever execute for ONCE entries that aren't stackable).
            // This is not added to the patch array (in general do not rely on this for stackable, negatable infinite, or negatable once patches!)
            chaos_remove_expired_entry(i--, gChaosInternalBuffer);

            gChaosImmediateActDeact = TRUE;
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc();
            }
            if (patch->deactivationFunc) {
                patch->deactivationFunc();
            }
            gChaosImmediateActDeact = FALSE;
            return;
        }
    }

    // Find a pre-existing patch entry, if it exists
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == patchId) {
            matchingIndex = i;
            break;
        }
    }

    if (patch->isStackable) {
        if (patch->durationType == CHAOS_DURATION_ONCE) {
            // Activate init func, deactivate, and return immediately.
            // This is not added to the patch array (in general do not rely on this for stackable, negatable infinite, or negatable once patches!)
            assert_args(patch->activatedInitFunc || patch->deactivationFunc || patch->hasMenuEvent
                        || (patchId == CHAOS_PATCH_NONE_POSITIVE || patchId == CHAOS_PATCH_NONE_NEGATIVE),
                        "%s%08X", "chaos_add_new_entry\nAttempted to add stackable ONCE patch\nwithout a callback: 0x", patchId);

            gChaosImmediateActDeact = TRUE;
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc();
            }

            // Activate non-persistent patch event, if applicable
            chaos_menuevent_populate_nonpersistent_patch_event(patchId);

            if (patch->deactivationFunc) {
                patch->deactivationFunc();
            }
            gChaosImmediateActDeact = FALSE;
            return;
        }

        if (patch->durationType == CHAOS_DURATION_USE_COUNT && matchingIndex >= 0) {
            s32 duration = chaos_calculate_patch_duration(patch);

            // Invoke activation function, add duration to existing patch, and return early
            gChaosActiveEntries[matchingIndex].remainingDuration += duration;
            if (gChaosActiveEntries[matchingIndex].remainingDuration < duration) {
                // Overflow detected!
                gChaosActiveEntries[matchingIndex].remainingDuration = -1;
            }
            // This is not added to the patch array (in general do not rely on this for stackable, negatable infinite, or negatable once patches!)
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc();
            }
            return;
        }
    } else if (matchingIndex >= 0) {
        assert_args(FALSE, "%s%08X", "chaos_add_new_entry:\nTried to add duplicate of non-stackable patch: 0x", patchId);
        return;
    }

    if (*gChaosActiveEntryCount >= CHAOS_PATCH_ENTRIES) {
        assert(FALSE, "chaos_add_new_entry:\nMaximum patch count exceeded!");
        return;
    }

    // Allocate new patch entry
    struct ChaosActiveEntry *newEntry = &gChaosActiveEntries[*gChaosActiveEntryCount];

    // Set values for new entry appropriately
    if (patch->durationType == CHAOS_DURATION_ONCE || patch->durationType == CHAOS_DURATION_INFINITE) {
        newEntry->remainingDuration = 0;
    } else {
        newEntry->remainingDuration = chaos_calculate_patch_duration(patch);
        assert_args(newEntry->remainingDuration > 0, "%s%08X", "chaos_add_new_entry:\nDuration-type patch contains duration of 0: 0x", patchId);
    }

    // Check patch duration first, before adding these properties (CHAOS_PATCH_POSITIVE_EXTENSION and CHAOS_PATCH_NEGATIVE_EXTENSION rely on this!)
    newEntry->id = patchId;
    newEntry->frameTimer = 0;
    (*gChaosActiveEntryCount)++;
    activePatchCounts[patchId]++;

    // Activate init func for new entry
    if (patch->activatedInitFunc) {
        patch->activatedInitFunc();
    }
    assert(patch->deactivationFunc == NULL || patch->durationType != CHAOS_DURATION_ONCE, "chaos_add_new_entry:\nDeactivation func will never run for CHAOS_DURATION_ONCE entries!\nRemove this assert if this becomes desirable.");
    
    // NOTE: After invoking the activation callback, it is no longer safe to reference newEntry from this point on, unless we search for it again manually! (This doesn't functionally do anything; it just serves as a future safeguard.)
    newEntry = NULL;

    // Sort patch order
    chaos_sort_active_patches();

    save_file_update_most_active();
}

void chaos_decrement_star_timers(enum ChaosStarDecrementType decrementType) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    gSaveFileModified = TRUE;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const struct ChaosPatch *patch = &gChaosPatches[entry->id];
        if (patch->durationType != CHAOS_DURATION_STARS) {
            continue;
        }
        if (patch->hasMenuEvent) {
            continue;
        }
        if (decrementType == CHAOS_STAR_DECREMENT_STANDARD && patch->affectsPatchSelect) {
            // Skip patches that affect chaos patch generation / patch select menu
            continue;
        }
        if (decrementType == CHAOS_STAR_DECREMENT_MENU_IMPACTING && !patch->affectsPatchSelect) {
            // Skip patches that do not affect chaos patch generation / patch select menu
            continue;
        }

        assert_args(entry->remainingDuration > 0, "%s%08X", "chaos_decrement_star_timers:\nRemaining duration for patch is 0: ", entry->id);
        entry->remainingDuration--;
        if (entry->remainingDuration <= 0) {
            chaos_remove_expired_entry(i--, "%s: Expired!");
        }
    }
    
    // Sorting is needed because not every single star timer decrements here.
    chaos_sort_active_patches();
}

// Note: This sorts patches! Do not use a patch reference here!
void chaos_decrement_star_or_use_timer_with_id(enum ChaosPatchID patchId) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    if (patchId == CHAOS_PATCH_NONE) {
        assert(FALSE, "chaos_decrement_star_or_use_timer_with_id:\npatchId should not be CHAOS_PATCH_NONE!");
        return;
    }

    struct ChaosActiveEntry *entry;
    const struct ChaosPatch *patch = &gChaosPatches[patchId];
    s32 index = chaos_find_first_active_patch(patchId, &entry);
    if (index < 0) {
        // Make sure this is a non-persistent event type
        assert_args(patch->durationType == CHAOS_DURATION_ONCE && patch->isStackable, "chaos_decrement_star_timer_with_id:\nCould not locate patch ID:\n0x%08X", patchId);
        return;
    }

    if (patch->durationType == CHAOS_DURATION_USE_COUNT) {
        // Decrement use count for use count patch type
        chaos_decrement_patch_usage(patchId);
        return;
    }

    if (patch->durationType != CHAOS_DURATION_STARS) {
        // Do nothing for ONCE and INFINITE patches
        return;
    }

    assert_args(entry->remainingDuration > 0, "%s%08X", "chaos_decrement_star_timers:\nRemaining duration for patch is 0: ", patchId);
    entry->remainingDuration--;
    if (entry->remainingDuration <= 0) {
        chaos_remove_expired_entry(index, "%s: Expired!");
    }

    chaos_sort_active_patches();
    gSaveFileModified = TRUE;
}

// Note: This sorts patches! Do not use a patch reference here!
void chaos_decrement_patch_usage(const enum ChaosPatchID patchId) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    struct ChaosActiveEntry *firstFoundMatch = NULL;

    gSaveFileModified = TRUE;

    if (gChaosPatches[patchId].durationType != CHAOS_DURATION_USE_COUNT) {
        assert_args(FALSE, "%s%08X", "chaos_decrement_patch_usage:\nIncorrect patch type: 0x", patchId);
        return;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == patchId) {
            firstFoundMatch = &gChaosActiveEntries[i];
            break;
        }
    }

    if (!firstFoundMatch) {
        assert_args(FALSE, "%s%08X", "chaos_decrement_patch_usage:\nPatch not currently active: 0x", patchId);
        return;
    }

    assert_args(firstFoundMatch->remainingDuration > 0, "%s%08X", "chaos_decrement_patch_usage:\nRemaining duration for patch is 0: ", patchId);
    if (firstFoundMatch->remainingDuration > 0) {
        firstFoundMatch->remainingDuration--;
    }


    // Create message string
    s32 size;
    if (firstFoundMatch->remainingDuration == 0) {
        size = sprintf(gChaosInternalBuffer, "%%s: @FFFF00--No more@-------- uses remaining!");
    } else if (firstFoundMatch->remainingDuration == 1) {
        size = sprintf(gChaosInternalBuffer, "%%s: @FFFF00--%d@-------- use remaining!", firstFoundMatch->remainingDuration);
    } else {
        size = sprintf(gChaosInternalBuffer, "%%s: @FFFF00--%d@-------- uses remaining!", firstFoundMatch->remainingDuration);
    }
    assert_args(size < ARRAY_COUNT(gChaosInternalBuffer), "chaos_decrement_patch_usage:\nString too long:\n 0x%08X", size);
    (void) size; // Remove compiler warning

    chaosmsg_print(patchId, gChaosInternalBuffer);

    if (firstFoundMatch->remainingDuration <= 0) {
        chaos_remove_expired_entry_deferred(patchId, NULL);
    }

    chaos_sort_active_patches();
}

// Update a complete list of patches that are acceptible for generation
static void chaos_update_available_patches(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    for (s32 i = 0; i < ARRAY_COUNT(availablePatches); i++) {
        availablePatches[i] = TRUE;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const struct ChaosPatch *patch = &gChaosPatches[entry->id];

        if (!patch->isStackable) {
            availablePatches[entry->id] = FALSE;
            continue;
        }
    }

    for (u32 i = 0; i < CHAOS_PATCH_COUNT; i++) {
        const struct ChaosPatch *patch = &gChaosPatches[i];

        if (!chaos_check_conditional_func(patch)) {
            availablePatches[i] = FALSE;
            continue;
        }
    }
}

static void chaos_generate_patches(u8 severityCounts[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT],
                          u8 posNegPairings[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT], f32 severityWeights[CHAOS_PATCH_SEVERITY_COUNT]) {
    bzero(generatedPatches, sizeof(generatedPatches));

    // Generate patches
    for (s32 index = 0; index < CHAOS_PATCH_MAX_GENERATABLE; index++) {
        enum ChaosPatchID positivePatchId = CHAOS_PATCH_NONE_POSITIVE;
        enum ChaosPatchID negativePatchId = CHAOS_PATCH_NONE_NEGATIVE;
        s32 attempts;

        f32 totalWeight = 0.0f;
        s32 generatedSeverity = 0;
        for (s32 i = 1; i < CHAOS_PATCH_SEVERITY_COUNT; i++) {
            totalWeight += severityWeights[i];
        }

        if (totalWeight > 0.0f) {
            f32 rand = random_float() * totalWeight;
            for (s32 i = 1; i < CHAOS_PATCH_SEVERITY_COUNT; i++) {
                if (rand < severityWeights[i]) {
                    generatedSeverity = i;
                    break;
                }

                rand -= severityWeights[i];
            }
        }

        s32 negativeSeverity = posNegPairings[generatedSeverity][CHAOS_EFFECT_NEGATIVE];
        s32 positiveSeverity = posNegPairings[generatedSeverity][CHAOS_EFFECT_POSITIVE];

        for (attempts = 0; attempts < RETRY_ATTEMPTS_DUPLICATES; attempts++) {
            s32 negativeWeight = (s32) (random_float() * (f32) severityCounts[negativeSeverity][CHAOS_EFFECT_NEGATIVE]);

            for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
                const struct ChaosPatch *patch = &gChaosPatches[patchId];
                if (!availablePatches[patchId] || patch->effectType != CHAOS_EFFECT_NEGATIVE || patch->severity != negativeSeverity) {
                    continue;
                }

                if (negativeWeight <= 0) {
                    negativePatchId = patchId;
                    break;
                }

                negativeWeight--;
            }

            // Drastically reduce likelihood of repeat negative cards from showing up
            s32 foundRepeatGenerated = FALSE;
            for (s32 previousGenIndex = index - 1; previousGenIndex >= 0; previousGenIndex--) {
                if (generatedPatches[previousGenIndex].negativeId == negativePatchId) {
                    foundRepeatGenerated = TRUE;
                    break;
                }
            }

            if (foundRepeatGenerated) {
                continue;
            }

            // Make duplicates of stackable patches less likely
            s32 patchDuplicates = chaos_count_active_instances(negativePatchId);
            f32 retryChance = 1.0f;
            f32 duplicateAllowance = DUPLICATE_ALLOWANCE;
            if (gChaosPatches[negativePatchId].negationId) {
                // Double chances of passing allowance when patch negation is possible
                duplicateAllowance += (1.0f - duplicateAllowance) / 2.0f;
            }
            for (s32 i = 0; i < patchDuplicates; i++) {
                retryChance *= duplicateAllowance;
            }
            if (random_float() >= retryChance) {
                continue;
            }

            break;
        }
        if (attempts >= RETRY_ATTEMPTS_DUPLICATES && negativeSeverity > 0) {
            chaosmsg_print_debug("@AFAFAF--Negative attempt tries exceeded!@--------");
        }

        gChaosNegativePatchCompare = negativePatchId;

        s32 applicablePositiveCount = 0;
        for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
            const struct ChaosPatch *patch = &gChaosPatches[patchId];
            if (!availablePatches[patchId] || patch->effectType != CHAOS_EFFECT_POSITIVE || patch->severity != positiveSeverity) {
                continue;
            }
            if (gChaosPatches[negativePatchId].negationId && gChaosPatches[negativePatchId].negationId == patchId) {
                continue;
            }
            if (!chaos_check_conditional_func(patch)) {
                continue;
            }

            applicablePositiveCount++;
        }

        if (applicablePositiveCount > 0) {
            for (attempts = 0; attempts < RETRY_ATTEMPTS_DUPLICATES; attempts++) {
                s32 positiveWeight = (s32) (random_float() * applicablePositiveCount);

                for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
                    const struct ChaosPatch *patch = &gChaosPatches[patchId];
                    if (!availablePatches[patchId] || patch->effectType != CHAOS_EFFECT_POSITIVE || patch->severity != positiveSeverity) {
                        continue;
                    }
                    if (gChaosPatches[negativePatchId].negationId && gChaosPatches[negativePatchId].negationId == patchId) {
                        continue;
                    }
                    if (!chaos_check_conditional_func(patch)) {
                        continue;
                    }

                    if (positiveWeight <= 0) {
                        positivePatchId = patchId;
                        break;
                    }

                    positiveWeight--;
                }

                // Drastically reduce likelihood of repeat positive cards from showing up
                s32 foundRepeatGenerated = FALSE;
                for (s32 previousGenIndex = index - 1; previousGenIndex >= 0; previousGenIndex--) {
                    if (generatedPatches[previousGenIndex].positiveId == positivePatchId) {
                        foundRepeatGenerated = TRUE;
                        break;
                    }
                }

                if (foundRepeatGenerated) {
                    continue;
                }

                // Make duplicates of stackable patches less likely
                s32 patchDuplicates = chaos_count_active_instances(positivePatchId);
                f32 retryChance = 1.0f;
                f32 duplicateAllowance = DUPLICATE_ALLOWANCE;
                if (gChaosPatches[positivePatchId].negationId) {
                    // Double chances of passing allowance when patch negation is possible
                    duplicateAllowance += (1.0f - duplicateAllowance) / 2.0f;
                }
                for (s32 i = 0; i < patchDuplicates; i++) {
                    retryChance *= duplicateAllowance;
                }
                if (random_float() >= retryChance) {
                    continue;
                }

                break;
            }
            if (attempts >= RETRY_ATTEMPTS_DUPLICATES && positiveSeverity > 0) {
                chaosmsg_print_debug("@AFAFAF--Positive attempt tries exceeded!@--------");
            }
        }

        gChaosNegativePatchCompare = CHAOS_PATCH_NONE;

#ifdef CHAOS_FORCED_POSITIVE_CARD
        if (index == 0
         && (!chaos_check_if_patch_active(CHAOS_FORCED_POSITIVE_CARD) || gChaosPatches[CHAOS_FORCED_POSITIVE_CARD].isStackable)
         && chaos_check_conditional_func(&gChaosPatches[CHAOS_FORCED_POSITIVE_CARD])
        ) {
            positivePatchId = CHAOS_FORCED_POSITIVE_CARD;
            negativePatchId = CHAOS_PATCH_NONE_NEGATIVE;
            generatedSeverity = 0;
        }
#endif
#ifdef CHAOS_FORCED_NEGATIVE_CARD
        if (index == 1
         && (!chaos_check_if_patch_active(CHAOS_FORCED_NEGATIVE_CARD) || gChaosPatches[CHAOS_FORCED_NEGATIVE_CARD].isStackable)
         && chaos_check_conditional_func(&gChaosPatches[CHAOS_FORCED_NEGATIVE_CARD])
        ) {
            positivePatchId = CHAOS_PATCH_NONE_POSITIVE;
            negativePatchId = CHAOS_FORCED_NEGATIVE_CARD;
            generatedSeverity = 0;
        }
#endif

        generatedPatches[index].positiveId = positivePatchId;
        generatedPatches[index].negativeId = negativePatchId;
        generatedPatches[index].positivePatch = &gChaosPatches[positivePatchId];
        generatedPatches[index].negativePatch = &gChaosPatches[negativePatchId];
        generatedPatches[index].severityLevel = generatedSeverity;
    }
}

struct ChaosPatchSelection *chaos_roll_for_new_patches(s32 forcedSeverityOverride, enum ChaosPatchSpecialEvent forcedEventOverride) {
    if (!gChaosActiveEntryCount) {
        return NULL;
    }

    s32 forcedSeverity = -1;
    f32 offsetSeverityWeight;
    f32 generatedDifficultyWeight;
    s32 starCount = save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1);
    enum ChaosPatchSpecialEvent specialEvent = CHAOS_SPECIAL_NONE;
    u8 severityCounts[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT];
    u8 posNegPairings[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT];
    s8 allowedSeverities[CHAOS_PATCH_SEVERITY_COUNT];
    f32 severityWeights[CHAOS_PATCH_SEVERITY_COUNT];

    bzero(severityCounts, sizeof(severityCounts));
    bzero(posNegPairings, sizeof(posNegPairings));
    bzero(allowedSeverities, sizeof(allowedSeverities));
    bzero(severityWeights, sizeof(severityWeights));

    // Update RNG based on value in the save file to prevent reset cheese; effectively, these rolls are predetermined based on the last collected star!
    // Only exception here is the very first star, as otherwise that would too often return the exact same patch cards.
    u16 activeSeed = get_active_rng_seed();
    u16 savedSeed = save_file_get_rng_seed();
#ifndef DEBUG_PATCH_SELECT_MENU
    if (save_file_get_flags() & SAVE_FLAG_SET_RNG_SEED) {
        // Except only actually enforce the RNG decheese on at least Impossible difficulty, in challenge (and at least hard mode), or in hardcore mode.
        // This assumes most players are here to have fun, and less so to accomplish a painstaking challenge.
        if (gChaosGameMode == CHAOS_GAMEMODE_HARDCORE || (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE && gChaosDifficulty >= CHAOS_DIFFICULTY_HARD) || gChaosDifficulty >= CHAOS_DIFFICULTY_IMPOSSIBLE) {
            set_active_rng_seed(savedSeed);
        }
    }
#endif
    save_file_update_rng_seed(activeSeed ^ savedSeed);

    chaos_update_available_patches();

    // Determine how many of a patch currently exist
    for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (!availablePatches[patchId]) {
            continue;
        }

        severityCounts[patch->severity][patch->effectType]++;
        if (severityCounts[patch->severity][patch->effectType] == 0) {
            severityCounts[patch->severity][patch->effectType] = -1; // Overflow
        }
    }

    // Lessen likelihood of repeat severity randomization types in a row via attempts
    if (forcedSeverityOverride >= -1) {
        forcedSeverity = forcedSeverityOverride;
        if (forcedSeverityOverride >= CHAOS_PATCH_SEVERITY_COUNT) {
            assert_args(FALSE, "chaos_roll_for_new_patches:\nBad forced severity override: %d", forcedSeverityOverride);
            forcedSeverity = -1;
        }
        // Skip update for gChaosLastForcedSeverity here so future generation isn't influenced by override
    } else if (starCount < CHAOS_MIN_STARS_FOR_FORCED_DIFFICULTIES) {
        forcedSeverity = -1;
        gChaosLastForcedSeverity = -2;
    } else {
        for (s32 attempts = 0; attempts < 2; attempts++) {
            f32 weight = 0.0f;
            forcedSeverity = -1;
            generatedDifficultyWeight = random_float();
            for (s32 i = 0; i < ARRAY_COUNT(difficultyWeights[0]); i++) {
                if (!((i + 1) == 1 && chaos_check_if_patch_active(CHAOS_PATCH_HIGH_STAKES))
                            && !((i + 1) == CHAOS_PATCH_SEVERITY_MAX && chaos_check_if_patch_active(CHAOS_PATCH_LOW_STAKES))) {
                    weight += difficultyWeights[gChaosDifficulty][i];
                }
                if (generatedDifficultyWeight < weight) {
                    forcedSeverity = i + 1;
                    break;
                }
            }

            // Check if repeat or should break early.
            // 33% chance to allow generated repeat here before retrying severity generation.
            if (forcedSeverity != gChaosLastForcedSeverity || random_float() < 0.33f) {
                break;
            }
        }
        gChaosLastForcedSeverity = forcedSeverity;
    }

    offsetSeverityWeight = random_float();
    if (starCount < CHAOS_MIN_STARS_FOR_EVENTS) {
        offsetSeverityWeight = 100.0f;
    }

    chaosmsg_print_debug("@FFFF009FforcedSeverity: "
                         "@FF3F3F9F%d",
                         forcedSeverity);

    if (forcedEventOverride != CHAOS_SPECIAL_DO_NOT_FORCE) {
        specialEvent = forcedEventOverride;
        if (forcedEventOverride < CHAOS_SPECIAL_NONE || forcedEventOverride >= CHAOS_SPECIAL_COUNT) {
            assert_args(FALSE, "chaos_roll_for_new_patches:\nBad forced event override: %d", forcedEventOverride);
            specialEvent = CHAOS_SPECIAL_NONE;
        }
        // Skip update for gChaosLastEventType here so future generation isn't influenced by override

#ifdef CHAOS_ENGINE_DEBUG
        switch (specialEvent) {
            case CHAOS_SPECIAL_NONE:
            default:
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9FN/A (forced)");
                break;
            case CHAOS_SPECIAL_PLUS1_POSITIVE:
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fpos++ (forced)");
                break;
            case CHAOS_SPECIAL_PLUS1_NEGATIVE:
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fneg++ (forced)");
                break;
            case CHAOS_SPECIAL_ZERO_POSITIVE:
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fpos 0 (forced)");
                break;
        }
#endif
    } else if (gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE) {
        specialEvent = CHAOS_SPECIAL_NONE;
        gChaosLastEventType = CHAOS_SPECIAL_NONE;
        chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9FN/A (impossible)");
    } else {
        f32 luckyEventFactor = EVENT_ODDS_LUCKY;
        f32 unluckyEventFactor = EVENT_ODDS_UNLUCKY;
        f32 chaosEventFactor = EVENT_ODDS_CHAOS;
        if (chaos_check_if_patch_active(CHAOS_PATCH_LUCKY_CHARM)) {
            luckyEventFactor *= 2.0f;
        }
        if (chaos_check_if_patch_active(CHAOS_PATCH_UNLUCKY_CHARM)) {
            unluckyEventFactor *= 2.0f;
            chaosEventFactor *= 2.0f;
        }
        if (chaos_check_if_patch_active(CHAOS_PATCH_UNEVENTFUL)) {
            unluckyEventFactor = 0.0f;
            chaosEventFactor = 0.0f;
        }
        unluckyEventFactor += luckyEventFactor;
        chaosEventFactor += unluckyEventFactor;

        if (offsetSeverityWeight < luckyEventFactor) {
            // 17.5% chance to globally increase positive severity
            specialEvent = CHAOS_SPECIAL_PLUS1_POSITIVE;
            chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fpos++");
        } else if (offsetSeverityWeight < unluckyEventFactor) {
            // 17.5% chance to globally increase negative severity
            specialEvent = CHAOS_SPECIAL_PLUS1_NEGATIVE;
            chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fneg++");
        } else if (offsetSeverityWeight < chaosEventFactor) {
            // 12.5% chance to eliminate all positive patches
            specialEvent = CHAOS_SPECIAL_ZERO_POSITIVE;
            chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fpos 0");
        } else {
            specialEvent = CHAOS_SPECIAL_NONE;
            chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9FN/A");
        }

        // No repeat event types in a row
        if (gChaosLastEventType == specialEvent && specialEvent != CHAOS_SPECIAL_NONE) {
            specialEvent = CHAOS_SPECIAL_NONE;
            chaosmsg_print_debug("@FFFF009FDuplicate event detected, overruled to N/A!");
        }
        gChaosLastEventType = specialEvent; // Repeat stars also have no impact on star history for this
    }

    // Determine available severity combinations that may be used for selections
    for (s32 i = 1; i < ARRAY_COUNT(posNegPairings); i++) {
        s32 pos = i;
        s32 neg = i;

        for (s32 j = 0; j < ARRAY_COUNT(posNegPairings[i]); j++) {
            posNegPairings[i][j] = 0;
        }
        allowedSeverities[i] = FALSE;

        if (i == 1 && chaos_check_if_patch_active(CHAOS_PATCH_HIGH_STAKES)) {
            continue;
        }
        if (i == CHAOS_PATCH_SEVERITY_MAX && chaos_check_if_patch_active(CHAOS_PATCH_LOW_STAKES)) {
            continue;
        }

        if (forcedSeverity >= 0) {
            pos = forcedSeverity;
            neg = forcedSeverity;
        }

        switch (specialEvent) {
            case CHAOS_SPECIAL_PLUS1_NEGATIVE:
                neg++;
                break;
            case CHAOS_SPECIAL_PLUS1_POSITIVE:
                pos++;
                break;
            case CHAOS_SPECIAL_ZERO_POSITIVE:
                pos = 0;
                break;
            default:
                break;
        }

        if (!gChaosSkipSeverityDifficultyModifier) {
            if (gChaosDifficulty == CHAOS_DIFFICULTY_EASY) {
                neg--;
            } else if (gChaosDifficulty == CHAOS_DIFFICULTY_HARD) {
                pos--;
            } else if (gChaosDifficulty == CHAOS_DIFFICULTY_IMPOSSIBLE) {
                pos = 0;
            }
        }

        if (pos > CHAOS_PATCH_SEVERITY_MAX) {
            pos = CHAOS_PATCH_SEVERITY_MAX;
        }
        if (pos < 0) {
            pos = 0;
        }
        if (neg > CHAOS_PATCH_SEVERITY_MAX) {
            neg = CHAOS_PATCH_SEVERITY_MAX;
        }
        if (neg < 0) {
            neg = 0;
        }

        if (severityCounts[pos][CHAOS_EFFECT_POSITIVE] > 0 && severityCounts[neg][CHAOS_EFFECT_NEGATIVE] > 0) {
            posNegPairings[i][CHAOS_EFFECT_POSITIVE] = pos;
            posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = neg;
            allowedSeverities[i] = TRUE;
            continue;
        }

        if (severityCounts[pos][CHAOS_EFFECT_POSITIVE] == 0 && severityCounts[neg][CHAOS_EFFECT_NEGATIVE] == 0) {
            continue;
        }

        if (severityCounts[pos][CHAOS_EFFECT_POSITIVE] > 0) {
            posNegPairings[i][CHAOS_EFFECT_POSITIVE] = pos;

            if (neg < CHAOS_PATCH_SEVERITY_MAX && severityCounts[neg + 1][CHAOS_EFFECT_NEGATIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = neg + 1;
                allowedSeverities[i] = TRUE;
                continue;
            }

            if (neg > 0 && severityCounts[neg - 1][CHAOS_EFFECT_NEGATIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = neg - 1;
                allowedSeverities[i] = TRUE;
                continue;
            }
        }

        if (severityCounts[neg][CHAOS_EFFECT_NEGATIVE] > 0) {
            posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = neg;

            if (pos > 0 && severityCounts[pos - 1][CHAOS_EFFECT_POSITIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_POSITIVE] = pos - 1;
                allowedSeverities[i] = TRUE;
                continue;
            }

            if (pos < CHAOS_PATCH_SEVERITY_MAX && severityCounts[pos + 1][CHAOS_EFFECT_POSITIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_POSITIVE] = pos + 1;
                allowedSeverities[i] = TRUE;
                continue;
            }
        }
    }

    // Compute weights for generation
    f32 totalWeight = 0.0f;

    // NOTE: We don't actually apply anything to severityWeights right now, so this essentially does nothing (apart from disabled severities)...
    for (s32 i = 1; i < ARRAY_COUNT(severityWeights); i++) {
        severityWeights[i] += WEIGHT_OFFSET;
        totalWeight += severityWeights[i];
    }
    for (s32 i = 1; i < ARRAY_COUNT(severityWeights); i++) {
        severityWeights[i] = 1.0f - (severityWeights[i] / totalWeight);
        severityWeights[i] *= severityWeights[i];
        if (!allowedSeverities[i]) {
            severityWeights[i] = 0.0f;
        }
    }

    chaos_generate_patches(severityCounts, posNegPairings, severityWeights);

    if (forcedSeverity >= 0) {
        for (s32 index = 0; index < CHAOS_PATCH_MAX_GENERATABLE; index++) {
#ifdef CHAOS_FORCED_POSITIVE_CARD
            if (index == 0 && generatedPatches[index].positiveId == CHAOS_FORCED_POSITIVE_CARD && generatedPatches[index].severityLevel == 0) {
                continue;
            }
#endif
#ifdef CHAOS_FORCED_NEGATIVE_CARD
            if (index == 1 && generatedPatches[index].negativeId == CHAOS_FORCED_NEGATIVE_CARD && generatedPatches[index].severityLevel == 0) {
                continue;
            }
#endif
            generatedPatches[index].severityLevel = forcedSeverity;
        }
    }

    s32 overruledSpecialEvent = TRUE;
    for (s32 index = 0; index < CHAOS_PATCH_MAX_GENERATABLE; index++) {
        generatedPatches[index].specialEvent = specialEvent;
        if (specialEvent == CHAOS_SPECIAL_PLUS1_POSITIVE &&
                    gChaosDifficulty != CHAOS_DIFFICULTY_HARD &&
                    generatedPatches[index].severityLevel == CHAOS_PATCH_SEVERITY_MAX
        ) {
            generatedPatches[index].specialEvent = CHAOS_SPECIAL_NONE;
        }

        if (specialEvent == CHAOS_SPECIAL_PLUS1_NEGATIVE &&
                    gChaosDifficulty != CHAOS_DIFFICULTY_EASY &&
                    generatedPatches[index].severityLevel == CHAOS_PATCH_SEVERITY_MAX
        ) {
            generatedPatches[index].specialEvent = CHAOS_SPECIAL_NONE;
        }

        if (specialEvent == CHAOS_SPECIAL_ZERO_POSITIVE &&
                    gChaosDifficulty == CHAOS_DIFFICULTY_HARD &&
                    generatedPatches[index].severityLevel == 1
        ) {
            generatedPatches[index].specialEvent = CHAOS_SPECIAL_NONE;
        }

        if (generatedPatches[index].specialEvent == specialEvent) {
            overruledSpecialEvent = FALSE;
        }
    }
    if (overruledSpecialEvent) {
        chaosmsg_print_debug("@FFFF009FAll generated patch events overridden to no event, discounting event...");
        gChaosLastEventType = CHAOS_SPECIAL_NONE;
    }

    save_file_set_new_chaos_gen_data(gChaosLastForcedSeverity, gChaosLastEventType);

    return generatedPatches;
}

void chaos_select_patches(struct ChaosPatchSelection *patchSelection) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    chaos_add_new_entry(patchSelection->positiveId);
    chaos_add_new_entry(patchSelection->negativeId);
}

void chaos_init(void) {
    save_file_get_chaos_data(&gChaosActiveEntries, &gChaosActiveEntryCount, &gChaosDifficulty, &gChaosGameMode, &gChaosLastForcedSeverity, &gChaosLastEventType);
    chaos_recompute_active_patch_counts();

    deferredPatchCount = 0;
    gChaosLevelWarped = FALSE;
    gChaosBlueStarLastCollected = FALSE;
    gChaosForcedDurationType = CHAOS_DURATION_DO_NOT_FORCE;
    gChaosForcedDurationMaximum = 0;
    bzero(gChaosEventQueue, sizeof(gChaosEventQueue));

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->durationType == CHAOS_DURATION_ONCE) {
            continue;
        }

        if (patch->activatedInitFunc) {
            gChaosActiveEntries[i].frameTimer = 0; // Is this desirable?
            patch->activatedInitFunc();
        }
    }

    chaos_sort_active_patches();
    chaosmsg_init();
    chaos_menuevent_init();
}

void chaos_area_update(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    if (gCurrCourseNum == COURSE_NONE || gCurrCourseNum == COURSE_CAKE_END) {
        return;
    }

    if (sWarpDest.type == WARP_TYPE_SAME_AREA) {
        return;
    }

    unsafeDeactivationFunc = TRUE;
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (gChaosLevelWarped && patch->levelInitFunc) {
            patch->levelInitFunc();
        }
        if (patch->areaInitFunc) {
            patch->areaInitFunc();
        }
    }
    unsafeDeactivationFunc = FALSE;

    gChaosLevelWarped = FALSE;
}

void chaos_instant_warp_area_update(struct InstantWarp *warp, s32 isPostWarp) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    if (gCurrCourseNum == COURSE_NONE || gCurrCourseNum == COURSE_CAKE_END) {
        return;
    }

    if (sWarpDest.type == WARP_TYPE_SAME_AREA) {
        return;
    }

    unsafeDeactivationFunc = TRUE;
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (isPostWarp) {
            if (patch->instWarpPostFunc) {
                patch->instWarpPostFunc(warp);
            }
        } else {
            if (patch->instWarpPreFunc) {
                patch->instWarpPreFunc(warp);
            }
        }
    }
    unsafeDeactivationFunc = FALSE;
}

void chaos_frame_update(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    if (gTimeStopState & TIME_STOP_ACTIVE) {
        return;
    }

    unsafeDeactivationFunc = TRUE;
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->frameUpdateFunc) {
            patch->frameUpdateFunc();
        }
        if (gChaosActiveEntries[i].frameTimer < 0xFFFFFF) {
            gChaosActiveEntries[i].frameTimer++;
        }
    }
    unsafeDeactivationFunc = FALSE;
}

void chaos_remove_deferred_patches(void) {
    if (!gChaosActiveEntryCount || deferredPatchCount == 0) {
        return;
    }
    
    // Prioritize should-be expiring patches with 0 duration, just in case they're otherwise missed somehow
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].remainingDuration != 0) {
            continue;
        }

        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];
        if (patch->durationType != CHAOS_DURATION_STARS && patch->durationType != CHAOS_DURATION_USE_COUNT) {
            continue;
        }

        for (u32 entry = 0; entry < deferredPatchCount; entry++) {
            if (patchId != deferredPatchesForRemoval[entry] || patchId == CHAOS_PATCH_NONE) {
                continue;
            }

            chaos_remove_expired_entry(i--, NULL);
            deferredPatchesForRemoval[entry] = CHAOS_PATCH_NONE;
        }
    }

    // Remove everything else
    for (u32 entry = 0; entry < deferredPatchCount; entry++) {
        const enum ChaosPatchID patchId = deferredPatchesForRemoval[entry];
        if (patchId == CHAOS_PATCH_NONE) {
            continue;
        }

        // Remove first found patch (favors lower patch timers for stackable patches)
        for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
            if (gChaosActiveEntries[i].id == patchId) {
                chaos_remove_expired_entry(i--, NULL);
                break;
            }
        }
    }
    
    deferredPatchCount = 0;
}

s32 chaos_precheck_conditional_exclusions(UNUSED s16 arg0, UNUSED s32 arg1) {
#ifdef DEBUG_ASSERTIONS
    for (enum ChaosPatchID currentPatchID = 0; currentPatchID < ARRAY_COUNT(gChaosPatches); currentPatchID++) {
        const struct ChaosPatch *currentPatch = &gChaosPatches[currentPatchID];
        if (currentPatch->incompatible == NULL || currentPatch->incompatibleCount == 0) {
            continue;
        }

        for (s32 incompatiblePatchIndex = 0; incompatiblePatchIndex < currentPatch->incompatibleCount; incompatiblePatchIndex++) {
            const enum ChaosPatchID incompatiblePatchID = currentPatch->incompatible[incompatiblePatchIndex];
            const struct ChaosPatch *incompatiblePatch = &gChaosPatches[incompatiblePatchID];
            u8 found = FALSE;

            if (incompatiblePatch->incompatible != NULL && incompatiblePatch->incompatibleCount > 0) {
                for (s32 matchingPatchIndex = 0; matchingPatchIndex < incompatiblePatch->incompatibleCount; matchingPatchIndex++) {
                    const enum ChaosPatchID matchingPatchID = incompatiblePatch->incompatible[matchingPatchIndex];
                    if (currentPatchID == matchingPatchID) {
                        found = TRUE;
                        break;
                    }
                }
            }

            if (!found && incompatiblePatch->__dbg_exempt != NULL && incompatiblePatch->__dbg_exemptCount > 0) {
                for (s32 matchingPatchIndex = 0; matchingPatchIndex < incompatiblePatch->__dbg_exemptCount; matchingPatchIndex++) {
                    const enum ChaosPatchID matchingPatchID = incompatiblePatch->__dbg_exempt[matchingPatchIndex];
                    if (currentPatchID == matchingPatchID) {
                        found = TRUE;
                        break;
                    }
                }
            }

            assert_args(found, "chaos_precheck_conditional_exclusions:\n" \
                "Exclusion mismatch found!\n" \
                "\n" \
                "Bad Patch:\n" \
                "  %s:\n" \
                "Should exclude:\n" \
                "  %s\n", \
            incompatiblePatch->name, currentPatch->name);
        }
    }
#endif

    return 0;
}
