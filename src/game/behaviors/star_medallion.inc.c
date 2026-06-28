// star_medallion.inc.c

void bhv_star_medallion_loop(void) {
    o->oFaceAngleYaw += 0x200;

    if (o->oInteractStatus & INT_STATUS_INTERACTED || !chaos_check_if_patch_active(CHAOS_PATCH_STAR_MEDALLION) || gStarMedallionCollected) {
        mark_obj_for_deletion(o);
        o->oInteractStatus = 0;
    }
}
