#ifndef MEMORY_H
#define MEMORY_H

#include <PR/ultratypes.h>

#include "types.h"

#define MEMORY_POOL_LEFT  0
#define MEMORY_POOL_RIGHT 1

struct AllocOnlyPool {
    s32 totalSpace;
    s32 usedSpace;
    u8 *startPtr;
    u8 *freePtr;
};

struct MemoryPool;

struct OffsetSizePair {
    u32 offset;
    u32 size;
};

struct DmaTable {
    u32 count;
    u8 *srcAddr;
    struct OffsetSizePair anim[1]; // dynamic size
};

struct DmaHandlerList {
    struct DmaTable *dmaTable;
    void *currentAddr;
    void *bufTarget;
};

#ifndef INCLUDED_FROM_MEMORY_C
// Declaring this variable extern puts it in the wrong place in the bss order
// when this file is included from memory.c (first instead of last). Hence,
// ifdef hack. It was very likely subject to bss reordering originally.
extern struct MemoryPool *gEffectsMemoryPool;
#endif

uintptr_t set_segment_base_addr(s32 segment, void *addr);
void *get_segment_base_addr(s32 segment);
void *segmented_to_virtual(const void *addr);
void *virtual_to_segmented(u32 segment, const void *addr);
void move_segment_table_to_dmem(void);

void main_pool_init(void *start, void *end);
void *main_pool_alloc(u32 size, u32 side);
u32 main_pool_free(void *addr);
void *main_pool_realloc(void *addr, u32 size);
u32 main_pool_available(void);
u32 main_pool_push_state(void);
u32 main_pool_pop_state(void);

void dma_read(u8 *dest, u8 *srcStart, u8 *srcEnd);

#ifndef NO_SEGMENTED_MEMORY
void *load_segment(s32 segment, u8 *srcStart, u8 *srcEnd, u32 side);
void *load_to_fixed_pool_addr(u8 *destAddr, u8 *srcStart, u8 *srcEnd);
void *load_segment_decompress(s32 segment, u8 *srcStart, u8 *srcEnd);
void load_engine_code_segment(void);
#else
#define load_segment(...)
#define load_to_fixed_pool_addr(...)
#define load_segment_decompress(...)
#define load_engine_code_segment(...)
#endif

struct AllocOnlyPool *alloc_only_pool_init(u32 size, u32 side);
void *alloc_only_pool_alloc(struct AllocOnlyPool *pool, s32 size);
struct AllocOnlyPool *alloc_only_pool_resize(struct AllocOnlyPool *pool, u32 size);

struct MemoryPool *mem_pool_init(u32 size, u32 side);
void *mem_pool_alloc(struct MemoryPool *pool, u32 size);
void mem_pool_free(struct MemoryPool *pool, void *addr);

void *alloc_display_list(u32 size);
void setup_dma_table_list(struct DmaHandlerList *list, void *srcAddr, void *buffer);
s32 load_patchable_table(struct DmaHandlerList *list, s32 index);

#endif // MEMORY_H
