#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#define CHAOS_PATCH_SEVERITY_MIN 1
#define CHAOS_PATCH_SEVERITY_MAX 3

enum ChaosPatchID {
// Empty Modifiers
    CHAOS_PATCH_NONE_POSITIVE,
    CHAOS_PATCH_NONE_NEGATIVE,

// Lives Modifiers
    CHAOS_PATCH_LIVES_INCREASE_LV1,
    CHAOS_PATCH_LIVES_INCREASE_LV2,
    CHAOS_PATCH_LIVES_INCREASE_LV3,
    CHAOS_PATCH_LIVES_DECREASE_LV1,
    CHAOS_PATCH_LIVES_DECREASE_LV2,
    CHAOS_PATCH_LIVES_DECREASE_LV3,

// Star Modifiers
    CHAOS_PATCH_STARS_INCREASE_LV2,
    CHAOS_PATCH_STARS_INCREASE_LV3,
    CHAOS_PATCH_STARS_INCREASE_GUARANTEE,
    CHAOS_PATCH_STARS_DECREASE_LV2,
    CHAOS_PATCH_STARS_DECREASE_LV3,
    CHAOS_PATCH_STARS_DECREASE_GUARANTEE,

// Gravity Modifiers
    CHAOS_PATCH_GRAVITY_DECREASE_LV1,
    CHAOS_PATCH_GRAVITY_DECREASE_LV2,
    CHAOS_PATCH_GRAVITY_DECREASE_LV3,
    CHAOS_PATCH_GRAVITY_INCREASE_LV1,
    CHAOS_PATCH_GRAVITY_INCREASE_LV2,
    CHAOS_PATCH_GRAVITY_INCREASE_LV3,

// Patch Count
    CHAOS_PATCH_COUNT,
};

enum ChaosPatchEffectType {
    CHAOS_EFFECT_POSITIVE,
    CHAOS_EFFECT_NEGATIVE,
    // CHAOS_EFFECT_NEUTRAL, // Undecided if this is worth supporting yet
};

enum ChaosPatchDurationType {
    CHAOS_DURATION_ONCE,      // Calls init function, then deactivates immediately
    CHAOS_DURATION_INFINITE,  // Activates forever and cannot be deactivated
    CHAOS_DURATION_STARS,     // Duration is decremented for each star collected (blue stars included)
    CHAOS_DURATION_USE_COUNT, // Duration is decremented each time its use case is invoked (this must be done manually!)
};

// Remaining Duration and Patch ID will be tracked within a separate array of active patch data (to be defined later). Memory behind said array should also be copied to the save file.
// Any uses of activatedInitFunc should also consider save file reloads appropriately and never touch duration directly.
struct ChaosPatch {
    const enum ChaosPatchDurationType durationType; // How/when should this patch be consumed or deactivated?
    const enum ChaosPatchEffectType effectType;     // Is this patch beneficial or punishing?
    const enum ChaosPatchID negationId;             // This is the exact opposite of what effect (if any)? This is useful for deactivating an infinite, opposite effect rather than creating a useless stack, which helps oppose theoretically infinite memory requirements.
    const u8 severity;    // Usefulness or severity impact of the patch (must be between 1 and CHAOS_PATCH_SEVERITY_MAX, excluding CHAOS_PATCH_NONE_*)
    const u8 isStackable; // Can this patch be active more than once at a time?
    const u8 duration;    // Ignored for CHAOS_DURATION_ONCE and CHAOS_DURATION_INFINITE

    u8   (*conditionalFunc  )(const struct ChaosPatch *); // Check specific scenarios for whether this patch type is allowed to show up (Optional)
    void (*activatedInitFunc)(const struct ChaosPatch *); // Invoked the moment this patch takes effect (Optional)
    void (*areaInitFunc     )(const struct ChaosPatch *); // Invoked once immediately after warping into a new level/area that isn't COURSE_NONE (Optional)
    void (*frameUpdateFunc  )(const struct ChaosPatch *); // Invoked once at the start of each frame while active (Optional)
    void (*deactivationFunc )(const struct ChaosPatch *); // Invoked once the patch is deactivated (Optional)

    const char *name;             // Display name for the patch
    const char *shortDescription; // Short description for the patch
    const char *longDescription;  // Long description for the patch (if any)
};

extern const struct ChaosPatch gChaosPatches[CHAOS_PATCH_COUNT];





#include "chaos_patch_behaviors.h"
