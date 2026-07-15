#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "chaos.h"

/*******************
 * Lives Modifiers *
 *******************/

 #define LIFETIME_SALE_COIN_REQ 50

/***********************
 * Star/Save Modifiers *
 ***********************/

 #define HIGH_ROLLER_COIN_REQ 60

/*******************
 * Random Griefing *
 *******************/

// Blindfolded Speedrun
#define BLIND_TIME_MAX    (30 * 60 * 4) // 3 minutes max without blindness
#define BLIND_TIME_END    (BLIND_TIME_MAX + (30 * 5)) // BLIND_TIME_MAX + 5 seconds
#define BLIND_TIME_BUFFER (30 * 60 * 1) // At least 60 seconds of relief

/**************
 * Time Limit *
 **************/
 
#define CHS_TIME_LIMIT (3 * 60 * 30)

/*********
 * Misc. *
 *********/

// Double Cherry
s16 set_cherry_animation(struct Object *o, s32 targetAnimID);
void delete_cherry_clone(struct Object *obj);

// Star Medallion
extern s32 gStarMedallionCollected;
extern s32 gStarMedallionActiveIndex;
