#define SIGN_CHAOS_DISTMULT 8

void bhv_sign_loop(void) {
    if (chaos_check_if_patch_active(CHAOS_PATCH_SIGNREAD_FAR)) {
        o->hitboxRadius = 150 * SIGN_CHAOS_DISTMULT;
        o->hitboxHeight = 80 * (SIGN_CHAOS_DISTMULT / 4);
    } else {
        o->hitboxRadius = 150;
        o->hitboxHeight = 80;
    }
}