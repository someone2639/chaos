#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"
#include "sm64.h"
#include "audio/heap.h"
#include "audio/data.h"
#include "engine/behavior_script.h"
#include "engine/surface_collision.h"
#include "game/chaos/chaos.h"
#include "game/emutest.h"
#include "game/level_update.h"
#include "game/area.h"
#include "game/main.h"
#include "game/object_helpers.h"
#include "game/save_file.h"
#include "course_table.h"
#include "behavior_data.h"

void chs_act_reverb(void) { init_reverb_us(1U << 31); }
void chs_deact_reverb(void) { init_reverb_us(1U << 31); }

u8 chs_cond_randomized_music(void) {
    return (!gConfig.disableBGMusic);
}

u8 chs_cond_chill_out(void) {
    return (!gConfig.disableBGMusic);
}

u8 chs_cond_mad_musical_mess(void) {
    return (!gConfig.disableBGMusic);
}

void chs_act_sound_effect_shuffle(void) {
    s32 randomizedIndex;
    s32 tmp;

    // Shuffle discrete sfx
    for (s32 i = gDiscreteSFXCount - 1; i >= 0; i--) {
        randomizedIndex = random_u16() % (i + 1);
        tmp = gDiscreteSFXRandomized[i];
        gDiscreteSFXRandomized[i] = gDiscreteSFXRandomized[randomizedIndex];
        gDiscreteSFXRandomized[randomizedIndex] = tmp;
    }

    // Shuffle nondiscrete sfx
    for (s32 i = gNonDiscreteSFXCount - 1; i >= 0; i--) {
        randomizedIndex = random_u16() % (i + 1);
        tmp = gNonDiscreteSFXRandomized[i];
        gNonDiscreteSFXRandomized[i] = gNonDiscreteSFXRandomized[randomizedIndex];
        gNonDiscreteSFXRandomized[randomizedIndex] = tmp;
    }
}

s32 get_randomized_sound(s32 sound) {
    s32 count;
    s32 *normalArray;
    s32 *randomizedArray;
    s32 found = -1;

    if (sound & SOUND_DISCRETE) {
        count = gDiscreteSFXCount;
        normalArray = gDiscreteSFX;
        randomizedArray = gDiscreteSFXRandomized;
    } else {
        count = gNonDiscreteSFXCount;
        normalArray = gNonDiscreteSFX;
        randomizedArray = gNonDiscreteSFXRandomized;
    }

    // Binary search
    s32 low = 0;
    s32 high = count - 1;
    s32 mid;
    while (low <= high) {
        mid = low + ((high - low) / 2);

        if (normalArray[mid] == sound) {
            found = mid;
            break;
        }

        if (normalArray[mid] < sound) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    // Match found
    if (found >= 0) {
        // Carry over SOUND_LOWER_BACKGROUND_MUSIC flag, but nothing else
        return (randomizedArray[found] & ~SOUND_LOWER_BACKGROUND_MUSIC) | (sound & SOUND_LOWER_BACKGROUND_MUSIC);
    }

    // Not found
    return sound;
}