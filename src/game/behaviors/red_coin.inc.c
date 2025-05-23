
/**
 * This file contains the initialization and behavior for red coins.
 * Behavior controls audio and the orange number spawned, as well as interacting with
 * the course's red coin star.
 */

/**
 * Red coin's hitbox details.
 */
static struct ObjectHitbox sRedCoinHitbox = {
    /* interactType:      */ INTERACT_COIN,
    /* downOffset:        */ 0,
    /* damageOrCoinValue: */ 2,
    /* health:            */ 0,
    /* numLootCoins:      */ 0,
    /* radius:            */ 100,
    /* height:            */ 64,
    /* hurtboxRadius:     */ 0,
    /* hurtboxHeight:     */ 0,
};

/**
 * Red coin initialization function. Sets the coin's hitbox and parent object.
 */
void bhv_red_coin_init(void) {
    // This floor and floor height are unused. Perhaps for orange number spawns originally?
    struct Surface *dummyFloor;
    UNUSED f32 floorHeight = find_floor(o->oPosX, o->oPosY, o->oPosZ, &dummyFloor);

    // Set the red coins to have a parent of the closest red coin star.
    struct Object *hiddenRedCoinStar;

    if ((hiddenRedCoinStar =
             cur_obj_nearest_object_with_behavior(bhvHiddenRedCoinStar)) != NULL) {
        o->parentObj = hiddenRedCoinStar;
    } else if ((hiddenRedCoinStar =
             cur_obj_nearest_object_with_behavior(bhvBowserCourseRedCoinStar)) != NULL) {
        o->parentObj = hiddenRedCoinStar;
    } else {
        o->parentObj = NULL;
    }

    obj_set_hitbox(o, &sRedCoinHitbox);
}

/**
 * Main behavior for red coins. Primarily controls coin collection noise and spawning
 * the orange number counter.
 */
void bhv_red_coin_loop(void) {
    s32 coinRequirement = 8;
    if (chaos_check_if_patch_active(CHAOS_PATCH_6_RED_COINS)) {
        coinRequirement = 6;
    }

    if (chaos_check_if_patch_active(CHAOS_PATCH_SCARED_COINS)) {
        f32 dx = o->oHomeX - gMarioObject->oPosX;
        f32 dy = o->oHomeY - gMarioObject->oPosY;
        f32 dz = o->oHomeZ - gMarioObject->oPosZ;
        f32 marioDistToHome = sqrtf(dx * dx + dy * dy + dz * dz);
        f32 objDistToHome = cur_obj_lateral_dist_to_home();
        s16 runAngle;

        if(o->oDistanceToMario < 600.0f) {
            runAngle = (s16)o->oAngleToMario + 0x8000;
            o->oPosX += 8 * sins(runAngle);
            o->oPosZ += 8 * coss(runAngle);
        } else if(marioDistToHome > 1000.0f) {
            if(objDistToHome > 30.0f) {
                runAngle = cur_obj_angle_to_home();
                o->oPosX += 30 * sins(runAngle);
                o->oPosZ += 30 * coss(runAngle);
            } else {
                o->oPosX = o->oHomeX;
                o->oPosZ = o->oHomeZ;
            }
        } 
    }

    // If Mario interacted with the object...
    if (o->oInteractStatus & INT_STATUS_INTERACTED) {
        // ...and there is a red coin star in the level...
        if (o->parentObj != NULL) {
            // ...increment the star's counter.
            o->parentObj->oHiddenStarTriggerCounter++;

            // For JP version, play an identical sound for all coins.
#ifdef VERSION_JP
            create_sound_spawner(SOUND_GENERAL_RED_COIN);
#endif
            // Spawn the orange number counter, as long as it isn't the last coin.
            if (o->parentObj->oHiddenStarTriggerCounter != coinRequirement) {
                spawn_orange_number(o->parentObj->oHiddenStarTriggerCounter, 0, 0, 0);
            }

            // On all versions but the JP version, each coin collected plays a higher noise.
#ifdef JP_RED_COIN_SOUND
            // For JP version, play an identical sound for all coins.
            create_sound_spawner(SOUND_GENERAL_RED_COIN);
#else
            play_sound(SOUND_MENU_COLLECT_RED_COIN
                       + (((u8) o->parentObj->oHiddenStarTriggerCounter - 1) << 16),
                       gGlobalSoundSource);
#endif
        }

        coin_collected();
        // Despawn the coin.
        o->oInteractStatus = 0;
    }
}
