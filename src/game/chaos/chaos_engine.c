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
#define RETRY_ATTEMPTS_DUPLICATES 3
#define DUPLICATE_ALLOWANCE 0.45f

static u32 activePatchCounts[CHAOS_PATCH_COUNT];
static u8 availablePatches[CHAOS_PATCH_COUNT];
static struct ChaosPatchSelection generatedPatches[CHAOS_PATCH_MAX_GENERATABLE];

char gChaosInternalBuffer[0x1000];

s32 *gChaosActiveEntryCount = NULL;
struct ChaosActiveEntry *gChaosActiveEntries = NULL;
u8 gChaosLevelWarped = FALSE;

static const f32 difficultyWeights[CHAOS_DIFFICULTY_COUNT][CHAOS_PATCH_SEVERITY_COUNT - 1] = {
    [CHAOS_DIFFICULTY_EASY  ] = { 0.12f, 0.25f, 0.37f }, // Difficulty offset should make highest level more common
    [CHAOS_DIFFICULTY_NORMAL] = { 0.25f, 0.25f, 0.25f }, // Difficulty probability is balanced across the board
    [CHAOS_DIFFICULTY_HARD  ] = { 0.15f, 0.25f, 0.35f }, // Difficulty offset doesn't really matter as much for hard, so just make highest level more common
};

static enum ChaosPatchID negativePatchCompare = CHAOS_PATCH_NONE;

// TODO: These are written to by the save file, but are never actually saved to the save file!
// This essentially means that they will always be 0 and are unconfigurable at compile time!
// Implement save file options to allow difficulty selection and lives!
enum ChaosDifficulty gChaosDifficulty = CHAOS_DIFFICULTY_NORMAL;
u8 gChaosLivesEnabled = FALSE;

static void chaos_recompute_active_patch_counts(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    bzero(activePatchCounts, sizeof(activePatchCounts));
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        activePatchCounts[gChaosActiveEntries[i].id]++;
    }
}

u8 chaos_check_if_patch_active(const enum ChaosPatchID patchId) {
    return (activePatchCounts[patchId] > 0 || patchId == negativePatchCompare);
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
static void chaos_sort_active_patches(void) {
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

u8 chaos_find_first_active_patch(const enum ChaosPatchID patchId, struct ChaosActiveEntry **firstFoundMatch) {
    if (!gChaosActiveEntryCount) {
        if (firstFoundMatch) {
            *firstFoundMatch = NULL;
        }

        return FALSE;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == patchId) {
            if (firstFoundMatch) {
                *firstFoundMatch = &gChaosActiveEntries[i];
            }

            return TRUE;
        }
    }

    if (firstFoundMatch) {
        *firstFoundMatch = NULL;
    }
    return FALSE;
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

void chaos_remove_expired_entry(const s32 patchIndex, const char *msg) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    const enum ChaosPatchID patchId = gChaosActiveEntries[patchIndex].id;
    const struct ChaosPatch *patch = &gChaosPatches[patchId];

    gSaveFileModified = TRUE;

    if (patchIndex >= *gChaosActiveEntryCount || patchIndex < 0) {
        // May as well make these aggressions instead of asserts, since they'll infiniloop in practice anyway if we don't.
        aggress(*gChaosActiveEntryCount > 0, "chaos_remove_expired_entry:\nNo patches are currently active!");
        aggress(FALSE, "chaos_remove_expired_entry:\nPatch index out of range!");

        return;
    }

    // Decrease number of active patches and move the last active patch in the array to the index of the removed patch.
    // This means of moving stuff around implies we cannot make use of sorting order!
    (*gChaosActiveEntryCount)--;
    assert(activePatchCounts[patchId] > 0, "chaos_remove_expired_entry:\nactivePatchCounts mismatch!");
    if (activePatchCounts[patchId] > 0) {
        activePatchCounts[patchId]--;
    }
    gChaosActiveEntries[patchIndex] = gChaosActiveEntries[*gChaosActiveEntryCount];

    // Invoke deactivation function, now that the entry has been removed
    if (patch->deactivationFunc) {
        patch->deactivationFunc();
    }

    // Print chaos message, if it exists
    if (msg) {
        chaosmsg_print(patchId, msg);
    }

    // Sort patch order
    chaos_sort_active_patches();
}

void chaos_add_new_entry(const enum ChaosPatchID patchId) {
    if (!gChaosActiveEntryCount) {
        return;
    }
    
    save_file_update_total_patches();

    const struct ChaosPatch *patch = &gChaosPatches[patchId];
    s32 matchingIndex = -1;

    gSaveFileModified = TRUE;

    // If an active patch is negatable, remove that instead of adding a new one.
    // This is the means of combatting theoretically infinite memory and save file requirements.
    if (patch->negationId && patch->durationType == CHAOS_DURATION_INFINITE) {
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

            // Deactivate negated action, and also instantly activate and deactivate the new function (deactivate will not ever execute for ONCE entries that aren't stackable)
            chaos_remove_expired_entry(i--, gChaosInternalBuffer);
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc();
            }
            if (patch->deactivationFunc) {
                patch->deactivationFunc();
            }
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
            // Activate init func, deactivate, and return immediately, do not add to patch array
            assert_args(patch->activatedInitFunc || patch->deactivationFunc
                        || (patchId == CHAOS_PATCH_NONE_POSITIVE || patchId == CHAOS_PATCH_NONE_NEGATIVE),
                        "%s%08X", "chaos_add_new_entry\nAttempted to add stackable ONCE patch\nwithout a callback: 0x", patchId);
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc();
            }
            if (patch->deactivationFunc) {
                patch->deactivationFunc();
            }
            return;
        }

        if (patch->durationType == CHAOS_DURATION_USE_COUNT && matchingIndex >= 0) {
            // Invoke activation function, add duration to existing patch, and return early
            gChaosActiveEntries[matchingIndex].remainingDuration += patch->duration;
            if (gChaosActiveEntries[matchingIndex].remainingDuration < patch->duration) {
                // Overflow detected!
                gChaosActiveEntries[matchingIndex].remainingDuration = -1;
            }
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
    (*gChaosActiveEntryCount)++;
    activePatchCounts[patchId]++;

    // Set values for new entry appropriately
    newEntry->id = patchId;
    newEntry->frameTimer = 0;
    if (patch->durationType == CHAOS_DURATION_ONCE || patch->durationType == CHAOS_DURATION_INFINITE) {
        newEntry->remainingDuration = 0;
    } else {
        newEntry->remainingDuration = patch->duration;
        assert_args(newEntry->remainingDuration > 0, "%s%08X", "chaos_add_new_entry:\nDuration-type patch contains duration of 0: 0x", patchId);
    }

    // Activate init func for new entry
    if (patch->activatedInitFunc) {
        patch->activatedInitFunc();
    }
    assert(patch->deactivationFunc == NULL || patch->durationType != CHAOS_DURATION_ONCE, "chaos_add_new_entry:\nDeactivation func will never run for CHAOS_DURATION_ONCE entries!\nRemove this assert if this becomes desirable.");

    // Sort patch order
    chaos_sort_active_patches();
}

void chaos_decrement_star_timers(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    gSaveFileModified = TRUE;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        if (gChaosPatches[entry->id].durationType != CHAOS_DURATION_STARS) {
            continue;
        }

        assert_args(entry->remainingDuration > 0, "%s%08X", "chaos_decrement_star_timers:\nRemaining duration for patch is 0: ", entry->id);
        entry->remainingDuration--;
        if (entry->remainingDuration <= 0) {
            chaos_remove_expired_entry(i--, "%s: Expired!");
        }
    }
}

void chaos_decrement_patch_usage(const enum ChaosPatchID patchId) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    struct ChaosActiveEntry *firstFoundMatch = NULL;
    s32 matchIndex = -1;

    gSaveFileModified = TRUE;

    if (gChaosPatches[patchId].durationType != CHAOS_DURATION_USE_COUNT) {
        assert_args(FALSE, "%s%08X", "chaos_decrement_patch_usage:\nIncorrect patch type: 0x", patchId);
        return;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        if (gChaosActiveEntries[i].id == patchId) {
            firstFoundMatch = &gChaosActiveEntries[i];
            matchIndex = i;
            break;
        }
    }

    if (!firstFoundMatch) {
        assert_args(FALSE, "%s%08X", "chaos_decrement_patch_usage:\nPatch not currently active: 0x", patchId);
        return;
    }

    assert_args(firstFoundMatch->remainingDuration > 0, "%s%08X", "chaos_decrement_patch_usage:\nRemaining duration for patch is 0: ", patchId);
    firstFoundMatch->remainingDuration--;


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
        chaos_remove_expired_entry(matchIndex--, NULL);
    }
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

        if (patch->conditionalFunc && !patch->conditionalFunc()) {
            availablePatches[i] = FALSE;
            continue;
        }
    }
}

void chaos_generate_patches(u8 severityCounts[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT],
                          u8 posNegPairings[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT], f32 severityWeights[CHAOS_PATCH_SEVERITY_COUNT]) {
    bzero(generatedPatches, sizeof(generatedPatches));

    // Generate patches
    for (s32 index = 0; index < CHAOS_PATCH_MAX_GENERATABLE; index++) {
        enum ChaosPatchID positivePatchId = CHAOS_PATCH_NONE_POSITIVE;
        enum ChaosPatchID negativePatchId = CHAOS_PATCH_NONE_NEGATIVE;

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

        for (s32 attempts = 0; attempts < RETRY_ATTEMPTS_DUPLICATES; attempts++) {
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

        negativePatchCompare = negativePatchId;

        s32 applicablePositiveCount = 0;
        for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
            const struct ChaosPatch *patch = &gChaosPatches[patchId];
            if (!availablePatches[patchId] || patch->effectType != CHAOS_EFFECT_POSITIVE || patch->severity != positiveSeverity) {
                continue;
            }
            if (gChaosPatches[negativePatchId].negationId && gChaosPatches[negativePatchId].negationId == patchId) {
                continue;
            }
            if (gChaosPatches[patchId].conditionalFunc && !gChaosPatches[patchId].conditionalFunc()) {
                continue;
            }

            applicablePositiveCount++;
        }

        if (applicablePositiveCount > 0) {
            for (s32 attempts = 0; attempts < RETRY_ATTEMPTS_DUPLICATES; attempts++) {
                s32 positiveWeight = (s32) (random_float() * applicablePositiveCount);

                for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
                    const struct ChaosPatch *patch = &gChaosPatches[patchId];
                    if (!availablePatches[patchId] || patch->effectType != CHAOS_EFFECT_POSITIVE || patch->severity != positiveSeverity) {
                        continue;
                    }
                    if (gChaosPatches[negativePatchId].negationId && gChaosPatches[negativePatchId].negationId == patchId) {
                        continue;
                    }
                    if (gChaosPatches[patchId].conditionalFunc && !gChaosPatches[patchId].conditionalFunc()) {
                        continue;
                    }

                    if (positiveWeight <= 0) {
                        positivePatchId = patchId;
                        break;
                    }

                    positiveWeight--;
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
                    break;
                }
            }
        }

        negativePatchCompare = CHAOS_PATCH_NONE;

        generatedPatches[index].positiveId = positivePatchId;
        generatedPatches[index].negativeId = negativePatchId;
        generatedPatches[index].positivePatch = &gChaosPatches[positivePatchId];
        generatedPatches[index].negativePatch = &gChaosPatches[negativePatchId];
        generatedPatches[index].severityLevel = generatedSeverity;
    }
}

struct ChaosPatchSelection *chaos_roll_for_new_patches(void) {
    if (!gChaosActiveEntryCount) {
        return NULL;
    }

    static s32 lastForcedDifficulty = -2;
    s32 forcedDifficulty;
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

    // Lessen likelihood of repeat aeverity randomization types in a row via attempts
    for (s32 attempts = 0; attempts < 2; attempts++) {
        f32 weight = 0.0f;
        forcedDifficulty = -1;
        generatedDifficultyWeight = random_float();
        for (s32 i = 0; i < ARRAY_COUNT(difficultyWeights[0]); i++) {
            weight += difficultyWeights[gChaosDifficulty][i];
            if (generatedDifficultyWeight < weight) {
                forcedDifficulty = i + 1;
                break;
            }
        }

        // Check if repeat or should break early
        if (forcedDifficulty != lastForcedDifficulty) {
            lastForcedDifficulty = forcedDifficulty;
            break;
        } else if (random_float() < 0.33f) {
            break;
        }
    }

    offsetSeverityWeight = random_float();

    if (starCount < 3) {
        forcedDifficulty = -1;
    }
    if (starCount < 5) {
        offsetSeverityWeight = 1.0f;
    }

    chaosmsg_print_debug("@FFFF009FforcedDifficulty: "
                         "@FF3F3F9F%d",
                         forcedDifficulty);

    // Determine available severity combinations that may be used for selections
    for (s32 i = 1; i < ARRAY_COUNT(posNegPairings); i++) {
        s32 pos = i;
        s32 neg = i;

        posNegPairings[i][CHAOS_EFFECT_POSITIVE] = 0;
        posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = 0;
        allowedSeverities[i] = FALSE;

        if (forcedDifficulty >= 0) {
            pos = forcedDifficulty;
            neg = forcedDifficulty;
        }

        // Determine whether to offset pairings of positive and negative effects
        if (offsetSeverityWeight < 0.15f) {
            // 15% chance to globally increase negative severity
            neg++;
            if (i == 1) {
                specialEvent = CHAOS_SPECIAL_PLUS1_NEGATIVE;
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fneg++");
            }
        } else if (offsetSeverityWeight < 0.30f) {
            // 15% chance to globally increase positive severity
            pos++;
            if (i == 1) {
                specialEvent = CHAOS_SPECIAL_PLUS1_POSITIVE;
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fpos++");
            }
        } else if (offsetSeverityWeight < 0.40f) {
            // 10% chance to eliminate all positive patches
            pos = 0;
            if (i == 1) {
                specialEvent = CHAOS_SPECIAL_ZERO_POSITIVE;
                chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9Fpos 0");
            }
        } else if (i == 1) {
            specialEvent = CHAOS_SPECIAL_NONE;
            chaosmsg_print_debug("@FFFF009FEVENT: @FF3F3F9FN/A");
        }

        if (gChaosDifficulty == CHAOS_DIFFICULTY_EASY) {
            neg--;
        } else if (gChaosDifficulty == CHAOS_DIFFICULTY_HARD) {
            pos--;
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

    // NOTE: Comment out these weight calculations now that a different severity weighting system exists (so these weights are all even now)
    // for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
    //     struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
    //     const struct ChaosPatch *patch = &gChaosPatches[entry->id];

    //     if (patch->durationType == CHAOS_DURATION_ONCE) {
    //         severityWeights[patch->severity] += 0.33f;
    //         continue;
    //     }
    //     if (patch->durationType == CHAOS_DURATION_USE_COUNT) {
    //         severityWeights[patch->severity] += 0.5f;
    //         continue;
    //     }
    //     if (patch->durationType == CHAOS_DURATION_INFINITE) {
    //         severityWeights[patch->severity] += 0.67f;
    //         continue;
    //     }

    //     severityWeights[patch->severity] += 1.0f;
    // }

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

    if (forcedDifficulty >= 0) {
        for (s32 index = 0; index < CHAOS_PATCH_MAX_GENERATABLE; index++) {
            generatedPatches[index].severityLevel = forcedDifficulty;
        }
    }

    for (s32 index = 0; index < CHAOS_PATCH_MAX_GENERATABLE; index++) {
        generatedPatches[index].specialEvent = specialEvent;
        if (specialEvent == CHAOS_SPECIAL_PLUS1_POSITIVE &&
                    gChaosDifficulty == CHAOS_DIFFICULTY_EASY &&
                    generatedPatches[index].severityLevel == 3
        ) {
            generatedPatches[index].specialEvent = CHAOS_SPECIAL_NONE;
        }

        if (specialEvent == CHAOS_SPECIAL_PLUS1_NEGATIVE &&
                    gChaosDifficulty == CHAOS_DIFFICULTY_HARD &&
                    generatedPatches[index].severityLevel == 3
        ) {
            generatedPatches[index].specialEvent = CHAOS_SPECIAL_NONE;
        }

        if (specialEvent == CHAOS_SPECIAL_ZERO_POSITIVE &&
                    gChaosDifficulty == CHAOS_DIFFICULTY_HARD &&
                    generatedPatches[index].severityLevel == 1
        ) {
            generatedPatches[index].specialEvent = CHAOS_SPECIAL_NONE;
        }
    }

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
    save_file_get_chaos_data(&gChaosActiveEntries, &gChaosActiveEntryCount, &gChaosDifficulty, &gChaosLivesEnabled);
    chaos_recompute_active_patch_counts();

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
}

void chaos_area_update(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    if (gCurrCourseNum == COURSE_NONE) {
        return;
    }

    if (sWarpDest.type == WARP_TYPE_SAME_AREA) {
        return;
    }

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

    gChaosLevelWarped = FALSE;
}

void chaos_frame_update(void) {
    if (!gChaosActiveEntryCount) {
        return;
    }

    if (gTimeStopState & TIME_STOP_ACTIVE) {
        return;
    }

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
}
