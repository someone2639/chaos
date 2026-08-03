#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#define CHAOS_EVENT_QUEUE_SIZE 32

extern u32 gChaosEventCount;
extern enum ChaosPatchID gChaosEventActiveEvent;
extern enum ChaosPatchID gChaosEventQueue[CHAOS_EVENT_QUEUE_SIZE];

// Initialize chaos event system
void chaos_menuevent_init(void);

// Function to populate chaos menu event queue with events that persist over multiple stars / indefinitely.
void chaos_menuevent_populate_persistent_patch_events(void);

// Function to populate chaos menu event queue with an event that only activates once.
// This event gets placed at the front of the queue, taking priority over persistent and older nonpersistent events.
void chaos_menuevent_populate_nonpersistent_patch_event(enum ChaosPatchID patchId);

// Function to be invoked whenever a particular event has concluded.
// This is to be manually invoked by the actual event callbacks, whenever they've finished.
void chaos_menuevent_finish_event(void);

// Entry function to handle special chaos menu events. This also handles all rendering for events.
// Return TRUE while events are still active; otherwise return FALSE when all events have concluded.
u8 chaos_menuevent_update(Gfx **dl);
