#ifndef __PROFILING_H__
#define __PROFILING_H__

#include <ultra64.h>
#include "macros.h"
#include "config/config_debug.h"
#include "config/config_safeguards.h"

#if defined(USE_PROFILER) && defined(PUPPYPRINT_DEBUG)
/**
 * Toggle this define to enable verbose audio profiling with Pupprprint Debug.
*/
#define AUDIO_PROFILING
#endif

#define OS_GET_COUNT_INLINE(x) asm volatile("mfc0 %0, $9" : "=r"(x): )

#define PROFILING_BUFFER_SIZE 64

#define AUDIO_SUBSET_ENTRIES \
    PROFILER_TIME_SUB_AUDIO_START, \
    PROFILER_TIME_SUB_AUDIO_SEQUENCES = PROFILER_TIME_SUB_AUDIO_START, \
    PROFILER_TIME_SUB_AUDIO_SEQUENCES_SCRIPT, \
    PROFILER_TIME_SUB_AUDIO_SEQUENCES_RECLAIM, \
    PROFILER_TIME_SUB_AUDIO_SEQUENCES_PROCESSING, \
    PROFILER_TIME_SUB_AUDIO_SYNTHESIS, \
    PROFILER_TIME_SUB_AUDIO_SYNTHESIS_PROCESSING, \
    PROFILER_TIME_SUB_AUDIO_SYNTHESIS_ENVELOPE_REVERB, \
    PROFILER_TIME_SUB_AUDIO_SYNTHESIS_DMA, \
    PROFILER_TIME_SUB_AUDIO_UPDATE, \
    PROFILER_TIME_SUB_AUDIO_END

enum ProfilerTime {
    PROFILER_TIME_FPS,
    PROFILER_TIME_CONTROLLERS,
    PROFILER_TIME_SPAWNER,
    PROFILER_TIME_DYNAMIC,
    PROFILER_TIME_BEHAVIOR_BEFORE_MARIO,
    PROFILER_TIME_MARIO,
    PROFILER_TIME_BEHAVIOR_AFTER_MARIO,
    PROFILER_TIME_GFX,
#ifdef AUDIO_PROFILING
    AUDIO_SUBSET_ENTRIES,

    PROFILER_TIME_AUDIO = PROFILER_TIME_SUB_AUDIO_END,
#else
    PROFILER_TIME_AUDIO,
#endif
    PROFILER_TIME_TOTAL,
    PROFILER_TIME_RSP_GFX,
    PROFILER_TIME_RSP_AUDIO,
    PROFILER_TIME_TMEM,
    PROFILER_TIME_PIPE,
    PROFILER_TIME_CMD,
    PROFILER_TIME_COUNT,
};

enum ProfilerRSPTime {
    PROFILER_RSP_GFX,
    PROFILER_RSP_AUDIO,
    PROFILER_RSP_COUNT
};

#ifdef USE_PROFILER
void profiler_update(enum ProfilerTime which);
void profiler_print_times();
void profiler_frame_setup();
void profiler_rsp_started(enum ProfilerRSPTime which);
void profiler_rsp_completed(enum ProfilerRSPTime which);
void profiler_rsp_resumed();
void profiler_audio_started();
void profiler_audio_completed();
// See profiling.c to see why profiler_rsp_yielded isn't its own function
static inline __attribute__((always_inline)) void profiler_rsp_yielded() {
    profiler_rsp_resumed();
}
#else
#define profiler_update(which)
#define profiler_print_times()
#define profiler_frame_setup()
#define profiler_rsp_started(which)
#define profiler_rsp_completed(which)
#define profiler_rsp_resumed()
#define profiler_audio_started()
#define profiler_audio_completed()
#define profiler_rsp_yielded()
#endif

#ifdef AUDIO_PROFILING
#define AUDIO_SUBSET_SIZE PROFILER_TIME_SUB_AUDIO_END - PROFILER_TIME_SUB_AUDIO_START
extern u32 audio_subset_starts[AUDIO_SUBSET_SIZE];
extern u32 audio_subset_tallies[AUDIO_SUBSET_SIZE];

static ALWAYS_INLINE void profiler_audio_subset_switch_func(enum ProfilerTime complete, enum ProfilerTime start) {
    u32 time;
    OS_GET_COUNT_INLINE(time);

    audio_subset_tallies[complete] += time - audio_subset_starts[complete];
    audio_subset_starts[start] = time;
}

static ALWAYS_INLINE void profiler_audio_subset_complete_and_switch_func(enum ProfilerTime complete1, enum ProfilerTime complete2, enum ProfilerTime start) {
    u32 time;
    OS_GET_COUNT_INLINE(time);

    audio_subset_tallies[complete1] += time - audio_subset_starts[complete1];
    audio_subset_tallies[complete2] += time - audio_subset_starts[complete2];
    audio_subset_starts[start] = time;
}

static ALWAYS_INLINE void profiler_audio_subset_start_shared_func(enum ProfilerTime first, enum ProfilerTime new) {
    audio_subset_starts[new] = audio_subset_starts[first];
}

static ALWAYS_INLINE void profiler_audio_subset_start_func(enum ProfilerTime index) {
    OS_GET_COUNT_INLINE(audio_subset_starts[index]);
}

static ALWAYS_INLINE void profiler_audio_subset_complete_func(enum ProfilerTime index) {
    u32 time;
    OS_GET_COUNT_INLINE(time);

    audio_subset_tallies[index] += time - audio_subset_starts[index];
}

#define AUDIO_PROFILER_SWITCH(complete, begin) profiler_audio_subset_switch_func(complete - PROFILER_TIME_SUB_AUDIO_START, begin - PROFILER_TIME_SUB_AUDIO_START)
#define AUDIO_PROFILER_COMPLETE_AND_SWITCH(complete1, complete2, begin) profiler_audio_subset_complete_and_switch_func(complete1 - PROFILER_TIME_SUB_AUDIO_START, \
    complete2 - PROFILER_TIME_SUB_AUDIO_START, begin - PROFILER_TIME_SUB_AUDIO_START)
#define AUDIO_PROFILER_START_SHARED(first, new) profiler_audio_subset_start_shared_func(first - PROFILER_TIME_SUB_AUDIO_START, new - PROFILER_TIME_SUB_AUDIO_START)

// These two are unused by the default audio profiler; left in for cases of manual profiling of smaller functions as needed
#define AUDIO_PROFILER_START(which) profiler_audio_subset_start_func(which - PROFILER_TIME_SUB_AUDIO_START)
#define AUDIO_PROFILER_COMPLETE(which) profiler_audio_subset_complete_func(which - PROFILER_TIME_SUB_AUDIO_START)
#else // AUDIO_PROFILING
enum ProfilerTimeAudioUnused {
    AUDIO_SUBSET_ENTRIES
};
#define AUDIO_PROFILER_SWITCH(complete, begin)
#define AUDIO_PROFILER_COMPLETE_AND_SWITCH(complete1, complete2, begin)
#define AUDIO_PROFILER_START_SHARED(first, new)

// These two are unused by the default audio profiler; left in for cases of manual profiling of smaller functions as needed
#define AUDIO_PROFILER_START(which)
#define AUDIO_PROFILER_COMPLETE(which)
#endif // AUDIO_PROFILING

#endif
