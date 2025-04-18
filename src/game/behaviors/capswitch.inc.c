// capswitch.inc.c

UNUSED u8 sCapSwitchText[] = { DIALOG_010, DIALOG_011, DIALOG_012 };

void cap_switch_act_0(void) {
    o->oAnimState = o->oBehParams2ndByte;
    cur_obj_scale(0.5f);
    o->oPosY += 71.0f;

    spawn_object_relative_with_scale(0, 0, -71, 0, 0.5f, o, MODEL_CAP_SWITCH_BASE, bhvCapSwitchBase);

    if (gCurrLevelNum != LEVEL_UNKNOWN_32) {
        if (save_file_get_flags() & sCapSaveFlags[o->oBehParams2ndByte]) {
            o->oAction = 3;
            o->header.gfx.scale[1] = 0.1f;
        } else {
            o->oAction = 1;
        }
    } else {
        o->oAction = 1;
    }
}

void cap_switch_act_1(void) {
    if (cur_obj_is_mario_on_platform()) {
        save_file_set_flags(sCapSaveFlags[o->oBehParams2ndByte]);
        o->oAction = 2;
        cur_obj_play_sound_2(SOUND_GENERAL_ACTIVATE_CAP_SWITCH);
    }
}

void cap_switch_act_2(void) {
    if (o->oTimer < 5) {
        cur_obj_scale_over_time(2, 4, 0.5f, 0.1f);
        if (o->oTimer == 4) {
            cur_obj_shake_screen(SHAKE_POS_SMALL);
            spawn_mist_particles();
            spawn_triangle_break_particles(60, MODEL_CARTOON_STAR, 0.3f, o->oBehParams2ndByte);
#if ENABLE_RUMBLE
            queue_rumble_data(5, 80);
#endif
        }
    } else {
        if (gChaosLivesEnabled) {
            play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
            gMarioState->numLives++;
            save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, FALSE);
        }
        set_play_mode(PLAY_MODE_SELECT_PATCH);
        o->oAction = 3;
    }
}

// dead function
void cap_switch_act_3(void) {
}

void (*sCapSwitchActions[])(void) = {
    cap_switch_act_0,
    cap_switch_act_1,
    cap_switch_act_2,
    cap_switch_act_3,
};

void bhv_cap_switch_loop(void) {
    cur_obj_call_action_function(sCapSwitchActions);
}
