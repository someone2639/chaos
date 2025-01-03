#include <ultra64.h>
#include <hvqm2dec.h>

#include "buffers.h"
#ifdef HVQM
#include <hvqm/hvqm.h>
#endif
#include "config.h"
#include "audio/data.h"

ALIGNED16 u8 gAudioHeap[DOUBLE_SIZE_ON_64_BIT(AUDIO_HEAP_SIZE)];

ALIGNED16 u8 gIdleThreadStack[THREAD1_STACK];
ALIGNED16 u8 gThread3Stack[THREAD3_STACK];
ALIGNED16 u8 gThread4Stack[THREAD4_STACK];
ALIGNED16 u8 gThread5Stack[THREAD5_STACK];
#if ENABLE_RUMBLE
ALIGNED16 u8 gThread6Stack[THREAD6_STACK];
#endif
// 0x400 bytes
ALIGNED32 u8 gGfxSPTaskStack[SP_DRAM_STACK_SIZE8];
// 0xc00 bytes for f3dex, 0x900 otherwise
ALIGNED32 u8 gGfxSPTaskYieldBuffer[OS_YIELD_DATA_SIZE];
// 0x200 bytes
ALIGNED16 struct SaveBuffer gSaveBuffer;
// 0x190a0 bytes
ALIGNED16 struct GfxPool gGfxPools[2];
