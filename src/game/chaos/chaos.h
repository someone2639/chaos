#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#define CHAOS_PATCH_SEVERITY_MAX 3
#define CHAOS_PATCH_SEVERITY_COUNT (CHAOS_PATCH_SEVERITY_MAX + 1)

#ifndef DISPLAY_MAX_PATCHES
#define DEFAULT_PATCH_DISPLAY_QUANTITY 2
#else
#define DEFAULT_PATCH_DISPLAY_QUANTITY 4
#endif


#define CHAOS_PATCH_MAX_GENERATABLE 4

#define CHAOS_PATCH_ENTRIES 0x100

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

// Star/Save Modifiers
    CHAOS_PATCH_STARS_SHUFFLE_STARS,
    CHAOS_PATCH_STARS_INCREASE_LV2,
    CHAOS_PATCH_STARS_INCREASE_LV3,
    CHAOS_PATCH_STARS_INCREASE_GUARANTEE,
    CHAOS_PATCH_STARS_DECREASE_LV2,
    CHAOS_PATCH_STARS_DECREASE_LV3,
    CHAOS_PATCH_STARS_DECREASE_GUARANTEE,
    CHAOS_PATCH_GET_KEY_1,
    CHAOS_PATCH_GET_KEY_2,
    CHAOS_PATCH_UNLOCK_CANNONS,

// Gravity Modifiers
    CHAOS_PATCH_GRAVITY_DECREASE,
    CHAOS_PATCH_GRAVITY_INCREASE,

// Health Modifiers
    CHAOS_PATCH_NOHEAL_HEARTS,
    CHAOS_PATCH_NOHEAL_WATER,
    CHAOS_PATCH_NOHEAL_COINS,
    CHAOS_PATCH_HEALTH_DRAIN,
    CHAOS_PATCH_HEALTH_GAIN,
    CHAOS_PATCH_HEALTH_UP,
    CHAOS_PATCH_HEALTH_DOWN,
    CHAOS_PATCH_NO_FALL_DAMAGE,
    CHAOS_PATCH_TEMPORARY_INVINCIBILITY,
    CHAOS_PATCH_INSTAKILL_SQUISH,
    CHAOS_PATCH_INSTAKILL_GOOMBA,
    CHAOS_PATCH_INSTAKILL_LAVA,
    CHAOS_PATCH_EXTRADAMAGE_ENEMIES,
    CHAOS_PATCH_EXTRADAMAGE_LAVA,
    CHAOS_PATCH_BREATH_BOOST,
    CHAOS_PATCH_SHIELD,
    CHAOS_PATCH_RANDOM_INVINCIBILITY,

// Coin Modifiers
    CHAOS_PATCH_DOUBLE_COINS,
    CHAOS_PATCH_100C_DECREASE_LV2,
    CHAOS_PATCH_100C_DECREASE_LV3,
    CHAOS_PATCH_PAY2WIN,
    CHAOS_PATCH_6_RED_COINS,
    CHAOS_PATCH_SONIC_SIMULATOR,
    CHAOS_PATCH_BLUECOIN_LOTTERY,
    CHAOS_PATCH_COIN_SIZE,
    CHAOS_PATCH_SCARED_COINS,
    CHAOS_PATCH_COIN_MAGNET,

// Random Griefing
    CHAOS_PATCH_RANDOM_SLEEP,
    CHAOS_PATCH_RANDOM_SHOCK,
    CHAOS_PATCH_RANDOM_BURN,
    CHAOS_PATCH_RANDOM_BLIND,
    CHAOS_PATCH_RANDOM_DIALOGUE,
    CHAOS_PATCH_KAIZO_BLOCKS,
    CHAOS_PATCH_BATTLEFIELD,

// Movement Modifiers
    CHAOS_PATCH_LOSEMOVE_BREAKDANCE,
    CHAOS_PATCH_LOSEMOVE_LEDGE_GRAB,
    CHAOS_PATCH_LOSEMOVE_KICK,
    CHAOS_PATCH_LOSEMOVE_SIDEFLIP,
    CHAOS_PATCH_LOSEMOVE_LONG_JUMP,
    CHAOS_PATCH_LOSEMOVE_BACKFLIP,
    CHAOS_PATCH_LOSEMOVE_WALL_KICK,
    CHAOS_PATCH_LOSEMOVE_DOUBLE_JUMP,
    CHAOS_PATCH_BRAWL_TRIPPING,
    CHAOS_PATCH_GALAXY_SPIN,
    CHAOS_PATCH_GROUND_POUND_DIVE,
    CHAOS_PATCH_GROUND_POUND_JUMP,
    CHAOS_PATCH_STICKY_WALL_JUMP,
    CHAOS_PATCH_BETA,
    CHAOS_PATCH_HARDER_LONG_JUMPS,

// Object Spawners
    CHAOS_PATCH_GREEN_DEMON,
    CHAOS_PATCH_BULLET_HELL,
    CHAOS_PATCH_SPAWN_ON_SHELL,
    CHAOS_PATCH_DOUBLE_CHERRY,
    CHAOS_PATCH_COSMIC_CLONES,

// Visual Modifiers
    CHAOS_PATCH_NO_Z_BUFFER,
    CHAOS_PATCH_INVERTED_Z_BUFFER,
    CHAOS_PATCH_UPSIDE_DOWN_CAMERA,
    CHAOS_PATCH_DECREASED_FOV,
    CHAOS_PATCH_INCREASED_FOV,
    CHAOS_PATCH_CONFUSED_OBJECTS,
    CHAOS_PATCH_NO_SKYBOX,
    CHAOS_PATCH_45_FPS,
    CHAOS_PATCH_20_FPS,
    CHAOS_PATCH_TOP_DOWN_CAMERA,
    CHAOS_PATCH_MIRROR_MODE,
    CHAOS_PATCH_LOW_RESOLUTION,
    CHAOS_PATCH_BIG_HEAD,
    CHAOS_PATCH_ORTHO,

// Time Limit
    CHAOS_PATCH_TIME_LIMIT,
    CHAOS_PATCH_LOWER_TIME_LIMIT,

// Cheats
    CHAOS_PATCH_L_TO_LEVITATE,
    CHAOS_PATCH_DEBUG_FREE_MOVE,
    CHAOS_PATCH_LEVEL_RESET,

// Chaos Modifiers
    CHAOS_PATCH_REMOVE_NEGATIVE_PATCH,
    CHAOS_PATCH_ADD_SELECTABLE_PATCH,
    CHAOS_PATCH_REMOVE_SELECTABLE_PATCH,

// Speed Modifiers
    CHAOS_PATCH_PUSH_BACK,
    CHAOS_PATCH_WALKIES,
    CHAOS_PATCH_SPEED_TAX,

// Cap Effects
    CHAOS_PATCH_WING_CAP,
    CHAOS_PATCH_VANISH_CAP,
    CHAOS_PATCH_REMOVE_CAP,
    CHAOS_PATCH_DISABLE_CAPS,

// Input Modifiers
    CHAOS_PATCH_BUTTON_BROKEN_A,
    CHAOS_PATCH_BUTTON_BROKEN_B,
    CHAOS_PATCH_BUTTON_BROKEN_Z,
    CHAOS_PATCH_BUTTON_BROKEN_C,
    CHAOS_PATCH_SWAPPED_ZR_AB,
    CHAOS_PATCH_INVERTED_CAMERA_X,
    CHAOS_PATCH_INVERTED_STICK_X,
    CHAOS_PATCH_INVERTED_STICK_Y,
    CHAOS_PATCH_INPUT_LAG,
    CHAOS_PATCH_SM64_DS,

// Size Modifiers
    CHAOS_PATCH_MARIO_BIG,
    CHAOS_PATCH_MARIO_SMALL,

// Lethal Damages
    CHAOS_PATCH_LETHAL_BONK,
    CHAOS_PATCH_LETHAL_FALL_DAMAGE,

// Miscellaneous Modifiers
    CHAOS_PATCH_MARIO_INVISIBLE,
    CHAOS_PATCH_SIGNREAD_FAR,
    CHAOS_PATCH_ONE_HIT_WONDER,
    CHAOS_PATCH_NO_HUD,
    CHAOS_PATCH_FORCED_MARIO_CAM,
    CHAOS_PATCH_BOWSER_THROWS,
    CHAOS_PATCH_INVERTED_SOUND,
    CHAOS_PATCH_AD_BREAK,
    CHAOS_PATCH_ALL_STARS_SELECTABLE,
    CHAOS_PATCH_MIRACLE,
    CHAOS_PATCH_DISABLE_FADE_WARPS,
    CHAOS_PATCH_LUIGI,
    CHAOS_PATCH_REVERB,
    CHAOS_PATCH_WEAK_BOSSES,
    CHAOS_PATCH_QUICKTIME,
    CHAOS_PATCH_PLATFORM_MISPLACEMENT,
    CHAOS_PATCH_SLIPPERY_FLOORS,
    CHAOS_PATCH_MARTH_GRAB,
    CHAOS_PATCH_NOCLIP,
    CHAOS_PATCH_RAINBOW_STARS,
    CHAOS_PATCH_SHUFFLE_OBJECTS,

// Patch Count
    CHAOS_PATCH_COUNT,

// NOTE: Only to be used for comparisons, there are not indexing safeguards in place here!
    CHAOS_PATCH_NONE = 0xFFFFFFFF,
};

enum ChaosPatchEffectType {
    CHAOS_EFFECT_POSITIVE,
    CHAOS_EFFECT_NEGATIVE,
    // CHAOS_EFFECT_NEUTRAL, // Undecided if this is worth supporting yet

    CHAOS_EFFECT_COUNT,
};

enum ChaosDifficulty {
    CHAOS_DIFFICULTY_EASY,   // Offsets negative patches such that top severities should (almost) never show up
    CHAOS_DIFFICULTY_NORMAL, // Standard difficulty
    CHAOS_DIFFICULTY_HARD,   // Offsets positive patches such that top severities should (almost) never show up

    CHAOS_DIFFICULTY_COUNT,
};

enum ChaosPatchDurationType {
    CHAOS_DURATION_ONCE,      // Calls init function, then deactivates immediately
    CHAOS_DURATION_INFINITE,  // Activates forever and cannot be deactivated
    CHAOS_DURATION_STARS,     // Duration is decremented for each star collected (blue stars included)
    CHAOS_DURATION_USE_COUNT, // Duration is decremented each time its use case is invoked (this must be done manually!)
};

enum ChaosPatchSpecialEvent {
    CHAOS_SPECIAL_NONE,           // No special effect
    CHAOS_SPECIAL_PLUS1_POSITIVE, // Add +1 severity to each positive listing only
    CHAOS_SPECIAL_PLUS1_NEGATIVE, // Add +1 severity to each negative listing only
    CHAOS_SPECIAL_ZERO_POSITIVE,  // Floor positive severity to 0, effectively eliminating any positive effect

    CHAOS_SPECIAL_COUNT,
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

    u8   (*conditionalFunc  )(void); // Check specific scenarios for whether this patch type is allowed to show up (Optional)
    void (*activatedInitFunc)(void); // Invoked the moment this patch takes effect (Optional)
    void (*levelInitFunc    )(void); // Invoked once immediately after warping into a new level that isn't COURSE_NONE (Optional)
    void (*areaInitFunc     )(void); // Invoked once immediately after warping into a new level and/or area that isn't COURSE_NONE (Optional)
    void (*frameUpdateFunc  )(void); // Invoked once at the start of each frame while active (Optional)
    void (*deactivationFunc )(void); // Invoked once the patch is deactivated (Optional)

    const char *name;             // Display name for the patch
    const char *shortDescription; // Short description for the patch
    const char *longDescription;  // Long description for the patch (if any)
};

struct ChaosPatchSelection {
    enum ChaosPatchID positiveId;             // ID of positive generated patch
    enum ChaosPatchID negativeId;             // ID of negative generated patch
    enum ChaosPatchSpecialEvent specialEvent; // Special event used during generation to further manipulate patch severities
    const struct ChaosPatch *positivePatch;   // pointer to actual positive patch data
    const struct ChaosPatch *negativePatch;   // pointer to actual negative patch data

    u8 severityLevel; // For selection display (since actual patch severities may vary, can be 0)
};

// This represents the modifiable chaos patch data that ultimately gets saved to the save file.
// Anything not represented here needs to be kept track of and/or saved separately.
// Because these are saved, any property modification that gets negated via deactivation func SHOULD NOT BE be saved to the save file, even for stacking patches!
// Similarly, DO NOT track CHAOS_DURATION_USE_COUNT patches with separate variables!
// Adding manual save data is best reserved for tracking any variables with unrelated outside influence, such as Mario's lives.
// Lives are applied with a stackable CHAOS_DURATION_ONCE instead of CHAOS_DURATION_USE_COUNT for this reason.
struct ChaosActiveEntry {
    enum ChaosPatchID id; // ID of a currently active patch
    u32 remainingDuration : 8; // Number of stars/uses/etc left for this patch to remain active
    u32 frameTimer : 24;
};

extern const struct ChaosPatch gChaosPatches[CHAOS_PATCH_COUNT];
extern char gChaosInternalBuffer[0x1000];
extern s32 *gChaosActiveEntryCount;
extern struct ChaosActiveEntry *gChaosActiveEntries;
extern u8 gChaosLevelWarped;
extern enum ChaosDifficulty gChaosDifficulty;
extern u8 gChaosLivesEnabled;

// Check whether a particular chaos patch is active. Overall cheaper operation than the function below this one.
u8 chaos_check_if_patch_active(const enum ChaosPatchID patchId);

// Get patch data for an active patch (if active at all). Return TRUE if a match is found.
u8 chaos_find_first_active_patch(const enum ChaosPatchID patchId, struct ChaosActiveEntry **firstFoundMatch);

// Get the number of active instances for a particular patch.
u32 chaos_count_active_instances(const enum ChaosPatchID patchId);

// Deactivate an old chaos patch, based on its current index.
// Be careful when invoking this with stackable patches, as it may cause undesirable behavior if used incorrectly.
// In general it is not recommended to invoke this (externally) with stackable patches that use CHAOS_DURATION_USE_COUNT (since they get combined).
// Additionally a message may be printed, abiding by the format of chaosmsg_print.
void chaos_remove_expired_entry(const s32 patchIndex, const char *msg);

// Activate a new chaos patch.
void chaos_add_new_entry(const enum ChaosPatchID patchId);

// Decrement all of durations of each pass using a star timer (i.e. CHAOS_DURATION_STARS).
// Additionally deconstructs any applicable patches if their duration hits 0.
void chaos_decrement_star_timers(void);

// Handles behavior for decrementing consumable patches (i.e. CHAOS_DURATION_USE_COUNT).
// This does NOT invoke a callback for what it should do when the patch is consumed,
// however it will invoke the deactivation callback if remaining uses hit 0.
void chaos_decrement_patch_usage(const enum ChaosPatchID patchId);

// Generates a list of CHAOS_PATCH_MAX_GENERATABLE patch combinations for selection use.
struct ChaosPatchSelection *chaos_roll_for_new_patches(void);

// Send a new patch selection to be applied.
void chaos_select_patches(struct ChaosPatchSelection *patchSelection);

// Initialize the chaos data, to be run immediately after loading the save file.
// Activate any patches that were previously active in a different session.
void chaos_init(void);

// Invokes the area callback for each chaos patch as soon as Mario enters a new area.
// This function also executes the callback for level changes.
// Neither are invoked if the current course is COURSE_NONE.
void chaos_area_update(void);

// Invokes a frame update for each active and applicable chaos patch.
// Only updates if current play mode is normal (i.e. not paused) and timestop is inactive.
// Takes place right before objects are updated.
void chaos_frame_update(void);


#include "chaos_patch_behaviors.h"
#include "chaos_message.h"
