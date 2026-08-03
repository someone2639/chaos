// capswitch.inc.c

UNUSED u8 sCapSwitchText[] = { DIALOG_010, DIALOG_011, DIALOG_012 };

static u8 has_authentic_cap_unlocked(s32 flags) {
    if (flags & SAVE_FLAG_HAVE_WING_CAP) {
        if (chaos_check_if_patch_active(CHAOS_PATCH_GET_WING_CAP)) {
            return FALSE;
        }
    } else if (flags & SAVE_FLAG_HAVE_METAL_CAP) {
        if (chaos_check_if_patch_active(CHAOS_PATCH_GET_METAL_CAP)) {
            return FALSE;
        }
    } else if (flags & SAVE_FLAG_HAVE_VANISH_CAP) {
        if (chaos_check_if_patch_active(CHAOS_PATCH_GET_VANISH_CAP)) {
            return FALSE;
        }
    }

    return (save_file_get_flags() & flags);
}

static void activate_cap_switch(s32 flags) {
    if (flags & SAVE_FLAG_HAVE_WING_CAP) {
        s32 index = chaos_find_first_active_patch(CHAOS_PATCH_GET_WING_CAP, NULL);
        if (index >= 0) {
            chaos_remove_expired_entry(index, "%s: Expired!");
        }
    }

    if (flags & SAVE_FLAG_HAVE_METAL_CAP) {
        s32 index = chaos_find_first_active_patch(CHAOS_PATCH_GET_METAL_CAP, NULL);
        if (index >= 0) {
            chaos_remove_expired_entry(index, "%s: Expired!");
        }
    }

    if (flags & SAVE_FLAG_HAVE_VANISH_CAP) {
        s32 index = chaos_find_first_active_patch(CHAOS_PATCH_GET_VANISH_CAP, NULL);
        if (index >= 0) {
            chaos_remove_expired_entry(index, "%s: Expired!");
        }
    }

    save_file_set_flags(flags);
}

void cap_switch_act_0(void) {
    o->oAnimState = o->oBehParams2ndByte;
    cur_obj_scale(0.5f);
    o->oPosY += 71.0f;

    spawn_object_relative_with_scale(0, 0, -71, 0, 0.5f, o, MODEL_CAP_SWITCH_BASE, bhvCapSwitchBase);

    if (gCurrLevelNum != LEVEL_UNKNOWN_32) {
        if (has_authentic_cap_unlocked(sCapSaveFlags[o->oBehParams2ndByte])) {
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
        activate_cap_switch(sCapSaveFlags[o->oBehParams2ndByte]);
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
        if (gChaosGameMode == CHAOS_GAMEMODE_CHALLENGE) {
            play_sound(SOUND_GENERAL_COLLECT_1UP, gGlobalSoundSource);
            gMarioState->numLives++;
            save_file_set_life_count(gCurrSaveFileNum - 1, gMarioState->numLives, FALSE);
        }
        gChaosBlueStarLastCollected = FALSE;
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
