#pragma once

/******************
 * DEBUG SETTINGS *
 ******************/

/**
 * Disables all debug options
 */
// #define DISABLE_ALL


/* --------------------------------------------------------------------------------------------------------------------------- */


/**
 * Enables a comprehensive standalone profiler. Press D-Pad Up + L to toggle the profiler.
 */
#define USE_PROFILER

/**
 * -- TEST LEVEL --
 * Set a test level in order to boot straight into said level.
 */
// #define TEST_LEVEL LEVEL_WF

/**
 * Enables debug level select. Hold L while the game boots to enter level selection menu.
 */
// #define DEBUG_LEVEL_SELECT

/**
 * Enables debug free move (D-pad up to enter, A to exit).
 */
#define ENABLE_DEBUG_FREE_MOVE

/**
 * Opens all courses and doors. Used for debugging purposes to unlock all content.
 */
// #define UNLOCK_ALL

/**
 * Intentionally crash the game if a certain condition is not met.
 */
#define DEBUG_ASSERTIONS

/**
 * Allows you to test the patch selection screen
 * Hold Z and press R while in PLAY_MODE_NORMAL
 * Also fills the menu with dummy data for testing
*/
#define DEBUG_PATCH_SELECT_MENU

/**
 * Start with all 4 chaos patches to display by default.
 * NOTE: Toggling this on an existing save file may trigger an assertion if patch counts have been meddled with already!
 */
#define DISPLAY_MAX_PATCHES

/**
 * Activate a patch by pressing L on the controller. Will not apply if it's already active.
 */
// #define L_TO_ADD_PATCH CHAOS_PATCH_NONE_POSITIVE

/**
 * Force patch cards to show up for specifiable positive and negative effects.
 * The positive patch will always show up on card 0, and the negative on card 1.
 * These patches will only show up if they pass their conditional checks and aren't unstackable duplicates.
 */
// #define CHAOS_FORCED_POSITIVE_CARD CHAOS_PATCH_NONE_POSITIVE
// #define CHAOS_FORCED_NEGATIVE_CARD CHAOS_PATCH_NONE_NEGATIVE
