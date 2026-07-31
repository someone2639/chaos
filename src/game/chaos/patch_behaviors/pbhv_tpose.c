#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "game/mario.h"
#include "sm64.h"
#include "object_fields.h"
#include "game/level_update.h"
#include "behavior_data.h"
#include "game/chaos/chaos.h"
#include "game/object_list_processor.h"
#include "game/object_helpers.h"
#include "game/level_update.h"
#include "engine/math_util.h"

void chs_update_poser(void) {
    if (!gMarioObject) {
        return;
    }

    struct Object *obj = find_first_object_with_behavior(bhvPoser);

    if (gMarioObject && obj == NULL) {
        obj = spawn_object_relative(0, 0, 0, 0, gMarioObject, MODEL_MARIO_ALWAYS_VISIBLE, bhvPoser);
    }

    if (obj) {
        obj->header.gfx.sharedChild = gLoadedGraphNodes[MODEL_MARIO_ALWAYS_VISIBLE];
    }
}

void chs_deact_poser(void) {
    struct Object *obj = find_first_object_with_behavior(bhvPoser);
    if (obj) {
        obj->header.gfx.node.flags |= GRAPH_RENDER_INVISIBLE;
        obj_mark_for_deletion(obj);
    }
}
