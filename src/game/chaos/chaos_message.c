#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

#include "string.h"
#include "buffers/buffers.h"
#include "engine/behavior_script.h"
#include "engine/math_util.h"
#include "game/debug.h"
#include "game/fasttext.h"
#include "game/object_list_processor.h"

#define MSGBUF_COUNT 16
#define MSGBUF_SIZE  0x400

#define BASE_X 16
#define BASE_Y 28
#define MAX_WIDTH ((SCREEN_CENTER_X - BASE_X) + (SCREEN_WIDTH / 5))

#define TRANS_TIME 8
#define WAIT_TIME 4
#define DISPLAY_TIME ((4 * 30) + TRANS_TIME)
#define FADE_TIME 30

#define MESSAGE_MARGIN 5

#define BG_TRANSPARENCY 0.667f

#define FT_FONT FT_FONT_SMALL_BOLD

enum ChaosMessageStatus {
    CHSMSG_INACTIVE = 0,
    CHSMSG_WAITING,
    CHSMSG_DISPLAYING,
};

struct ChaosMessageParams {
    u16 animFrame;
    s16 yOffset;
    s16 lineHeight;
    enum ChaosMessageStatus status;
    char *str;
};

static s32 chsStrIter = 0;
static u32 waitFrames = 0;
static struct ChaosMessageParams chsStrParams[MSGBUF_COUNT];
static char chsStrBufs[MSGBUF_COUNT][MSGBUF_SIZE];

void chaosmsg_print(enum ChaosPatchID patchId, const char *str) {
    s32 index = chsStrIter;
    s32 strlen;
    s32 lineCount;
    struct ChaosMessageParams *params = &chsStrParams[index];
    if (!str) {
        assert(FALSE, "chaosmsg_print:\nAttempted to pass NULL string!");
        return;
    }

    params->status = CHSMSG_WAITING;
    params->animFrame = 0;
    params->yOffset = BASE_Y - (3 * MESSAGE_MARGIN);
    params->str = chsStrBufs[index];

    if (patchId < CHAOS_PATCH_COUNT && patchId != CHAOS_PATCH_NONE) {
        const struct ChaosPatch *patch = &gChaosPatches[patchId];

        if (patch->effectType == CHAOS_EFFECT_POSITIVE) {
            sprintf(gFasttextTmpBuffer, "@05DF15--%s@--------", patch->name);
        } else if (patch->effectType == CHAOS_EFFECT_NEGATIVE) {
            sprintf(gFasttextTmpBuffer, "@FF1525--%s@--------", patch->name);
        } else {
            sprintf(gFasttextTmpBuffer, "@9F9F9F--%s@--------", patch->name);
        }

        strlen = sprintf(params->str, str, gFasttextTmpBuffer); // Print to params->str for now, to release gFasttextTmpBuffer for use again in the fasttext helper
        assert_args(strlen < MSGBUF_SIZE, "chaosmsg_print:\nString message too long:\n 0x%08X", strlen);
        fasttext_compute_print_text_with_line_breaks(FT_FONT, MAX_WIDTH, &lineCount, &strlen, gFasttextTmpBuffer, params->str);
    } else {
        fasttext_compute_print_text_with_line_breaks(FT_FONT, MAX_WIDTH, &lineCount, &strlen, gFasttextTmpBuffer, str);
    }

    assert_args(strlen < MSGBUF_SIZE, "chaosmsg_print:\nString message too long:\n 0x%08X", strlen);
    if (strlen > MSGBUF_SIZE) {
        strlen = MSGBUF_SIZE - 1;
    }
    bcopy(gFasttextTmpBuffer, params->str, strlen);
    params->str[strlen] = '\0';

    params->lineHeight = (lineCount * gFasttextFonts[FT_FONT].lineHeight) -
                         (gFasttextFonts[FT_FONT].lineHeight - gFasttextFonts[FT_FONT].averageCharHeight) +
                         (2 * MESSAGE_MARGIN);
    for (s32 i = 0; i < MSGBUF_COUNT; i++) {
        chsStrParams[i].yOffset += params->lineHeight;
    }

    chsStrIter = (chsStrIter + 1) % MSGBUF_COUNT;
}

static void chaosmsg_draw_bg(s32 x, s32 y, s32 width, s32 height, s32 alpha) {
    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
    gDPSetFillColor(gDisplayListHead++, (GPACK_RGBA5551(0, 0, 0, 1) << 16) | GPACK_RGBA5551(0, 0, 0, 1));
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, (u8) ((alpha * BG_TRANSPARENCY) + 0.5f));
    gDPFillRectangle(gDisplayListHead++, x, y, x + width, y + height);
    gDPPipeSync(gDisplayListHead++);
}

void chaosmsg_render(void) {
    s32 index = chsStrIter;
    if (waitFrames > 0) {
        waitFrames--;
    }

    fasttext_setup_textrect_rendering(FT_FONT);

    do {
        index = (index + 1) % MSGBUF_COUNT;

        struct ChaosMessageParams *params = &chsStrParams[index];
        if (params->status == CHSMSG_INACTIVE) {
            continue;
        }

        if (params->status == CHSMSG_WAITING && waitFrames == 0) {
            params->status = CHSMSG_DISPLAYING;
            waitFrames = WAIT_TIME;
        }

        if (params->status != CHSMSG_DISPLAYING) {
            continue;
        }

        if (params->animFrame >= DISPLAY_TIME + FADE_TIME) {
            params->status = CHSMSG_INACTIVE;
            continue;
        }

        s32 printX = BASE_X;
        s32 printY = SCREEN_HEIGHT - params->yOffset - MESSAGE_MARGIN;
        s32 transparency = 255;
        if (params->animFrame < TRANS_TIME) {
            printX += (120.0f * (1.0f - sins((0x4000 * params->animFrame) / TRANS_TIME)));
            transparency = ((255.0f * params->animFrame) / TRANS_TIME) + 0.5f;
        } else if (params->animFrame >= DISPLAY_TIME) {
            transparency = ((255.0f * (FADE_TIME - (params->animFrame - DISPLAY_TIME))) / FADE_TIME) + 0.5f;
            if (transparency < 0) {
                transparency = 0;
            }
        }

        chaosmsg_draw_bg(printX - MESSAGE_MARGIN,
                         printY + (gFasttextFonts[FT_FONT].lineHeight - gFasttextFonts[FT_FONT].averageCharHeight) - MESSAGE_MARGIN, 
                         MAX_WIDTH + (MESSAGE_MARGIN * 2),
                         params->lineHeight,
                         transparency);
        fasttext_draw_texrect(printX, printY, params->str, FT_FLAG_ALIGN_LEFT, 255, 255, 255, transparency);

        params->animFrame++;
    } while (index != chsStrIter);

    fasttext_finished_rendering();
}
