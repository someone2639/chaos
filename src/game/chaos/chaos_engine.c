#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

#include "buffers/buffers.h"
#include "game/debug.h"

s32 *gChaosActiveEntryCount = &gSaveBuffer.files[0].chaosEntryCount;
struct ChaosActiveEntry *gChaosActiveEntries = gSaveBuffer.files[0].chaosEntries;

void chaos_init(UNUSED s32 arg, UNUSED s32 unused) {
    save_file_get_chaos_data(&gChaosActiveEntries, &gChaosActiveEntryCount);

    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        const enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->durationType == CHAOS_DURATION_ONCE) {
            continue;
        }

        if (patch->activatedInitFunc) {
            patch->activatedInitFunc(patch);
        }
    }
}

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
