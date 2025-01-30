#pragma once

/*****************************
 * COMPATIBILITY SAFE GUARDS *
 *****************************/

/**
 * NOTE: Only mess with these if you know what you're doing!
 * These are put in place to insure that connected functionality works as intended.
 */


/*****************
 * config_audio.h
 */

#ifndef MAX_SIMULTANEOUS_NOTES_EMULATOR
    #ifdef EXPAND_AUDIO_HEAP
        #define MAX_SIMULTANEOUS_NOTES_EMULATOR 40
    #else
        #define MAX_SIMULTANEOUS_NOTES_EMULATOR 20
    #endif
#endif // MAX_SIMULTANEOUS_NOTES_EMULATOR

#ifndef MAX_SIMULTANEOUS_NOTES_CONSOLE
    #ifdef EXPAND_AUDIO_HEAP
        #define MAX_SIMULTANEOUS_NOTES_CONSOLE 24
    #else
        #define MAX_SIMULTANEOUS_NOTES_CONSOLE 16
    #endif
#endif // MAX_SIMULTANEOUS_NOTES_CONSOLE

#if (MAX_SIMULTANEOUS_NOTES_EMULATOR >= MAX_SIMULTANEOUS_NOTES_CONSOLE)
    #define MAX_SIMULTANEOUS_NOTES MAX_SIMULTANEOUS_NOTES_EMULATOR
#else
    #define MAX_SIMULTANEOUS_NOTES MAX_SIMULTANEOUS_NOTES_CONSOLE
#endif

// Anything higher than 64 will most likely crash on boot. Even if it doesn't, it's still dangerous.
#if (MAX_SIMULTANEOUS_NOTES > 64)
    #undef MAX_SIMULTANEOUS_NOTES
    #define MAX_SIMULTANEOUS_NOTES 64
#elif (MAX_SIMULTANEOUS_NOTES < 0)
    #undef MAX_SIMULTANEOUS_NOTES
    #define MAX_SIMULTANEOUS_NOTES 0
#endif

#if defined(BETTER_REVERB) && !(defined(VERSION_US) || defined(VERSION_JP))
    #undef BETTER_REVERB
#endif


/*****************
 * config_debug.h
 */

#ifdef DISABLE_ALL
    #undef USE_PROFILER
    #undef TEST_LEVEL
    #undef DEBUG_LEVEL_SELECT
    #undef ENABLE_DEBUG_FREE_MOVE
    #undef UNLOCK_ALL
    #undef DEBUG_ASSERTIONS
    #undef DEBUG_PATCH_SELECT_MENU
    #undef DISPLAY_MAX_PATCHES
    #undef L_TO_ADD_PATCH
#endif // DISABLE_ALL

#ifdef USE_PROFILER
    #define PROFILER_DEBUG_INFO
#endif // USE_PROFILER


/*****************
 * config_general.h
 */
 
#ifdef DISABLE_EXIT_COURSE
    #undef ALWAYS_EXIT_COURSE
#endif // DISABLE_EXIT_COURSE
