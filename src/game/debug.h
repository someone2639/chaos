#ifndef DEBUG_H
#define DEBUG_H

#include <PR/ultratypes.h>

enum DebugPage {
    DEBUG_PAGE_OBJECTINFO,       // 0: objectinfo
    DEBUG_PAGE_CHECKSURFACEINFO, // 1: checkinfo/surfaceinfo
    DEBUG_PAGE_MAPINFO,          // 2: mapinfo
    DEBUG_PAGE_STAGEINFO,        // 3: stageinfo
    DEBUG_PAGE_EFFECTINFO,       // 4: effectinfo
    DEBUG_PAGE_ENEMYINFO         // 5: enemyinfo
};

s64 get_current_clock(void);
s64 get_clock_difference(UNUSED s64 cycles);
void set_text_array_x_y(s32 xOffset, s32 yOffset);
void print_debug_top_down_objectinfo(const char *str, s32 number);
void print_debug_top_down_mapinfo(const char * str, s32 number);
void print_debug_bottom_up(const char *str, s32 number);
void debug_unknown_level_select_check(void);
void reset_debug_objectinfo(void);
void stub_debug_5(void);
void try_print_debug_mario_object_info(void);
void try_do_mario_debug_object_spawn(void);
void try_print_debug_mario_level_info(void);

extern volatile char *__n64Assert_Filename;
extern volatile u32   __n64Assert_LineNum;
extern volatile char *__n64Assert_Message;
extern void __n64Assert(char *fileName, u32 lineNum, char *message);
extern void __n64DelayedAssertCheck(void);

extern char gAssertionStr[512];

/**
 * Will always cause a crash with your message of choice
 */
#define error(message) __n64Assert(__FILE__, __LINE__, (message))

/**
 * Will always cause a crash if cond is not true (handle with care)
 */
#define aggress(cond, message) do {\
    if ((cond) == FALSE) { \
        error(message); \
    } \
} while (0);

#define aggress_args(cond, ...) do {\
    if ((cond) == FALSE) { \
        sprintf(gAssertionStr, __VA_ARGS__); \
        error(gAssertionStr); \
    } \
} while (0);

/**
 * Will cause a crash if cond is not true, and DEBUG is defined (allows for quick removal of littered asserts)
 */
#if defined(DEBUG) || defined(DEBUG_ASSERTIONS)

#define assert(cond, message) do {\
    if ((cond) == FALSE) { \
        error(message); \
    } \
} while (0);

#define assert_args(cond, ...) do {\
    if ((cond) == FALSE) { \
        sprintf(gAssertionStr, __VA_ARGS__); \
        error(gAssertionStr); \
    } \
} while (0);

#else
#define assert(cond, message)
#define assert_args(cond, ...)
#endif

#endif // DEBUG_H
