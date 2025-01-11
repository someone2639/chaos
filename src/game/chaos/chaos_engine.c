#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

#include "string.h"
#include "buffers/buffers.h"
#include "engine/behavior_script.h"
#include "game/debug.h"
#include "game/object_list_processor.h"

#define WEIGHT_OFFSET 1.5f // Cannot be 0 or else div by 0!

static u8 availablePatches[CHAOS_PATCH_COUNT];
static struct ChaosPatchSelection generatedPatches[CHAOS_PATCH_MAX_GENERATABLE];

s32 *gChaosActiveEntryCount = &gSaveBuffer.files[0].chaosEntryCount;
struct ChaosActiveEntry *gChaosActiveEntries = gSaveBuffer.files[0].chaosEntries;
enum ChaosDifficulty gChaosDifficulty = CHAOS_DIFFICULTY_NORMAL;

u8 chaos_check_if_patch_active(const enum ChaosPatchID patchId, struct ChaosActiveEntry **firstFoundMatch) {
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

void chaos_remove_expired_entry(const s32 patchIndex) {
    const enum ChaosPatchID patchId = gChaosActiveEntries[patchIndex].id;
    const struct ChaosPatch *patch = &gChaosPatches[patchId];

    gSaveFileModified = TRUE;

    if (patchIndex >= *gChaosActiveEntryCount || patchIndex < 0) {
        // May as well make these aggressions instead of asserts, since they'll infiniloop in practice anyway if we don't.
        aggress(*gChaosActiveEntryCount > 0, "chaos_remove_expired_entry:\nNo patches are currently active!");
        aggress(FALSE, "chaos_remove_expired_entry:\nPatch index out of range!");

        return;
    }

    // Activate init func and return immediately, do not add to patch array
    if (patch->deactivationFunc) {
        patch->deactivationFunc(patch);
    }

    // Decrease number of active patches and move the last active patch in the array to the index of the removed patch.
    // This means of moving stuff around implies we cannot make use of sorting order!
    (*gChaosActiveEntryCount)--;
    gChaosActiveEntries[patchIndex] = gChaosActiveEntries[*gChaosActiveEntryCount];
}

void chaos_add_new_entry(const enum ChaosPatchID patchId) {
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

            // Deactivate negated action, and also instantly activate and deactivate the new function (deactivate will not ever execute for ONCE entries that aren't stackable)
            chaos_remove_expired_entry(i--);
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc(patch);
            }
            if (patch->deactivationFunc) {
                patch->deactivationFunc(patch);
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
            assert_args(patch->activatedInitFunc || patch->deactivationFunc, "%s%08X", "chaos_add_new_entry\nAttempted to add stackable ONCE patch\nwithout a callback: 0x", patchId);
            if (patch->activatedInitFunc) {
                patch->activatedInitFunc(patch);
            }
            if (patch->deactivationFunc) {
                patch->deactivationFunc(patch);
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
                patch->activatedInitFunc(patch);
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

    // Set values for new entry appropriately
    newEntry->id = patchId;
    newEntry->frameTimer = 0;
    if (patch->durationType == CHAOS_DURATION_ONCE || patch->durationType == CHAOS_DURATION_USE_COUNT) {
        newEntry->remainingDuration = 0;
    } else {
        newEntry->remainingDuration = patch->duration;
        assert_args(newEntry->remainingDuration > 0, "%s%08X", "chaos_add_new_entry:\nDuration-type patch contains duration of 0: 0x", patchId);
    }

    // Activate init func for new entry
    if (patch->activatedInitFunc) {
        patch->activatedInitFunc(patch);
    }

    assert(patch->deactivationFunc == NULL || patch->durationType != CHAOS_DURATION_ONCE, "chaos_add_new_entry:\nDeactivation func will never run for CHAOS_DURATION_ONCE entries!\nRemove this assert if this becomes desirable.");
}

void chaos_decrement_star_timers(void) {
    gSaveFileModified = TRUE;

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        if (gChaosPatches[entry->id].durationType != CHAOS_DURATION_STARS) {
            continue;
        }

        assert_args(entry->remainingDuration > 0, "%s%08X", "chaos_decrement_star_timers:\nRemaining duration for patch is 0: ", entry->id);
        entry->remainingDuration--;
        if (entry->remainingDuration <= 0) {
            chaos_remove_expired_entry(i--);
        }
    }
}

void chaos_decrement_patch_usage(const enum ChaosPatchID patchId) {
    struct ChaosActiveEntry *firstFoundMatch = NULL;
    s32 matchIndex = -1;

    gSaveFileModified = TRUE;

    if (gChaosPatches[patchId].duration != CHAOS_DURATION_USE_COUNT) {
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
    if (firstFoundMatch->remainingDuration <= 0) {
        chaos_remove_expired_entry(matchIndex--);
    }
}

// Update a complete list of patches that are acceptible for generation
static void chaos_update_available_patches(void) {
    memset(availablePatches, TRUE, ARRAY_COUNT(availablePatches));

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const struct ChaosPatch *patch = &gChaosPatches[entry->id];

        if (!patch->isStackable || (patch->conditionalFunc && !patch->conditionalFunc(patch))) {
            availablePatches[entry->id] = FALSE;
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

        s32 totalWeight = 0.0f;
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

        s32 positiveSeverity = posNegPairings[generatedSeverity][CHAOS_EFFECT_POSITIVE];
        s32 negativeSeverity = posNegPairings[generatedSeverity][CHAOS_EFFECT_NEGATIVE];
        s32 positiveWeight = (s32) (random_float() * (f32) severityCounts[positiveSeverity][CHAOS_EFFECT_POSITIVE]);
        s32 negativeWeight = (s32) (random_float() * (f32) severityCounts[negativeSeverity][CHAOS_EFFECT_NEGATIVE]);

        for (enum ChaosPatchID patchId = 0; patchId < CHAOS_PATCH_COUNT; patchId++) {
            const struct ChaosPatch *patch = &gChaosPatches[patchId];
            if (!availablePatches[patchId] || patch->effectType != CHAOS_EFFECT_POSITIVE || patch->severity != positiveSeverity) {
                continue;
            }

            if (positiveWeight <= 0) {
                positivePatchId = patchId;
                break;
            }

            positiveWeight--;
        }

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

        generatedPatches[index].positiveId = positivePatchId;
        generatedPatches[index].negativeId = negativePatchId;
        generatedPatches[index].positivePatch = &gChaosPatches[positivePatchId];
        generatedPatches[index].negativePatch = &gChaosPatches[negativePatchId];
        generatedPatches[index].severityLevel = generatedSeverity;
    }
}

struct ChaosPatchSelection *chaos_roll_for_new_patches(void) {
    u8 severityCounts[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT];
    u8 posNegPairings[CHAOS_PATCH_SEVERITY_COUNT][CHAOS_EFFECT_COUNT];
    s8 allowedSeverities[CHAOS_PATCH_SEVERITY_COUNT];
    f32 severityWeights[CHAOS_PATCH_SEVERITY_COUNT];
    f32 offsetSeverityWeight;

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

    offsetSeverityWeight = random_float();

    // Determine available severity combinations that may be used for selections
    for (s32 i = 1; i < ARRAY_COUNT(posNegPairings); i++) {
        s32 pos = i;
        s32 neg = i;

        posNegPairings[i][CHAOS_EFFECT_POSITIVE] = 0;
        posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = 0;
        allowedSeverities[i] = FALSE;

        // Determine whether to offset pairings of positive and negative effects
        if (offsetSeverityWeight >= 0.90f) {
            pos++;
        } else if (offsetSeverityWeight < 0.20f) {
            neg++;
        }
        if (gChaosDifficulty == CHAOS_DIFFICULTY_EASY) {
            neg--;
        } else if (gChaosDifficulty == CHAOS_DIFFICULTY_HARD) {
            pos--;
        }
        if (pos > CHAOS_PATCH_SEVERITY_MAX) {
            pos = CHAOS_PATCH_SEVERITY_MAX;
        }
        if (neg > CHAOS_PATCH_SEVERITY_MAX) {
            neg = CHAOS_PATCH_SEVERITY_MAX;
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
            posNegPairings[i][CHAOS_EFFECT_POSITIVE] = i;

            if (i < CHAOS_PATCH_SEVERITY_MAX && severityCounts[i + 1][CHAOS_EFFECT_NEGATIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = i + 1;
                allowedSeverities[i] = TRUE;
                continue;
            }

            if (i > 0 && severityCounts[i - 1][CHAOS_EFFECT_NEGATIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = i - 1;
                allowedSeverities[i] = TRUE;
                continue;
            }
        }

        if (severityCounts[neg][CHAOS_EFFECT_NEGATIVE] > 0) {
            posNegPairings[i][CHAOS_EFFECT_NEGATIVE] = i;

            if (i > 0 && severityCounts[i - 1][CHAOS_EFFECT_POSITIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_POSITIVE] = i - 1;
                allowedSeverities[i] = TRUE;
                continue;
            }

            if (i < CHAOS_PATCH_SEVERITY_MAX && severityCounts[i + 1][CHAOS_EFFECT_POSITIVE] > 0) {
                posNegPairings[i][CHAOS_EFFECT_POSITIVE] = i + 1;
                allowedSeverities[i] = TRUE;
                continue;
            }
        }
    }

    // Compute weights for generation
    f32 totalWeight = 0.0f;
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        struct ChaosActiveEntry *entry = &gChaosActiveEntries[i];
        const struct ChaosPatch *patch = &gChaosPatches[entry->id];

        if (patch->durationType == CHAOS_DURATION_ONCE) {
            severityWeights[patch->severity] += 0.33f;
            continue;
        }
        if (patch->durationType == CHAOS_DURATION_USE_COUNT) {
            severityWeights[patch->severity] += 0.5f;
            continue;
        }
        if (patch->durationType == CHAOS_DURATION_INFINITE) {
            severityWeights[patch->severity] += 0.67f;
            continue;
        }

        severityWeights[patch->severity] += 1.0f;
    }
    for (s32 i = 0; i < ARRAY_COUNT(severityWeights); i++) {
        severityWeights[i] += WEIGHT_OFFSET;
        totalWeight += severityWeights[i];
    }
    for (s32 i = 0; i < ARRAY_COUNT(severityWeights); i++) {
        severityWeights[i] = 1.0f - (severityWeights[i] / totalWeight);
        severityWeights[i] *= severityWeights[i];
        if (i != 0 && !allowedSeverities[i]) {
            severityWeights[i] = 0.0f;
        }
    }

    chaos_generate_patches(severityCounts, posNegPairings, severityWeights);
    return generatedPatches;
}

void chaos_select_patches(struct ChaosPatchSelection *patchSelection) {
    chaos_add_new_entry(patchSelection->positiveId);
    chaos_add_new_entry(patchSelection->negativeId);
}

void chaos_init(UNUSED s32 arg, UNUSED s32 unused) {
    save_file_get_chaos_data(&gChaosActiveEntries, &gChaosActiveEntryCount);

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->durationType == CHAOS_DURATION_ONCE) {
            continue;
        }

        if (patch->activatedInitFunc) {
            gChaosActiveEntries[i].frameTimer = 0; // Is this desirable?
            patch->activatedInitFunc(patch);
        }
    }

    chaos_update_available_patches();
}

void chaos_area_update(void) {
    if (gCurrCourseNum == COURSE_NONE) {
        return;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->areaInitFunc) {
            patch->areaInitFunc(patch);
        }
    }
}

void chaos_frame_update(void) {
    if (gTimeStopState | TIME_STOP_ACTIVE) {
        return;
    }

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->frameUpdateFunc) {
            patch->frameUpdateFunc(patch);
            if (gChaosActiveEntries[i].frameTimer < 0xFFFFFF) {
                gChaosActiveEntries[i].frameTimer++;
            }
        }
    }
}
