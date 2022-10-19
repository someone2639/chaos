#pragma once

/**
 * Enables widescreen (16:9) support.
 */
// #define WIDE

/**
 * Include the English characters that were missing from US segment2
 * J, Q, V, X, Z, ¨, !, !!, ?, &, %, ., and the beta key.
 * [MAKE SURE TO INCLUDE EU AND JP/SH BASEROMS IN THE REPO TO OBTAIN THE ASSETS]
 * If this is disabled, backup assets will be used.
 */
#define COMPLETE_EN_US_SEGMENT2

/**
 * Disable lives and the lives counter.
 */
#define DISABLE_LIVES

/**
 * If A and Z are pressed on the same frame while running, Mario will long jump instead of ground pound.
 */
// #define EASIER_LONG_JUMPS

/**
 * Disables Exit Course.
 */
// #define DISABLE_EXIT_COURSE

/**
 * Can exit course in all situations (assumes DISABLE_EXIT_COURSE is not being used).
 */
#define ALWAYS_EXIT_COURSE

/**
 * Informs supported emulators to default to gamecube controller inputs
 */
// #define USE_GAMECUBE_CONTROLLER
