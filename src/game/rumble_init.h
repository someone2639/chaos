#ifndef RUMBLE_INIT_H
#define RUMBLE_INIT_H

#include <PR/ultratypes.h>

#include "config.h"

#if ENABLE_RUMBLE

extern s32 gRumblePakTimer;

void stop_rumble(void);
void init_rumble_pak_scheduler_queue(void);
void block_until_rumble_pak_free(void);
void release_rumble_pak_control(void);
void queue_rumble_data(s16 time, s16 level);
void func_sh_8024C89C(s16 decay);
u32  is_rumble_finished_and_queue_empty(void);
void reset_rumble_timers(void);
void reset_rumble_timers_2(s32 level);
void func_sh_8024CA04(void);
void cancel_rumble(void);
void create_thread_6(void);
void rumble_thread_update_vi(void);

#endif // ENABLE_RUMBLE

#endif // RUMBLE_INIT_H
