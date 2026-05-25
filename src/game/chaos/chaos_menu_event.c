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

u32 gChaosEventCount = 0;
enum ChaosPatchID gChaosEventActiveEvent = CHAOS_PATCH_NONE;
enum ChaosPatchID gChaosEventQueue[CHAOS_EVENT_QUEUE_SIZE] = {[0 ... (CHAOS_EVENT_QUEUE_SIZE - 1)] = CHAOS_PATCH_NONE};

static void chaos_menuevent_insert_event_at_start(enum ChaosPatchID patchId) {
    if (gChaosEventCount >= ARRAY_COUNT(gChaosEventQueue)) {
        chaosmsg_print_debug("Insert Start:\ngChaosEventQueue is full!");
    } else {
        gChaosEventCount++;
    }

    for (s32 i = gChaosEventCount - 1; i >= 1; i--) {
        gChaosEventQueue[i] = gChaosEventQueue[i - 1];
    }

    gChaosEventQueue[0] = patchId;
}

static void chaos_menuevent_insert_event_at_end(enum ChaosPatchID patchId) {
    if (gChaosEventCount >= ARRAY_COUNT(gChaosEventQueue)) {
        chaosmsg_print_debug("Insert End:\ngChaosEventQueue is full!");
        return;
    }
    
    gChaosEventQueue[gChaosEventCount] = patchId;
    gChaosEventCount++;
}

static enum ChaosPatchID chaos_menuevent_remove_event_at_start(void) {
    if (gChaosEventCount <= 0) {
        chaosmsg_print_debug("Remove Start:\ngChaosEventQueue is empty!");
        return CHAOS_PATCH_NONE;
    }

    enum ChaosPatchID returnItem = gChaosEventQueue[0];

    gChaosEventCount--;
    for (u32 i = 0; i < gChaosEventCount; i++) {
        gChaosEventQueue[i] = gChaosEventQueue[i + 1];
    }
    gChaosEventQueue[gChaosEventCount] = CHAOS_PATCH_NONE;

    return returnItem;
}

UNUSED static enum ChaosPatchID chaos_menuevent_remove_event_at_end(void) {
    if (gChaosEventCount <= 0) {
        chaosmsg_print_debug("Remove End:\ngChaosEventQueue is empty!");
        return CHAOS_PATCH_NONE;
    }

    gChaosEventCount--;
    enum ChaosPatchID returnItem = gChaosEventQueue[gChaosEventCount];
    gChaosEventQueue[gChaosEventCount] = CHAOS_PATCH_NONE;

    return returnItem;
}

void chaos_menuevent_init(void) {
    gChaosEventCount = 0;
    gChaosEventActiveEvent = CHAOS_PATCH_NONE;
    for (u32 i = 0; i < ARRAY_COUNT(gChaosEventQueue); i++) {
        gChaosEventQueue[i] = CHAOS_PATCH_NONE;
    }
}

void chaos_menuevent_populate_persistent_patch_events(void) {
    for (s32 i = 0; i < *gChaosActiveEntryCount; i++) {
        enum ChaosPatchID patchId = gChaosActiveEntries[i].id;
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (!patch->hasMenuEvent) {
            assert_args(!patch->chsMenuInitFunc, "chaos_menuevent_populate_persistent_patch_events:\nhasMenuEvent is FALSE, despite chsMenuInitFunc being set:\n0x%08X", patchId);
            assert_args(!patch->chsMenuUpdateFunc, "chaos_menuevent_populate_persistent_patch_events:\nhasMenuEvent is FALSE, despite chsMenuUpdateFunc being set:\n0x%08X", patchId);
            continue;
        }

        assert_args(patch->isStackable == FALSE || patch->durationType != CHAOS_DURATION_STARS, "chaos_menuevent_populate_timer_events:\nStackable star timer patches not supported:\n0x%08X", patchId);

        chaos_menuevent_insert_event_at_end(patchId);
    }
}

void chaos_menuevent_populate_nonpersistent_patch_event(enum ChaosPatchID patchId) {
    if (patchId == CHAOS_PATCH_NONE) {
        return;
    }

    const struct ChaosPatch *patch = &gChaosPatches[patchId];
    if (!patch->hasMenuEvent) {
        assert_args(!patch->chsMenuInitFunc, "chaos_menuevent_populate_persistent_patch_events:\nhasMenuEvent is FALSE, despite chsMenuInitFunc being set:\n0x%08X", patchId);
        assert_args(!patch->chsMenuUpdateFunc, "chaos_menuevent_populate_persistent_patch_events:\nhasMenuEvent is FALSE, despite chsMenuUpdateFunc being set:\n0x%08X", patchId);
        return;
    }

    // Give non-persistence priority
    chaos_menuevent_insert_event_at_start(patchId);
}

void chaos_menuevent_finish_event(void) {
    chaos_decrement_star_or_use_timer_with_id(gChaosEventActiveEvent);
    gChaosEventActiveEvent = CHAOS_PATCH_NONE;
}

u8 chaos_menuevent_update(Gfx **dl) {
    Gfx *dlHead = *dl;
    u8 shouldInit = FALSE;

    if (gChaosEventActiveEvent == CHAOS_PATCH_NONE) {
        gChaosEventActiveEvent = chaos_menuevent_remove_event_at_start();
        shouldInit = TRUE;

#ifdef CHAOS_ENGINE_DEBUG
        if (gChaosEventActiveEvent != CHAOS_PATCH_NONE) {
            chaosmsg_print_debug("Chaos event: 0x%08X", gChaosEventActiveEvent);
        }
#endif
    }

    if (gChaosEventActiveEvent != CHAOS_PATCH_NONE) {
        const struct ChaosPatch *patch = &gChaosPatches[gChaosEventActiveEvent];

        if (shouldInit && patch->chsMenuInitFunc) {
            patch->chsMenuInitFunc();
        }

        if (patch->chsMenuUpdateFunc) {
            patch->chsMenuUpdateFunc(&dlHead);
        }
    }

    *dl = dlHead;
    return (gChaosEventActiveEvent != CHAOS_PATCH_NONE || gChaosEventCount > 0);
}
