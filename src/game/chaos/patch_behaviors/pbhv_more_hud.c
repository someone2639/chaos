#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/emutest.h"
#include "game/segment2.h"
#include "game/hud.h"
#include "game/game_init.h"
#include "game/fasttext.h"
#include "game/area.h"
#include "course_table.h"
#include "game/level_update.h"
#include "engine/behavior_script.h"
#include "game/object_list_processor.h"
#include "game/rendering_graph_node.h"
#include "engine/math_util.h"
#include "lib/libpl2/libpl2-rhdc.h"

#include "game/chaos/chaos.h"

u8 rhdcUsernameSet = FALSE;
char rhdcUsername[64] = "";
s16 sMoreHUDLastCourseNum = -1;

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
    "Anonymous User Posts Cringe Online", // Note: THIS MUST COME FIRST! (libpl2 moment)
    "Local Player Still Hasn't Beat the Game",
    "B3313 Has Yet Another Drama Split",
    "There Isn't Enough Space On Screen To Fit This Headline",
    "I Can't See Anything!",
    "The Baseball Minigame is Real",
    "The Baseball Minigame is Fake",
    "There is No Cow Level",
    "L Might Be Real",
    "Romhacker Running Out of Ideas for Jokes",
    "You Can Press the A Button to Jump",
    "Mario Builder Competition Still Ongoing",
    "Just How Many Headlines Are There?",
    "Youngest Child Ever Just Born",
    VERSION_STRING " Out Now!",
    "That Last Death Was Really Embarrassing",
    "Ethan Wake Up",
    "Eastmost Penninsula is the Secret",
    "New Study Finds Gamers Don't Touch Grass",
    "Press F1 for Help",
    "safsggi3wh Sorry My Cat Stepped on the Keyboard",
    "F",
    "The News is Still Broken",
    "Insert Headline Here",
    "Is Every Copy of Super Mario 64 Personalized?",
    "Save Me",
    "Eating Laundry Detergent Found to be Unhealthy",
    "A Thing Happened",
    "@008F00--The Headline Turned Green!@--------",
    "Do You Know 'anime and games fan'?",
    "Regional News Channel Delivers the News",
    "Haha, Made You Look!",
    "Kirby is a...Pink Guy? (More at 11)",
    "Toad Soils Himself, Everyone Point and Laugh!",
    "Swing Your Arms From Side to Side",
    "Hotel Delfino Stocked With Dozens of Toasters",
    "DNA Test Confirms Luigi to Be Yoshi's Mother",
};

f32 sMoreHudHeadlinesWeights[ARRAY_COUNT(sMoreHudHeadlines)];

#define HEADLINE_WEIGHT_INCREASE (1.0f / (f32) ARRAY_COUNT(sMoreHudHeadlines))
s32 more_hud_get_headline(void) {
    s32 index;

    f32 weightTotal = 0.0f;
    f32 currentWeight = 0.0f;
    f32 generatedWeight = random_float();

    for (index = 0; index < ARRAY_COUNT(sMoreHudHeadlines); index++) {
        // Increase the probability selection window for each headline; offers future benefit against more recently selected indexes
        sMoreHudHeadlinesWeights[index] += HEADLINE_WEIGHT_INCREASE;

        // If weight is below 0, effectively exclude it from the possible selection pool
        if (sMoreHudHeadlinesWeights[index] > 0.0f) {
            weightTotal += sMoreHudHeadlinesWeights[index];
        }
    }

    generatedWeight *= weightTotal;

    for (index = 0; index < ARRAY_COUNT(sMoreHudHeadlines) - 1; index++) { // ARRAY_COUNT(sMoreHudHeadlines) - 1 not an accident
        // If weight is below 0, skip the index. This in theory should never favor the last unprocessed index if that falls below 0.
        if (sMoreHudHeadlinesWeights[index] <= 0.0f) {
            continue;
        }

        currentWeight += sMoreHudHeadlinesWeights[index];
        if (currentWeight > generatedWeight) {
            break;
        }
    }

    // Index should be guaranteed to not show up for next 50% of headlines, then will later become possible at low but increasing probability
    sMoreHudHeadlinesWeights[index] = -0.5f;

    return index;
}

void more_hud_draw_weather(void) {
    enum Weather weather = sMoreHudLevelData[sMoreHUDLastCourseNum].weather;

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
    f32 barFill = 0.0f;

    // Div by 0 check
    if (gMarioState->maxHealth != 0xFF) {
        barFill = 1.0f - ((f32)((gMarioState->maxHealth - 0xFF) - (gMarioState->health - 0xFF)) / (f32)(gMarioState->maxHealth - 0xFF));
    }
    s32 wedges = gMarioState->health / 0x100;

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
    // gDPSetEnvColor(gDisplayListHead++, 0x0A, 0xCF, 0x09, 0xFF);
    if (wedges > 8) {
        gDPSetEnvColor(gDisplayListHead++, 0xFD, 0xBD, 0x56, 0xFF);
    } else if (wedges > 6) {
        gDPSetEnvColor(gDisplayListHead++, 0x08, 0x83, 0xFF, 0xFF);
    } else if (wedges > 4) {
        gDPSetEnvColor(gDisplayListHead++, 0x08, 0xFF, 0x08, 0xFF);
    } else if (wedges > 2) {
        gDPSetEnvColor(gDisplayListHead++, 0xFF, 0xFF, 0x00, 0xFF);
    } else {
        gDPSetEnvColor(gDisplayListHead++, 0xFF, 0x29, 0x29, 0xFF);
    }
    if (gMarioState->health >= 0x100) {
        gDPFillRectangle(gDisplayListHead++, x + 46, y + 30, x + 46 + (79.0f * barFill) + 1, y + 36);
    }
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
    if (sMoreHudHeadlineIndex == 0 && rhdcUsernameSet) {
        sprintf(gFasttextTmpBuffer, "%s Posts Cringe Online", rhdcUsername);
        fasttext_draw_texrect(&gDisplayListHead, 10, SCREEN_HEIGHT - 35, gFasttextTmpBuffer, FT_FLAG_ALIGN_LEFT, 0x10, 0x10, 0x10, 0xFF);
    } else {
        fasttext_draw_texrect(&gDisplayListHead, 10, SCREEN_HEIGHT - 35, sMoreHudHeadlines[sMoreHudHeadlineIndex], FT_FLAG_ALIGN_LEFT, 0x10, 0x10, 0x10, 0xFF);
    }
    fasttext_finished_rendering(&gDisplayListHead);
}

void more_hud_draw_pings_for_list(struct Object* list, s32 x, s32 y) {
    struct Object *head = (struct Object *)list->header.next;
    f32 flipX = 1.0f;
    s16 cameraYaw = -gLakituState.yaw;

    if (isGameFlipped) {
        flipX *= -1.0f;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_UPSIDE_DOWN_CAMERA)) {
        cameraYaw += 0x8000;
    }
    if (chaos_check_if_patch_active(CHAOS_PATCH_SIDEWAYS_CAMERA)) {
        if (gChsSidewaysOrientation == 1) {
            cameraYaw += 0x4000;
        } else {
            cameraYaw -= 0x4000;
        }
    }

    while (head != list) {
        if(head->header.gfx.sharedChild != NULL) {
            f32 dist;
            s16 objYaw;
            vec3f_get_lateral_dist(&head->oPosVec, &gMarioObject->oPosVec, &dist);
            vec3f_get_yaw(&head->oPosVec, &gMarioObject->oPosVec, &objYaw);
            if(dist < 2900.0f) {
                f32 offX = sins(cameraYaw + objYaw) * (dist / 3000.0f) * -16.0f * flipX;
                f32 offZ = coss(cameraYaw + objYaw) * (dist / 3000.0f) * -16.0f;

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
    more_hud_draw_weather();
    more_hud_draw_health_meter();
    more_hud_draw_breaking_news();
    more_hud_draw_radar();
}

void chs_act_more_hud(void) {
    sMoreHUDLastCourseNum = -1;
    if (gLibplABI > 0 && lpl2_get_my_rhdc_username(rhdcUsername, NULL) > 0) {
        rhdcUsernameSet = TRUE;
    } else {
        rhdcUsernameSet = FALSE;
    }

    for (u32 i = 0; i < ARRAY_COUNT(sMoreHudHeadlinesWeights); i++) {
        sMoreHudHeadlinesWeights[i] = HEADLINE_WEIGHT_INCREASE;
    }

    chs_update_more_hud();
}

void chs_update_more_hud(void) {
    if (gCurrCourseNum == sMoreHUDLastCourseNum) {
        return;
    }

    if (gCurrCourseNum != COURSE_NONE || sMoreHUDLastCourseNum < 0) {
        sMoreHudHeadlineIndex = more_hud_get_headline();
    }
    sMoreHudCurTempHigh = sMoreHudLevelData[gCurrCourseNum].temperature + (random_float() * 4.0f);
    sMoreHudCurTempLow = sMoreHudLevelData[gCurrCourseNum].temperature - (random_float() * 4.0f);

    sMoreHUDLastCourseNum = gCurrCourseNum;
}