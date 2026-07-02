// castle_cannon_grate.inc.c

void bhv_castle_cannon_grate_init(void) {
    if (!chaos_check_if_patch_active(CHAOS_PATCH_LOCK_CANNONS) &&
                (chaos_check_if_patch_active(CHAOS_PATCH_UNLOCK_CANNONS) || save_file_get_total_star_count(gCurrSaveFileNum - 1, COURSE_MIN - 1, COURSE_MAX - 1) >= 120)) {
        o->activeFlags = ACTIVE_FLAG_DEACTIVATED;
    }
}
