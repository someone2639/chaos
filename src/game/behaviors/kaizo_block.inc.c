#define KBLOCK_ACT_1_FRAMES 3
#define KBLOCK_ACT_2_FRAMES 3

u32 interact_coin(struct MarioState *, u32, struct Object *);

void bhv_kaizo_block_loop(void) {
    switch(o->oAction) {
        case 0:
            //Raise and give coin
            o->oPosY += (10 * sins(((0xFFFF / 4) / KBLOCK_ACT_1_FRAMES) * o->oTimer));
            if(o->oTimer > KBLOCK_ACT_1_FRAMES) {
                o->oAction++;
                o->oAnimState++;
                struct Object *coinObj = spawn_object_abs_with_rot(o, 0, MODEL_YELLOW_COIN, bhvKaizoBlockCoin,
                            o->oPosX, o->oPosY + 50, o->oPosZ, 0, 0, 0);
                coinObj->oDamageOrCoinValue = 1;
                interact_coin(gMarioState, INTERACT_COIN, coinObj);
            }
            break;
        case 1:
            //Lower
            load_object_collision_model();
            o->oPosY -= (10 * sins(((0xFFFF / 4) / KBLOCK_ACT_2_FRAMES) * o->oTimer));
            if(o->oTimer > KBLOCK_ACT_2_FRAMES) {
                o->oAction++;
            }
            break;
        case 2:
            //Wait to despawn
            load_object_collision_model();
            if(o->oTimer > 300) {
                obj_mark_for_deletion(o);
            }
            break;
    }
}
