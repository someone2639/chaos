#ifndef MACROS_H
#define MACROS_H

#include "platform_info.h"

#ifndef __sgi
#define GLOBAL_ASM(...)
#endif

#if !defined(__sgi) && (!defined(NON_MATCHING) || !defined(AVOID_UB))
// asm-process isn't supported outside of IDO, and undefined behavior causes
// crashes.
#error Matching build is only possible on IDO; please build with NON_MATCHING=1.
#endif

#define ARRAY_COUNT(arr) (s32)(sizeof(arr) / sizeof(arr[0]))

#define GLUE(a, b) a ## b
#define GLUE2(a, b) GLUE(a, b)

// Avoid compiler warnings for unused variables
#ifdef __GNUC__
#define UNUSED __attribute__((unused))
#else
#define UNUSED
#endif

#ifdef __GNUC__
#define FALL_THROUGH __attribute__((fallthrough))
#else
#define FALL_THROUGH
#endif

// Avoid undefined behaviour for non-returning functions
#ifdef __GNUC__
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

// Static assertions
#ifdef __GNUC__
#define STATIC_ASSERT(cond, msg) _Static_assert(cond, msg)
#else
#define STATIC_ASSERT(cond, msg) typedef char GLUE2(static_assertion_failed, __LINE__)[(cond) ? 1 : -1]
#endif

// Align to 8-byte boundary for DMA requirements
#ifdef __GNUC__
#define ALIGNED8 __attribute__((aligned(8)))
#else
#define ALIGNED8
#endif

// Align to 16-byte boundary for audio lib requirements
#ifdef __GNUC__
#define ALIGNED16 __attribute__((aligned(16)))
#else
#define ALIGNED16
#endif

// Align to 32-byte boundary.
#ifdef __GNUC__
#define ALIGNED32 __attribute__((aligned(32)))
#else
#define ALIGNED32
#endif

// Align to 64-byte boundary.
#ifdef __GNUC__
#define ALIGNED64 __attribute__((aligned(64)))
#else
#define ALIGNED64
#endif

#ifndef ALIGN
#define ALIGN(VAL_, ALIGNMENT_) (((VAL_) + ((ALIGNMENT_) - 1)) & ~((ALIGNMENT_) - 1))
#endif

// Round up to the next multiple.
#define ALIGN4(val)  ALIGN((val),  4)
#define ALIGN8(val)  ALIGN((val),  8)
#define ALIGN16(val) ALIGN((val), 16)
#define ALIGN32(val) ALIGN((val), 32)
#define ALIGN64(val) ALIGN((val), 64)

#ifndef NO_SEGMENTED_MEMORY
// convert a virtual address to physical.
#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr) & 0x1FFFFFFF)

// convert a physical address to virtual.
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr) | 0x80000000)

// another way of converting virtual to physical
#define VIRTUAL_TO_PHYSICAL2(addr)  ((u8 *)(addr) - 0x80000000U)
#else
// no conversion needed other than cast
#define VIRTUAL_TO_PHYSICAL(addr)   ((uintptr_t)(addr))
#define PHYSICAL_TO_VIRTUAL(addr)   ((uintptr_t)(addr))
#define VIRTUAL_TO_PHYSICAL2(addr)  ((void *)(addr))
#endif

#define FORCE_CRASH { *(volatile s8*)0 = 0; }

enum VIModes {
    MODE_NTSC,
    MODE_MPAL,
    MODE_PAL,
};

#endif // MACROS_H
