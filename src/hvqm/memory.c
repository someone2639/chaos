#include <ultra64.h>
#include <HVQM2File.h>
#include "macros.h"
#include "system.h"
#include "game/debug.h"

OSPiHandle *cartrom_hd;

#define DMA_MSG_SIZE 1
OSIoMesg dmaIOMesg;
OSIoMesg audioIOMesg;

static OSMesgQueue dmaMessageQ;
static OSMesg dmaMessages[DMA_MSG_SIZE];
static OSMesgQueue audDMAMessageQ;
static OSMesg audDMAMessages[DMA_MSG_SIZE];


void init_dma() {
    osCreateMesgQueue(&dmaMessageQ, dmaMessages, DMA_MSG_SIZE);
    osCreateMesgQueue(&audDMAMessageQ, audDMAMessages, DMA_MSG_SIZE);
    cartrom_hd = osCartRomInit();

    dmaIOMesg.hdr.pri = OS_MESG_PRI_NORMAL;
    dmaIOMesg.hdr.retQueue = &dmaMessageQ;

    audioIOMesg.hdr.pri = OS_MESG_PRI_HIGH;
    audioIOMesg.hdr.retQueue = &audDMAMessageQ;
}

s32 skip_record(s32 record_size, OSIoMesg *mb, void **streamp, void *endDataAddr) {
    ALIGNED16 union {
        ALIGNED16 u8 _fill[ALIGN16(sizeof(HVQM2Record))];
        ALIGNED16 HVQM2Record record;
    } hdr;

    *streamp += record_size;
    if (*streamp >= endDataAddr) {
        return TRUE;
    }

    dma_copy(&hdr.record, *streamp, sizeof(HVQM2Record), mb);
    if (hdr.record.type != HVQM2_AUDIO && hdr.record.type != HVQM2_VIDEO) {
        return TRUE;
    }
    if (*streamp + sizeof(HVQM2Record) + hdr.record.size > endDataAddr) {
        return TRUE;
    }

    return FALSE;
}

s32 get_record(HVQM2Record *headerbuf, u16 type, void **stream, void *endDataAddr) {
    u16 record_type = 0x2639;
    u32 record_size;
    OSIoMesg *mb;

    mb = (type == HVQM2_AUDIO) ? &audioIOMesg : &dmaIOMesg;
    while (TRUE) {
        dma_copy(headerbuf, *stream, sizeof(HVQM2Record), mb);
        *stream += sizeof(HVQM2Record);
        record_type = headerbuf->type;
        if (record_type != HVQM2_AUDIO && record_type != HVQM2_VIDEO) {
            return -1;
        }
        record_size = headerbuf->size;
        if (record_size == 0xFFFFFFFF) {
            return -1;
        }
        if (record_type == HVQM2_VIDEO) { 
            if (record_size > HVQ_DATASIZE_MAX) {
                assert_args(FALSE, "HVQM get_record:\nVideo record too large!");
                return -1;
            }
            if (*stream + record_size > endDataAddr) {
                assert_args(FALSE, "HVQM get_record:\nVideo record out of bounds!");
                return -1;
            }
        } else if (record_type == HVQM2_AUDIO) {
            if (record_size > AUDIO_RECORD_SIZE_MAX) {
                assert_args(FALSE, "HVQM get_record:\nAudio record too large!");
                return -1;
            }
            if (*stream + record_size > endDataAddr) {
                assert_args(FALSE, "HVQM get_record:\nAudio record out of bounds!");
                return -1;
            }
        }

        if (record_type == type) {
            break;
        }

        if (skip_record(record_size, mb, stream, endDataAddr)) {
            return -1;
        }
    }

    return record_size;
}

// void get_record2(HVQM2Record *headerbuf, u16 type, void *bodybuf, void **stream) {
//     u16 record_type = 0x2639;
//     u32 record_size;
//     OSIoMesg *mb;

//     mb = (type == HVQM2_AUDIO) ? &audioIOMesg : &dmaIOMesg;
//     for (;;) {
//         dma_copy(headerbuf, *stream, sizeof(HVQM2Record), mb);
//         *stream += sizeof(HVQM2Record);
//         record_type = headerbuf->type;
//         record_size = headerbuf->size;
//         if (record_type == type)
//             break;
//         skip_record(record_size, stream);
//     }

//     if (record_size > 0) {
//         dma_copy(bodybuf, *stream, record_size, mb);
//         *stream += record_size;
//     }
// }

s32 load_record(s32 record_size, u16 type, void *bodybuf, void **streamp, void *endDataAddr) {
    OSIoMesg *mb = (type == HVQM2_AUDIO) ? &audioIOMesg : &dmaIOMesg;
    if (record_size < 0) {
        return TRUE;
    }
    if (record_size > 0) {
        dma_copy(bodybuf, *streamp, record_size, mb);
    }

    return skip_record(record_size, mb, streamp, endDataAddr);
}

void dma_copy(void *dest, void *src, u32 len, OSIoMesg *msg) {
    ALIGNED16 static OSMesg receivedMsg;
    len = ALIGN16(len);

    aggress_args(((u32) src  & (0x02 - 1)) == 0, "dma_read:\nMisaligned DMA start address: 0x%08X", (u32) src );
    aggress_args(((u32) dest & (0x10 - 1)) == 0, "dma_read:\nMisaligned DMA dest address: 0x%08X",  (u32) dest);

    osInvalDCache(dest, len);
    while (len != 0) {
        u32 copySize = (len >= 0x1000) ? 0x1000 : len;

        osPiStartDma(msg, OS_MESG_PRI_NORMAL, OS_READ, (uintptr_t) src, dest, copySize, msg->hdr.retQueue);
        osRecvMesg(msg->hdr.retQueue, &receivedMsg, OS_MESG_BLOCK);

        dest += copySize;
        src += copySize;
        len -= copySize;
    }
}
