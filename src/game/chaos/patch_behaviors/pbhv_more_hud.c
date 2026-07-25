#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/segment2.h"
#include "game/hud.h"
#include "game/game_init.h"
#include "game/fasttext.h"
#include "game/area.h"
#include "course_table.h"
#include "game/level_update.h"
#include "engine/behavior_script.h"
#include "game/object_list_processor.h"
#include "engine/math_util.h"

#include "game/chaos/chaos.h"

enum Weather {
    WTH_NONE,
    WTH_SUN,
    WTH_SNOW,
    WTH_CLOUD,
    WTH_SPOOKY,
    WTH_WET,
    WTH_HOT,
    WTH_CAVE,
    WTH_CLOCK,

    WTH_COUNT,
};

struct MoreHudLevelData {
    enum Weather weather;
    f32 temperature;
};

u8 *sWeatherTextures[WTH_COUNT] = {
    [WTH_NONE]      = wth_none_rgba16,
    [WTH_SUN]       = wth_sun_rgba16,
    [WTH_SNOW]      = wth_snow_rgba16,
    [WTH_CLOUD]     = wth_cloud_rgba16,
    [WTH_SPOOKY]    = wth_spooky_rgba16,
    [WTH_WET]       = wth_wet_rgba16,
    [WTH_HOT]       = wth_hot_rgba16,
    [WTH_CAVE]      = wth_cave_rgba16,
    [WTH_CLOCK]     = wth_clock_rgba16,
};

struct MoreHudLevelData sMoreHudLevelData[COURSE_COUNT] = {
    [COURSE_NONE] = {
        .weather = WTH_SUN,
        .temperature = 72.4f,
    },

    [COURSE_BOB] = {
        .weather = WTH_SUN,
        .temperature = 80.3f,
    },
    [COURSE_WF] = {
        .weather = WTH_SUN,
        .temperature = 65.4f,
    },
    [COURSE_JRB] = {
        .weather = WTH_WET,
        .temperature = 56.4f,
    },
    [COURSE_CCM] = {
        .weather = WTH_SNOW,
        .temperature = 10.7f,
    },
    [COURSE_BBH] = {
        .weather = WTH_SPOOKY,
        .temperature = 66.6f,
    },
    [COURSE_HMC] = {
        .weather = WTH_CAVE,
        .temperature = 59.1f,
    },
    [COURSE_LLL] = {
        .weather = WTH_HOT,
        .temperature = 115.4f,
    },
    [COURSE_SSL] = {
        .weather = WTH_SUN,
        .temperature = 105.3f,
    },
    [COURSE_DDD] = {
        .weather = WTH_WET,
        .temperature = 49.2f,
    },
    [COURSE_SL] = {
        .weather = WTH_SNOW,
        .temperature = 32.5f,
    },
    [COURSE_WDW] = {
        .weather = WTH_WET,
        .temperature = 62.2f,
    },
    [COURSE_TTM] = {
        .weather = WTH_SUN,
        .temperature = 70.3f,
    },
    [COURSE_THI] = {
        .weather = WTH_SUN,
        .temperature = 86.3f,
    },
    [COURSE_TTC] = {
        .weather = WTH_CLOCK,
        .temperature = 64.0f,
    },
    [COURSE_RR] = {
        .weather = WTH_CLOUD,
        .temperature = 44.3f,
    },

    [COURSE_BITDW] = {
        .weather = WTH_CAVE,
        .temperature = 49.1f,
    },
    [COURSE_BITFS] = {
        .weather = WTH_HOT,
        .temperature = 111.1f,
    },
    [COURSE_BITS] = {
        .weather = WTH_SPOOKY,
        .temperature = 46.8f,
    },

    [COURSE_TOTWC] = {
        .weather = WTH_CLOUD,
        .temperature = 42.4f,
    },
    [COURSE_COTMC] = {
        .weather = WTH_CAVE,
        .temperature = 59.0f,
    },
    [COURSE_VCUTM] = {
        .weather = WTH_WET,
        .temperature = 58.2f,
    },

    [COURSE_PSS] = {
        .weather = WTH_NONE,
        .temperature = 64.0f,
    },
    [COURSE_SA] = {
        .weather = WTH_CLOUD,
        .temperature = 60.5f,
    },
    [COURSE_WMOTR] = {
        .weather = WTH_CLOUD,
        .temperature = 41.4f,
    },
};

f32 sMoreHudCurTempHigh = 0.0f;
f32 sMoreHudCurTempLow = 0.0f;

u16 sMoreHudHeadlineIndex = 0;

const char *sMoreHudHeadlines[] = {
    "Local Player Still Hasn't Beat the Game",
    "B3313 Has Yet Another Drama Split",
    "There Isn't Enough Space On Screen To Fit This Headline",
    "I Can't See Anything!",
    "The Baseball Minigame is Real",
    "The Baseball Minigame is Fake",
    "You Posted Cringe Online",
    "There is no Cow Level",
    "L is Real",
    "Romhacker Running Out of Ideas for Jokes",
    "You Can Press the A Button to Jump",
    "Mario Builder Competition Still Ongoing",
    "You Probably Haven't Seen Every Headline Yet",
    "Youngest Child Ever Just Born",
    "Rogue Chaos 2.0 Out Now!",
    "That Last Death Was Really Embarrassing",
    "Ethan Wake Up",
    "Eastmost Penninsula is the Secret",
    "New Study Finds Gamers Don't Touch Grass",
    "Press F1 For Help",
    "safsggi3wh Sorry My Cat Stepped on The Keyboard",
    "Insert Headline Here",
    "\"Ad Breaks\" Patch Still Crashes Sometimes",
};

void more_hud_draw_weather(void) {
    enum Weather weather = sMoreHudLevelData[gCurrCourseNum].weather;

    f32 tempHighF = sMoreHudCurTempHigh;
    f32 tempHighC = (tempHighF - 32.0f) * (5.0f / 9.0f);
    f32 tempLowF = sMoreHudCurTempLow;
    f32 tempLowC = (tempLowF - 32.0f) * (5.0f / 9.0f);

    char tempHighBuf[32];
    char tempLowBuf[32];

    sprintf(tempHighBuf, "%.2f(F)\n%.2f(C)", tempHighF, tempHighC);
    sprintf(tempLowBuf, "%.2f(F)\n%.2f(C)", tempLowF, tempLowC);

    s32 xPosLeft = SCREEN_WIDTH - 70;
    s32 xPosRight = xPosLeft + 6;
    s32 yPos = 58;

    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);
    render_hud_tex_lut(xPosRight, yPos + 15, sWeatherTextures[weather]);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);

    fasttext_setup_textrect_rendering(&gDisplayListHead, FT_FONT_OUTLINE);
    fasttext_draw_texrect(&gDisplayListHead, xPosLeft, yPos, "Today's Forecast", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_draw_texrect(&gDisplayListHead, xPosLeft, yPos += 15, "Weather", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_draw_texrect(&gDisplayListHead, xPosLeft, yPos += 15, "Temp (High)", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_draw_texrect(&gDisplayListHead, xPosRight, yPos, tempHighBuf, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_draw_texrect(&gDisplayListHead, xPosLeft, yPos += 25, "Temp (Low)", FT_FLAG_ALIGN_RIGHT, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_draw_texrect(&gDisplayListHead, xPosRight, yPos, tempLowBuf, FT_FLAG_ALIGN_LEFT, 0xFF, 0xFF, 0xFF, 0xFF);
    fasttext_finished_rendering(&gDisplayListHead);
}

void more_hud_draw_health_meter(void) {
    s32 x = 20;
    s32 y = (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE) ? 24 : 6;
    f32 barFill = 1.0f - ((f32)((gMarioState->maxHealth - 0xFF) - (gMarioState->health - 0xFF)) / (f32)(gMarioState->maxHealth - 0xFF));

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

    draw_sprite(&gDisplayListHead, wow_health_meter_rgba16, G_IM_FMT_RGBA, G_IM_SIZ_16b, FALSE, 128, 64, x, y, 128, 64);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF);

    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
    gDPSetEnvColor(gDisplayListHead++, 0x0A, 0xCF, 0x09, 0xFF);
    gDPFillRectangle(gDisplayListHead++, x + 46, y + 30, x + 46 + (80.0f * barFill), y + 36);
    gDPPipeSync(gDisplayListHead++);

    gDPSetCombineMode (gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
}

void more_hud_draw_breaking_news(void) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

    draw_sprite(&gDisplayListHead, breaking_news_rgba16, G_IM_FMT_RGBA, G_IM_SIZ_16b, FALSE, 308, 48, 0, SCREEN_HEIGHT - 62, 308, 48);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF);
    gDPSetCombineMode (gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);

    fasttext_setup_textrect_rendering(&gDisplayListHead, FT_FONT_MEDIUM_NOSHADOW);
    fasttext_draw_texrect(&gDisplayListHead, 10, SCREEN_HEIGHT - 35, sMoreHudHeadlines[sMoreHudHeadlineIndex], FT_FLAG_ALIGN_LEFT, 0x10, 0x10, 0x10, 0xFF);
    fasttext_finished_rendering(&gDisplayListHead);
}

void more_hud_draw_pings_for_list(struct Object* list, s32 x, s32 y) {
    struct Object *head = (struct Object *)list->header.next;

    while (head != list) {
        if(head->header.gfx.sharedChild != NULL) {
            f32 dist;
            vec3f_get_lateral_dist(&head->oPosVec, &gMarioObject->oPosVec, &dist);
            if(dist < 2900.0f) {
                f32 offX = ((gMarioObject->oPosX - head->oPosX) / 3000.0f) * 16;
                f32 offZ = ((gMarioObject->oPosZ - head->oPosZ) / 3000.0f) * 16;

                gDPFillRectangle(gDisplayListHead++, (x + 16 + offX) - 1, (y + 16 + offZ) - 1, (x + 16 + offX) + 1, (y + 16 + offZ) + 1);
            }
            
        }
        head = (struct Object *)head->header.next;
    }
}

void more_hud_draw_radar(void) {
    s32 x = 16;
    s32 y = SCREEN_HEIGHT - 98;

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);

    draw_sprite(&gDisplayListHead, radar_bg_rgba16, G_IM_FMT_RGBA, G_IM_SIZ_16b, FALSE, 32, 32, x, y, 32, 32);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_BILERP);
    gDPSetRenderMode(gDisplayListHead++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF);

    gDPSetCombineLERP(gDisplayListHead++, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT, 0, 0, 0, ENVIRONMENT);
    gDPSetEnvColor(gDisplayListHead++, 0x0A, 0xCF, 0x09, 0xFF);

    more_hud_draw_pings_for_list((struct Object *) &gObjectLists[OBJ_LIST_PUSHABLE], x, y);
    more_hud_draw_pings_for_list((struct Object *) &gObjectLists[OBJ_LIST_GENACTOR], x, y);
    more_hud_draw_pings_for_list((struct Object *) &gObjectLists[OBJ_LIST_DESTRUCTIVE], x, y);

    gDPPipeSync(gDisplayListHead++);

    gDPSetCombineMode (gDisplayListHead++, G_CC_SHADE, G_CC_SHADE);
}

void draw_more_hud(void) {
    if (gInActSelect) return;

    more_hud_draw_weather();
    more_hud_draw_health_meter();
    more_hud_draw_breaking_news();
    more_hud_draw_radar();
}

void more_hud_update(void) {
    sMoreHudHeadlineIndex = random_u16() % ARRAY_COUNT(sMoreHudHeadlines);
    sMoreHudCurTempHigh = sMoreHudLevelData[gCurrCourseNum].temperature + (random_float() * 4.0f);
    sMoreHudCurTempLow = sMoreHudLevelData[gCurrCourseNum].temperature - (random_float() * 4.0f);
}

void chs_act_more_hud(void) {
    more_hud_update();
}

void chs_lvlinit_more_hud(void) {
    more_hud_update();
}