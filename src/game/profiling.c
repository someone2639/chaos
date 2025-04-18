#include <ultra64.h>
#include <PR/os_internal_reg.h>
#include "sm64.h"
#include "game_init.h"
#include "level_update.h"
#include "camera.h"

#include "profiling.h"
#include "fasttext.h"

#ifdef USE_PROFILER

#define RDP_CYCLE_CONV(x) ((10 * (x)) / 625) // 62.5 million cycles per frame

typedef struct {
    u32 counts[PROFILING_BUFFER_SIZE];
    u32 total;
} ProfileTimeData;

ProfileTimeData all_profiling_data[PROFILER_TIME_COUNT];

int profile_buffer_index = -1;
int rsp_buffer_indices[PROFILER_RSP_COUNT];
// Holds either the start time if the task is running, or the amount of time the task has run for so far if yielded
u32 rsp_pending_times[PROFILER_RSP_COUNT];
u32 prev_start;
u32 start;
u32 prev_time;
u32 audio_start;
u32 audio_buffer_index;
u32 preempted_time;

#ifdef AUDIO_PROFILING
u32 audio_subset_starts[AUDIO_SUBSET_SIZE];
u32 audio_subset_tallies[AUDIO_SUBSET_SIZE];
#endif

static void buffer_update(ProfileTimeData* data, u32 new, int buffer_index) {
    u32 old = data->counts[buffer_index];
    data->total -= old;
    data->total += new;
    data->counts[buffer_index] = new;
}

void profiler_update(enum ProfilerTime which) {
    u32 cur_time = osGetCount();
    u32 diff;
    ProfileTimeData* cur_data = &all_profiling_data[which];

    diff = cur_time - prev_time;

    u32 saved = __osDisableInt();
    u32 cur_preempted_time = preempted_time;
    preempted_time = 0;
    __osRestoreInt(saved);
    if (cur_preempted_time > 0) {
        diff -= cur_preempted_time;
        start += cur_preempted_time;
    }
    
    buffer_update(cur_data, diff, profile_buffer_index);
    prev_time = cur_time;
}

void profiler_rsp_started(enum ProfilerRSPTime which) {
    rsp_pending_times[which] = osGetCount();
}

void profiler_rsp_completed(enum ProfilerRSPTime which) {
    ProfileTimeData* cur_data = &all_profiling_data[PROFILER_TIME_RSP_GFX + which];
    int cur_index = rsp_buffer_indices[which];
    u32 time = osGetCount() - rsp_pending_times[which];
    rsp_pending_times[which] = 0;

    buffer_update(cur_data, time, cur_index);
    cur_index++;
    if (cur_index >= PROFILING_BUFFER_SIZE) {
        cur_index = 0;
    }
    rsp_buffer_indices[which] = cur_index;
}

void profiler_rsp_resumed() {
    rsp_pending_times[PROFILER_RSP_GFX] = osGetCount() - rsp_pending_times[PROFILER_RSP_GFX];
}

// This ends up being the same math as resumed, so we just use resumed for both
// void profiler_rsp_yielded() {
//     rsp_pending_times[PROFILER_RSP_GFX] = osGetCount() - rsp_pending_times[PROFILER_RSP_GFX];
// }

void profiler_audio_started() {
    audio_start = osGetCount();
    
#ifdef AUDIO_PROFILING
    for (s32 i = 0; i < AUDIO_SUBSET_SIZE; i++) {
        audio_subset_tallies[i] = 0;
    }

    audio_subset_starts[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START] = audio_start;
#endif
}

void profiler_audio_completed() {
    ProfileTimeData* cur_data = &all_profiling_data[PROFILER_TIME_AUDIO];
    u32 time = osGetCount() - audio_start;
    u32 cur_index = audio_buffer_index;

    preempted_time = time;
    buffer_update(cur_data, time, cur_index);

#ifdef AUDIO_PROFILING
    audio_subset_tallies[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START] += time - audio_subset_starts[PROFILER_TIME_SUB_AUDIO_UPDATE - PROFILER_TIME_SUB_AUDIO_START];

    for (s32 i = 0; i < AUDIO_SUBSET_SIZE; i++) {
        cur_data = &all_profiling_data[i + PROFILER_TIME_SUB_AUDIO_START];
        buffer_update(cur_data, audio_subset_tallies[i], cur_index);
    }
#endif

    cur_index++;
    if (cur_index >= PROFILING_BUFFER_SIZE) {
        cur_index = 0;
    }

    audio_buffer_index = cur_index;
}

static void update_fps_timer() {
    u32 diff = start - prev_start;

    buffer_update(&all_profiling_data[PROFILER_TIME_FPS], diff, profile_buffer_index);
    prev_start = start;
}

static void update_total_timer() {
    u32 saved = __osDisableInt();
    u32 cur_preempted_time = preempted_time;
    preempted_time = 0;
    __osRestoreInt(saved);

    prev_time = start + cur_preempted_time;
    profiler_update(PROFILER_TIME_TOTAL);
}

static void update_rdp_timers() {
    u32 tmem = IO_READ(DPC_TMEM_REG);
    u32 cmd =  IO_READ(DPC_BUFBUSY_REG);
    u32 pipe = IO_READ(DPC_PIPEBUSY_REG);
    
    if (gGlobalTimer > 5) {
        IO_WRITE(DPC_STATUS_REG, (DPC_CLR_CLOCK_CTR | DPC_CLR_CMD_CTR | DPC_CLR_PIPE_CTR | DPC_CLR_TMEM_CTR));
    }

    buffer_update(&all_profiling_data[PROFILER_TIME_TMEM], tmem, profile_buffer_index);
    buffer_update(&all_profiling_data[PROFILER_TIME_CMD], cmd, profile_buffer_index);
    buffer_update(&all_profiling_data[PROFILER_TIME_PIPE], pipe, profile_buffer_index);
}

float profiler_get_fps() {
    return (1000000.0f * PROFILING_BUFFER_SIZE) / (OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_FPS].total));
}

u32 profiler_get_cpu_cycles() {
    u32 cpu_normal_time = all_profiling_data[PROFILER_TIME_TOTAL].total / PROFILING_BUFFER_SIZE;
    u32 cpu_audio_time = all_profiling_data[PROFILER_TIME_AUDIO].total / PROFILING_BUFFER_SIZE;
    return cpu_normal_time + cpu_audio_time * 2;
}

u32 profiler_get_rsp_cycles() {
    u32 rsp_graphics_time = all_profiling_data[PROFILER_TIME_RSP_GFX].total / PROFILING_BUFFER_SIZE;
    u32 rsp_audio_time = all_profiling_data[PROFILER_TIME_RSP_AUDIO].total / PROFILING_BUFFER_SIZE;
    return rsp_graphics_time + rsp_audio_time;
}

u32 profiler_get_rdp_cycles() {
    u32 rdp_pipe_cycles = all_profiling_data[PROFILER_TIME_PIPE].total;
    u32 rdp_tmem_cycles = all_profiling_data[PROFILER_TIME_TMEM].total;
    u32 rdp_cmd_cycles = all_profiling_data[PROFILER_TIME_CMD].total;

    u32 rdp_max_cycles = MAX(MAX(rdp_pipe_cycles, rdp_tmem_cycles), rdp_cmd_cycles);

    return rdp_max_cycles / PROFILING_BUFFER_SIZE;
}

u32 profiler_get_cpu_microseconds() {
    u32 cpu_normal_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_TOTAL].total / PROFILING_BUFFER_SIZE);
    u32 cpu_audio_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_AUDIO].total / PROFILING_BUFFER_SIZE);
    return cpu_normal_time + cpu_audio_time * 2;
}

u32 profiler_get_rsp_microseconds() {
    u32 rsp_graphics_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_RSP_GFX].total / PROFILING_BUFFER_SIZE);
    u32 rsp_audio_time = OS_CYCLES_TO_USEC(all_profiling_data[PROFILER_TIME_RSP_AUDIO].total / PROFILING_BUFFER_SIZE);
    return rsp_graphics_time + rsp_audio_time;
}

u32 profiler_get_rdp_microseconds() {
    u32 rdp_pipe_cycles = all_profiling_data[PROFILER_TIME_PIPE].total;
    u32 rdp_tmem_cycles = all_profiling_data[PROFILER_TIME_TMEM].total;
    u32 rdp_cmd_cycles = all_profiling_data[PROFILER_TIME_CMD].total;

    u32 rdp_max_cycles = MAX(MAX(rdp_pipe_cycles, rdp_tmem_cycles), rdp_cmd_cycles);

    return RDP_CYCLE_CONV(rdp_max_cycles / PROFILING_BUFFER_SIZE);
}

extern u8 *sPoolEnd;
void profiler_print_times() {
    u32 microseconds[PROFILER_TIME_COUNT];
    char text_buffer[196];
    u32 totalMem = (u32) VIRTUAL_TO_PHYSICAL2(sPoolEnd);
    u32 memUsed = totalMem - main_pool_available();

    update_fps_timer();
    update_total_timer();
    update_rdp_timers();

    static u8 show_profiler = 0;
    if ((gPlayer1Controller->buttonDown & U_JPAD && gPlayer1Controller->buttonPressed & L_TRIG)
        || (gPlayer1Controller->buttonDown & L_TRIG && gPlayer1Controller->buttonPressed & U_JPAD)) {
        show_profiler ^= 1;
    }

    if (show_profiler) {
        for (int i = 0; i < PROFILER_TIME_COUNT; i++) {
            if (i < PROFILER_TIME_TMEM) {
                microseconds[i] = OS_CYCLES_TO_USEC(all_profiling_data[i].total / PROFILING_BUFFER_SIZE);
            } else {
                microseconds[i] = RDP_CYCLE_CONV(all_profiling_data[i].total / PROFILING_BUFFER_SIZE);
            }
        }

        // audio time is removed from the main thread profiling, so add it back here
        u32 total_cpu = microseconds[PROFILER_TIME_TOTAL] + microseconds[PROFILER_TIME_AUDIO] * 2;
        u32 total_rsp = microseconds[PROFILER_TIME_RSP_GFX] + microseconds[PROFILER_TIME_RSP_AUDIO] * 2;
        u32 max_rdp = MAX(MAX(microseconds[PROFILER_TIME_TMEM], microseconds[PROFILER_TIME_CMD]), microseconds[PROFILER_TIME_PIPE]);

        sprintf(text_buffer,
            "FPS: %5.2f\n"
            "\n"
            "CPU\t\t%d (%d%%)\n"
            " Input\t\t%d\n"
            " Dynamic\t\t%d\n"
            " Mario\t\t\t%d\n"
            " Behavior\t%d\n"
            " Graph\t\t%d\n"
            " Audio\t\t\t%d\n"
            "\n"
            "RDP\t\t%d (%d%%)\n"
            " Tmem\t\t\t%d\n"
            " Cmd\t\t\t%d\n"
            " Pipe\t\t\t%d\n"
            "\n"
            "RSP\t\t%d (%d%%)\n"
            " Gfx\t\t\t%d\n"
            " Audio\t\t\t%d\n",
            1000000.0f / microseconds[PROFILER_TIME_FPS],
            total_cpu, total_cpu / 333, 
            microseconds[PROFILER_TIME_CONTROLLERS],
            microseconds[PROFILER_TIME_DYNAMIC],
            microseconds[PROFILER_TIME_MARIO],
            microseconds[PROFILER_TIME_BEHAVIOR_BEFORE_MARIO] + microseconds[PROFILER_TIME_BEHAVIOR_AFTER_MARIO],
            microseconds[PROFILER_TIME_GFX],
            microseconds[PROFILER_TIME_AUDIO] * 2, // audio is 60Hz, so double the average
            max_rdp, max_rdp / 333,
            microseconds[PROFILER_TIME_TMEM],
            microseconds[PROFILER_TIME_CMD],
            microseconds[PROFILER_TIME_PIPE],
            total_rsp, total_rsp / 333,
            microseconds[PROFILER_TIME_RSP_GFX],
            microseconds[PROFILER_TIME_RSP_AUDIO] * 2
        );

        fasttext_setup_textrect_rendering(FT_FONT_OUTLINE);
        fasttext_draw_texrect(10, 10, text_buffer, FT_FLAG_ALIGN_LEFT, 255, 255, 255, 255);

#ifdef PROFILER_DEBUG_INFO
        text_buffer[0] = 0; // Effectively clear buffer

        // Very little point printing useless info if Mario doesn't even exist.
        if (gMarioState->marioObj) {
            sprintf(text_buffer,
                "Mario Pos\n"
                " X\t\t%d\n"
                " Y\t\t%d\n"
                " Z\t\t%d\n"
                "\n"
                "SPD\t%d\n"
                "ANG\t0x%04X\n"
                "ACT\t0x%04X\n"
                "\n",
                (s32) gMarioState->pos[0],
                (s32) gMarioState->pos[1],
                (s32) gMarioState->pos[2],
                (s32) gMarioState->forwardVel,
                (u16) gMarioState->faceAngle[1],
                (u16) (gMarioState->action & ACT_ID_MASK)
            );
        }

        // Same for the camera, especially so because this will crash otherwise.
        if (gCamera) {
            sprintf(text_buffer,
                "%s"
                "Camera\n"
                " X\t\t%d\n"
                " Y\t\t%d\n"
                " Z\t\t%d\n"
                " YAW\t0x%04X\n",
                text_buffer,
                (s32) (gCamera->pos[0]),
                (s32) (gCamera->pos[1]),
                (s32) (gCamera->pos[2]),
                (u16) (gCamera->yaw)
            );
        }

        fasttext_draw_texrect(235, 34, text_buffer, FT_FLAG_ALIGN_LEFT, 255, 255, 255, 255);
#endif

        // Print memory usage
        sprintf(text_buffer, "RAM: 0x%06X / 0x%06X", memUsed, totalMem);
        fasttext_draw_texrect(112, 214, text_buffer, FT_FLAG_ALIGN_LEFT, 255, 255, 255, 255);

        fasttext_finished_rendering();
    }
}

void profiler_frame_setup() {
    profile_buffer_index++;
    preempted_time = 0;

    if (profile_buffer_index >= PROFILING_BUFFER_SIZE) {
        profile_buffer_index = 0;
    }

    prev_time = start = osGetCount();
}

#endif
