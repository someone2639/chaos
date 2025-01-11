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
#define DEBUG_LEVEL_SELECT

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
 * Because what if I want to listen to other music while working on this without having to hear BoB music?
 */
// #define MUTE_MUSIC_PLAYERS

/**
 * Allows you to test the patch selection screen
 * Hold Z and press R while in PLAY_MODE_NORMAL
 * Also fills the menu with dummy data for testing
*/
// #define DEBUG_PATCH_SELECT_MENU
