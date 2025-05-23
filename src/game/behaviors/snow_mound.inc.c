// snow_mound.inc.c

void bhv_sliding_snow_mound_loop(void) {
    switch (o->oAction) {
        case 0:
            o->oVelX = -40.0f;
            o->oPosX += o->oVelX;
            if (o->oTimer > 117) {
                o->oAction = 1;
            }
            cur_obj_play_sound_1(SOUND_ENV_SINK_QUICKSAND);
            break;

        case 1:
            o->oVelX = -5.0f;
            o->oPosX += o->oVelX;
            o->oVelY = -10.0f;
            o->oPosY += o->oVelY;
            o->oPosZ = o->oHomeZ - 2.0f;
            if (o->oTimer > 50) {
                o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
            }
            break;
    }
}

void bhv_snow_mound_spawn_loop(void) {
    struct Object *sp1C = NULL;

    if (!is_point_within_radius_of_mario(o->oPosX, o->oPosY, o->oPosZ, 6000)
        || o->oPosY + 1000.0f < gMarioObject->header.gfx.pos[1]) {
        return;
    }

    if (o->oTimer == 64 || o->oTimer == 128 || o->oTimer == 192 || o->oTimer == 224 || o->oTimer == 256) {
        sp1C = spawn_object(o, MODEL_SL_SNOW_TRIANGLE, bhvSlidingSnowMound);
    }

    if (sp1C && o->oTimer == 256) {
        sp1C->header.gfx.scale[0] = 2.0f;
        sp1C->header.gfx.scale[1] = 2.0f;
    }

    if (o->oTimer >= 256) {
        o->oTimer = 0;
    }
}
