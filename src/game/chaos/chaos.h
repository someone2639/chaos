#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/area.h"

#define CHAOS_PATCH_SEVERITY_MAX 3
#define CHAOS_PATCH_SEVERITY_COUNT (CHAOS_PATCH_SEVERITY_MAX + 1)

#define HARD_DURATION_DEFAULT_OFFSET_POSITIVE 0
#define HARD_DURATION_DEFAULT_OFFSET_NEGATIVE 0
#define IMPOSSIBLE_DURATION_DEFAULT_OFFSET_POSITIVE 0
#define IMPOSSIBLE_DURATION_DEFAULT_OFFSET_NEGATIVE 0

#ifndef DISPLAY_MAX_PATCHES
#define DEFAULT_PATCH_DISPLAY_QUANTITY 2
#else
#define DEFAULT_PATCH_DISPLAY_QUANTITY 4
#endif


#define CHAOS_PATCH_MAX_GENERATABLE 4
#define CHAOS_PATCH_DEFERRED_QUEUE_SIZE 32

#define CHAOS_PATCH_ENTRIES 0x100

#define CHAOS_MIN_STARS_FOR_FORCED_DIFFICULTIES 3
#define CHAOS_MIN_STARS_FOR_EVENTS 5

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
    CHAOS_PATCH_LIFE_GAMBLER,
    CHAOS_PATCH_LIFETIME_SALE,
    CHAOS_PATCH_INSTANT_GAME_OVER,

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
    CHAOS_PATCH_GET_WING_CAP,
    CHAOS_PATCH_GET_METAL_CAP,
    CHAOS_PATCH_GET_VANISH_CAP,
    CHAOS_PATCH_LOCK_CANNONS,
    CHAOS_PATCH_UNLOCK_CANNONS,
    CHAOS_PATCH_UNLOCK_STAR_DOORS,
    CHAOS_PATCH_STAR_CLONING_DEVICE,
    CHAOS_PATCH_COIN_FLIP,
    CHAOS_PATCH_TOAD_STAR_RESTOCK,
    CHAOS_PATCH_HIGH_ROLLER,

// Gravity Modifiers
    CHAOS_PATCH_GRAVITY_DECREASE,
    CHAOS_PATCH_GRAVITY_INCREASE,

// Health Modifiers
    CHAOS_PATCH_NOHEAL_HEARTS,
    CHAOS_PATCH_NOHEAL_WATER,
    CHAOS_PATCH_NOHEAL_COINS,
    CHAOS_PATCH_HEALING_BONUS,
    CHAOS_PATCH_HEALTH_DRAIN,
    CHAOS_PATCH_HEALTH_GAIN,
    CHAOS_PATCH_HEALTH_UP,
    CHAOS_PATCH_HEALTH_DOWN,
    CHAOS_PATCH_NO_FALL_DAMAGE,
    CHAOS_PATCH_NO_LAVA_DAMAGE,
    CHAOS_PATCH_TEMPORARY_INVINCIBILITY,
    CHAOS_PATCH_INSTAKILL_SQUISH,
    CHAOS_PATCH_INSTAKILL_GOOMBA,
    CHAOS_PATCH_INSTAKILL_LAVA,
    CHAOS_PATCH_EXTRADAMAGE_ENEMIES,
    CHAOS_PATCH_EXTRADAMAGE_LAVA,
    CHAOS_PATCH_BREATH_BOOST,
    CHAOS_PATCH_OXYGEN_TANK,
    CHAOS_PATCH_SHIELD,
    CHAOS_PATCH_RANDOM_INVINCIBILITY,
    CHAOS_PATCH_A_BUTTON_CHALLENGE,
    CHAOS_PATCH_DAMAGE_LOTTERY,
    CHAOS_PATCH_MARIO_DIES,

// Coin Modifiers
    CHAOS_PATCH_DOUBLE_COINS,
    CHAOS_PATCH_100C_DECREASE_LV2,
    CHAOS_PATCH_100C_DECREASE_LV3,
    CHAOS_PATCH_PAY2WIN,
    CHAOS_PATCH_COLLECTORS_ANXIETY,
    CHAOS_PATCH_6_RED_COINS,
    CHAOS_PATCH_SONIC_SIMULATOR,
    CHAOS_PATCH_BLUECOIN_LOTTERY,
    CHAOS_PATCH_COIN_SIZE,
    CHAOS_PATCH_SCARED_COINS,
    CHAOS_PATCH_COIN_MAGNET,
    CHAOS_PATCH_ENEMY_BLUE_COINS,

// Random Griefing
    CHAOS_PATCH_RANDOM_SLEEP,
    CHAOS_PATCH_RANDOM_SHOCK,
    CHAOS_PATCH_RANDOM_BURN,
    CHAOS_PATCH_RANDOM_BLIND,
    CHAOS_PATCH_RANDOM_DIALOGUE,
    CHAOS_PATCH_KAIZO_BLOCKS,
    CHAOS_PATCH_BATTLEFIELD,
    CHAOS_PATCH_TROLL_SOUNDS,
    CHAOS_PATCH_RED_LIGHT,
    CHAOS_PATCH_COSMIC_RAYS,
    CHAOS_PATCH_RANDOM_BUTTON_PRESSES,
    CHAOS_PATCH_RANDOM_INVISIBLE_WALLS,

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
    CHAOS_PATCH_FORWARDS_BLJ,
    CHAOS_PATCH_CANT_STOP_WONT_STOP,
    CHAOS_PATCH_REFRIGERATOR_MOVEMENT,
    CHAOS_PATCH_UNDERWATER_GROUNDPOUND,
    CHAOS_PATCH_SUNSHINE_TWIRL,

// Object Spawners
    CHAOS_PATCH_GREEN_DEMON,
    CHAOS_PATCH_BULLET_HELL,
    CHAOS_PATCH_SPAWN_ON_SHELL,
    CHAOS_PATCH_DOUBLE_CHERRY,
    CHAOS_PATCH_COSMIC_CLONES,
    CHAOS_PATCH_WATER_BOMBS,

// Visual Modifiers
    CHAOS_PATCH_NO_Z_BUFFER,
    CHAOS_PATCH_INVERTED_Z_BUFFER,
    CHAOS_PATCH_UPSIDE_DOWN_CAMERA,
    CHAOS_PATCH_SIDEWAYS_CAMERA,
    CHAOS_PATCH_ROCKING_CAMERA,
    CHAOS_PATCH_DECREASED_FOV,
    CHAOS_PATCH_INCREASED_FOV,
    CHAOS_PATCH_CONFUSED_OBJECTS,
    CHAOS_PATCH_UPSIDE_DOWN_OBJECTS,
    CHAOS_PATCH_NO_SKYBOX,
    CHAOS_PATCH_20_FPS,
    CHAOS_PATCH_45_FPS,
    CHAOS_PATCH_60_FPS,
    CHAOS_PATCH_TOP_DOWN_CAMERA,
    CHAOS_PATCH_MIRROR_MODE,
    CHAOS_PATCH_LOW_RESOLUTION,
    CHAOS_PATCH_BIG_HEAD,
    CHAOS_PATCH_ORTHO,
    CHAOS_PATCH_DIZZY_OBJECTS,
    CHAOS_PATCH_CAMERA_LAG,
    CHAOS_PATCH_DARKNESS,
    CHAOS_PATCH_CLOWN_VOMIT,
    CHAOS_PATCH_45_DEGREE_CAM,
    CHAOS_PATCH_SMOOTH_CAM,
    CHAOS_PATCH_CARTRIDGE_TILT,
    CHAOS_PATCH_CORRUPTION,
    CHAOS_PATCH_SQUINT_MODE,

// Time Limit
    CHAOS_PATCH_TIME_LIMIT,
    CHAOS_PATCH_LOWER_TIME_LIMIT,

// Cheats
    CHAOS_PATCH_L_TO_LEVITATE,
    CHAOS_PATCH_DEBUG_FREE_MOVE,

// Chaos Modifiers
    CHAOS_PATCH_REMOVE_NEGATIVE_PATCH,
    CHAOS_PATCH_ADD_RANDOM_BUFF,
    CHAOS_PATCH_BITE_SIZED_SPLIT,
    CHAOS_PATCH_ADD_SELECTABLE_PATCH,
    CHAOS_PATCH_REMOVE_SELECTABLE_PATCH,
    CHAOS_PATCH_LUCKY_CHARM,
    CHAOS_PATCH_UNLUCKY_CHARM,
    CHAOS_PATCH_UNEVENTFUL,
    CHAOS_PATCH_POSITIVE_EXTENSION,
    CHAOS_PATCH_NEGATIVE_EXTENSION,
    CHAOS_PATCH_LOW_STAKES,
    CHAOS_PATCH_HIGH_STAKES,
    CHAOS_PATCH_FORGIVENESS,
    CHAOS_PATCH_LETS_GO_GAMBLING,
    CHAOS_PATCH_SWEET_RELIEF,

// Speed Modifiers
    CHAOS_PATCH_PUSH_BACK,
    CHAOS_PATCH_WALKIES,
    CHAOS_PATCH_SPEED_TAX,

// Cap Effects
    CHAOS_PATCH_WING_CAP,
    CHAOS_PATCH_VANISH_CAP,
    CHAOS_PATCH_REMOVE_CAP,
    CHAOS_PATCH_DISABLE_CAPS,
    CHAOS_PATCH_UNCAPPED_CAPS,

// Input Modifiers
    CHAOS_PATCH_BUTTON_BROKEN_A,
    CHAOS_PATCH_BUTTON_BROKEN_B,
    CHAOS_PATCH_BUTTON_BROKEN_Z,
    CHAOS_PATCH_BUTTON_BROKEN_C,
    CHAOS_PATCH_SWAPPED_ZR_AB,
    CHAOS_PATCH_SWAPPED_C_STICK,
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

// Audio Modifiers
    CHAOS_PATCH_INVERTED_SOUND,
    CHAOS_PATCH_REVERB,
    CHAOS_PATCH_RANDOMIZED_MUSIC,
    CHAOS_PATCH_AUDIO_DELAY,
    CHAOS_PATCH_CHILL_OUT,
    CHAOS_PATCH_MAD_MUSICAL_MESS,
    CHAOS_PATCH_SOUND_EFFECT_SHUFFLE,
    CHAOS_PATCH_PLEASANT_CAMERA_SOUNDS,

// Miscellaneous Modifiers
    CHAOS_PATCH_NO_TOLERANCE_FALL_DAMAGE,
    CHAOS_PATCH_FALL_CANCEL_CANCEL,
    CHAOS_PATCH_MARIO_INVISIBLE,
    CHAOS_PATCH_SIGNREAD_FAR,
    CHAOS_PATCH_ONE_HIT_WONDER,
    CHAOS_PATCH_NO_HUD,
    CHAOS_PATCH_FORCED_MARIO_CAM,
    CHAOS_PATCH_BOWSER_THROWS,
    CHAOS_PATCH_AD_BREAK,
    CHAOS_PATCH_ALL_STARS_SELECTABLE,
    CHAOS_PATCH_SAFETY_NET,
    CHAOS_PATCH_MIRACLE_NORMAL,
    CHAOS_PATCH_MIRACLE_HARDCORE,
    CHAOS_PATCH_DISABLE_FADE_WARPS,
    CHAOS_PATCH_LUIGI,
    CHAOS_PATCH_WEAK_BOSSES,
    CHAOS_PATCH_RESPAWNABLE_SHELLS,
    CHAOS_PATCH_NO_RIDING_SHELLS,
    CHAOS_PATCH_QUICKTIME,
    CHAOS_PATCH_PLATFORM_MISPLACEMENT,
    CHAOS_PATCH_CLIMBING_BOOTS,
    CHAOS_PATCH_SLIPPERY_FLOORS,
    CHAOS_PATCH_MARTH_GRAB,
    CHAOS_PATCH_NOCLIP,
    CHAOS_PATCH_RAINBOW_STARS,
    CHAOS_PATCH_SHUFFLE_OBJECTS,
    CHAOS_PATCH_RANDOMIZE_WARPS,
    CHAOS_PATCH_FROM_BEYOND_THE_GRAVE,
    CHAOS_PATCH_DVD,
    CHAOS_PATCH_NUMBER_BLINDNESS,
    CHAOS_PATCH_TETRIS,
    CHAOS_PATCH_STAR_MEDALLION,
    CHAOS_PATCH_MORE_HUD,

// Patch Count
    CHAOS_PATCH_COUNT,

// NOTE: Only to be used for comparisons, there are not indexing safeguards in place here!
    CHAOS_PATCH_NONE = 0xFFFFFFFFu,
};

enum ChaosPatchEffectType {
    CHAOS_EFFECT_POSITIVE,
    CHAOS_EFFECT_NEGATIVE,
    // CHAOS_EFFECT_NEUTRAL, // Undecided if this is worth supporting yet

    CHAOS_EFFECT_COUNT,
};

enum ChaosDifficulty {
    CHAOS_DIFFICULTY_EASY,       // Offsets negative patches such that top severities should (almost) never show up
    CHAOS_DIFFICULTY_NORMAL,     // Standard difficulty
    CHAOS_DIFFICULTY_HARD,       // Offsets positive patches such that top severities should (almost) never show up
    CHAOS_DIFFICULTY_IMPOSSIBLE, // Positive patches will never show up, negative patches last EVEN longer

    CHAOS_DIFFICULTY_COUNT,
};

enum ChaosGameMode {
    CHAOS_GAMEMODE_CLASSIC,   // Default game mode, dying results in no additional punishment
    CHAOS_GAMEMODE_CHALLENGE, // Enable lives. Running out of lives will delete the save file.
    CHAOS_GAMEMODE_HARDCORE,  // Only one life. Dying even once will delete the save file.

    CHAOS_GAMEMODE_COUNT,
};

enum ChaosPatchDurationType {
    CHAOS_DURATION_DO_NOT_FORCE = -1, // Used to specify no forced duration type when checking conditionals
    CHAOS_DURATION_ONCE         =  0, // Calls init function, then deactivates immediately
    CHAOS_DURATION_INFINITE,          // Activates forever and cannot be deactivated
    CHAOS_DURATION_STARS,             // Duration is decremented for each star collected (blue stars included)
    CHAOS_DURATION_USE_COUNT,         // Duration is decremented each time its use case is invoked (this must be done manually!)
};

enum ChaosPatchSpecialEvent {
    CHAOS_SPECIAL_DO_NOT_FORCE = -1, // Used to specify no event override for patch generation
    CHAOS_SPECIAL_NONE         =  0, // No special effect
    CHAOS_SPECIAL_PLUS1_POSITIVE,    // Add +1 severity to each positive listing only
    CHAOS_SPECIAL_PLUS1_NEGATIVE,    // Add +1 severity to each negative listing only
    CHAOS_SPECIAL_ZERO_POSITIVE,     // Floor positive severity to 0, effectively eliminating any positive effect

    CHAOS_SPECIAL_COUNT,
};

enum ChaosStarDecrementType {
    CHAOS_STAR_DECREMENT_ALL,
    CHAOS_STAR_DECREMENT_STANDARD,
    CHAOS_STAR_DECREMENT_MENU_IMPACTING,
};

// Remaining Duration and Patch ID will be tracked within a separate array of active patch data (to be defined later). Memory behind said array should also be copied to the save file.
// Any uses of activatedInitFunc should also consider save file reloads appropriately and never touch duration directly.
struct ChaosPatch {
    const enum ChaosPatchDurationType durationType; // How/when should this patch be consumed or deactivated?
    const enum ChaosPatchEffectType effectType;     // Is this patch beneficial or punishing?
    const enum ChaosPatchID negationId;             // This is the exact opposite of what effect (if any)? This is useful for deactivating an infinite, opposite effect rather than creating a useless stack, which helps oppose theoretically infinite memory requirements.
    const u8 severity;           // Usefulness or severity impact of the patch (must be between 1 and CHAOS_PATCH_SEVERITY_MAX, excluding CHAOS_PATCH_NONE_*)
    const u8 isStackable;        // Can this patch be active more than once at a time?
    const u8 affectsPatchSelect; // Directly impacts patch generation; star index should be decremented only after a new patch has been selected.
    const u8 disableForHardcore; // Should this patch type be disabled in hardcore mode?
    const u8 duration;           // How long should the patch last, or how many uses are left? (Ignored for CHAOS_DURATION_ONCE and CHAOS_DURATION_INFINITE)
    const u8 durationHard;       // Duration to be used in Hard mode (except when set to 0)
    const u8 durationImpossible; // Duration to be used in Impossible mode (except when set to 0)

    const u8 incompatibleCount;            // Number of specified incompatible patches
    const enum ChaosPatchID *incompatible; // List of all incompatible patch IDs (Optional)
#ifdef DEBUG_ASSERTIONS
    const u8 __dbg_exemptCount;            // Number of specified exempt patches
    const enum ChaosPatchID *__dbg_exempt; // List of all patch IDs in which associated patch lists as incompatible, but this one does not (Optional, for debugging purposes to catch any missed mutual exclusions)
#endif

    u8   (*conditionalFunc  )(void);                     // Check specific scenarios for whether this patch type is allowed to show up, beyond just conflicting patch IDs (Optional)
    void (*activatedInitFunc)(void);                     // Invoked the moment this patch takes effect (Optional)
    void (*levelInitFunc    )(void);                     // Invoked once immediately after warping into a new level that isn't COURSE_NONE (Optional)
    void (*areaInitFunc     )(void);                     // Invoked once immediately after warping into a new level and/or area that isn't COURSE_NONE (Optional, excludes instant warps)
    void (*instWarpPreFunc  )(struct InstantWarp *warp); // Invoked once immediately before instant warping into a new area that isn't in COURSE_NONE (Optional)
    void (*instWarpPostFunc )(struct InstantWarp *warp); // Invoked once immediately after instant warping into a new area that isn't in COURSE_NONE (Optional)
    void (*frameUpdateFunc  )(void);                     // Invoked once at the start of each frame while active (Optional)
    void (*deactivationFunc )(void);                     // Invoked once the patch is deactivated (Optional)

    const u8 hasMenuEvent;               // Does this patch have a chaos menu event? (Events activated immediately following patch selection, but prior to raising the curtain. Mainly for user-facing render events, such as Coin Flip or Let's Go Gambling!)
    void (*chsMenuInitFunc)(void);       // Invoked upon initialization of menu event (Optional if `hasMenuEvent` is TRUE)
    void (*chsMenuUpdateFunc)(Gfx **dl); // Invoked as the update/render entry point during a menu event (Mandatory if `hasMenuEvent` is TRUE)

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
// Because these are saved, any property modification that gets negated via deactivation func SHOULD NOT be saved to the save file, even for stacking patches!
// Similarly, DO NOT track CHAOS_DURATION_USE_COUNT patches with separate variables!
// Adding manual save data is best reserved for tracking any variables with unrelated outside influence, such as Mario's lives.
// Lives are applied with a stackable CHAOS_DURATION_ONCE instead of CHAOS_DURATION_USE_COUNT for this reason.
struct ChaosActiveEntry {
    enum ChaosPatchID id; // ID of a currently active patch
    u32 remainingDuration : 8; // Number of stars/uses/etc left for this patch to remain active
    u32 frameTimer : 24; // NOTE: Not actually read from the save file in practice (but still needed as a runtime property)
};

extern const struct ChaosPatch gChaosPatches[CHAOS_PATCH_COUNT];
extern char gChaosInternalBuffer[0x1000];
extern s32 *gChaosActiveEntryCount;
extern struct ChaosActiveEntry *gChaosActiveEntries;
extern u8 gChaosLevelWarped;
extern u8 gChaosBlueStarLastCollected;
extern u8 gChaosImmediateActDeact;
extern u8 gChaosCancelOutLostDuration;
extern u8 gChaosSkipSeverityDifficultyModifier;
extern s32 gChaosLastForcedSeverity;
extern enum ChaosPatchSpecialEvent gChaosLastEventType;
extern enum ChaosDifficulty gChaosDifficulty;
extern enum ChaosGameMode gChaosGameMode;
extern enum ChaosPatchDurationType gChaosForcedDurationType;
extern u32 gChaosForcedDurationMaximum;
extern enum ChaosPatchID gChaosNegativePatchCompare;

// Check whether a particular chaos patch is active. Overall cheaper operation than the function below this one.
u8 chaos_check_if_patch_active(const enum ChaosPatchID patchId);

// Sorts all active patches to make them display in order.
// NOTE: This will destroy any existing patch duration / frame timer pointer references!!!
void chaos_sort_active_patches(void);

// Get patch data for an active patch (if active at all). Return index if a match is found, else return -1.
s32 chaos_find_first_active_patch(const enum ChaosPatchID patchId, struct ChaosActiveEntry **firstFoundMatch);

// Get the number of active instances for a particular patch.
u32 chaos_count_active_instances(const enum ChaosPatchID patchId);

// Get the computed patch duration to apply for a particular patch, based on difficulty and other altering patches.
u32 chaos_calculate_patch_duration(const struct ChaosPatch *patch);

// Deactivate an old chaos patch, based on its current index.
// Be careful when invoking this with stackable patches, as it may cause undesirable behavior if used incorrectly.
// In general it is not recommended to invoke this (externally) with stackable patches that use CHAOS_DURATION_USE_COUNT (since they get combined).
// Additionally a message may be printed, abiding by the format of chaosmsg_print.
void chaos_remove_expired_entry(const s32 patchIndex, const char *msg);

// Deactivate an old chaos patch, based on its current patch ID (deferred for safety when invoked inside of callbacks).
// Be careful when invoking this with stackable patches, as it may cause undesirable behavior if used incorrectly.
// In general it is not recommended to invoke this (externally) with stackable patches that use CHAOS_DURATION_USE_COUNT (since they get combined).
// Additionally a message may be printed, abiding by the format of chaosmsg_print (the print message will NOT be deferred however).
void chaos_remove_expired_entry_deferred(const enum ChaosPatchID patchId, const char *msg);

// Activate a new chaos patch.
void chaos_add_new_entry(const enum ChaosPatchID patchId);

// Decrement all durations for each patch using a star timer (i.e. CHAOS_DURATION_STARS).
// Additionally deconstructs any applicable patches if their duration hits 0, invoking deactivation functions when applicable.
void chaos_decrement_star_timers(enum ChaosStarDecrementType decrementType);

// Decrement the duration of a patch using a star timer or use count (i.e. CHAOS_DURATION_STARS or CHAOS_DURATION_USE_COUNT).
// Does nothing if passed in a type of CHAOS_DURATION_ONCE or CHAOS_DURATION_INFINITE.
// Additionally deconstruct applicable patches if the remaining duration hits 0, invoking deactivation function if applicable.
void chaos_decrement_star_or_use_timer_with_id(enum ChaosPatchID patchId);

// Handles behavior for decrementing consumable patches (i.e. CHAOS_DURATION_USE_COUNT).
// This does NOT invoke a callback for what it should do when the patch is consumed,
// however it will invoke the deactivation callback if remaining uses hits 0.
void chaos_decrement_patch_usage(const enum ChaosPatchID patchId);

// Generates a list of CHAOS_PATCH_MAX_GENERATABLE patch combinations for selection use.
// Optionally a forced severity or event type may be passed in, which may potentially be useful for re-rolling
// patches or for reusing the same generation logic to determine random patch activations.
//     forcedSeverityOverride:
//         -2:                             Determine internally based on patches/history [DEFAULT]
//         -1:                             Explicitly do not force severity on any of the generated patches
//         [0 - CHAOS_PATCH_SEVERITY_MAX]: Force to that severity for all generated patches (this also override difficulty modifiers!)
//     forcedEventOverride:
//         CHAOS_SPECIAL_DO_NOT_FORCE: Determine internally based on patches/history [DEFAULT]
//         CHAOS_SPECIAL_NONE:         Explicitly do not activate an event for patch generation
//         CHAOS_SPECIAL_[OTHER]:      Force patch generation with specified event type
struct ChaosPatchSelection *chaos_roll_for_new_patches(s32 forcedSeverityOverride, enum ChaosPatchSpecialEvent forcedEventOverride);

// Send a new patch selection to be applied.
void chaos_select_patches(struct ChaosPatchSelection *patchSelection);

// Initialize the chaos data, to be run immediately after loading the save file.
// Activate any patches that were previously active in a different session.
void chaos_init(void);

// Invokes the area callback for each chaos patch as soon as Mario enters a new area (excluding via instant warps).
// This function also executes the callback for level changes.
// Neither are invoked if the current course is COURSE_NONE.
void chaos_area_update(void);

// Invokes the area callback for each chaos patch right before/after Mario enters a new area via instant warp.
// Not invoked if the current course is COURSE_NONE.
void chaos_instant_warp_area_update(struct InstantWarp *warp, s32 isPostWarp);

// Invokes a frame update for each active and applicable chaos patch.
// Only updates if current play mode is normal (i.e. not paused) and timestop is inactive.
// Takes place right before objects are updated.
void chaos_frame_update(void);

// Iterates through deferred patch list at the end of the frame and deactivates them accordingly.
void chaos_remove_deferred_patches(void);

// Debug function to verify mutual exclusions are properly handled between all patches at boot time.
// Each match must have an INCOMPAIBLE() entry that includes the alternative patch. If this is undesirable
// for any reason and should only work one way, the patch must use __DEBUG_EXEMPT() to prevent the
// debug assertion from tripping inside of this function.
s32 chaos_precheck_conditional_exclusions(UNUSED s16 arg0, UNUSED s32 arg1);


#include "chaos_patch_shared_vars.h"
#include "chaos_patch_behaviors.h"
#include "chaos_menu_event.h"
#include "chaos_message.h"
