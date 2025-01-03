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
 * Intentionally crash the game if a certain condition is not met.
 */
#define DEBUG_ASSERTIONS
