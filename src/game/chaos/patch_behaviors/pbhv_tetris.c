#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

#include "game/chaos/chaos.h"
#include "pbhv_tetris.h"

#include "engine/behavior_script.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/area.h"
#include "game/fasttext.h"
#include "game/debug.h"
#include "game/chaos_menus.h"
#include "game/ingame_menu.h"
#include "audio/external.h"
#include "sounds.h"

static s8 sTetrisHoldRTimer  =   0;

// Swaps the R button input to change camera mode from a button press to a button release
s8 gTetrisTriggerCameraR = FALSE;

// Tetrimino shape
#define TET_SHP(_1, _2, _3, _4) \
    ((_4 << 12) | (_3 << 8) | (_2 << 4) | _1)

// Every tetrimino and possible orientation.
// Hardcoded because it makes certain things easier and these are u16s so who cares
static const u16 sTetrisShapes[TETRIMINO_COUNT][TET_ORIENTATIONS] = {
    [TET_I] = {
        TET_SHP(
            0b0000,
            0b1111,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0100,
            0b0100,
            0b0100,
            0b0100
        ),
        TET_SHP(
            0b0000,
            0b0000,
            0b1111,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0010,
            0b0010,
            0b0010
        ),
    },
    [TET_O] = {
        TET_SHP(
            0b0000,
            0b0110,
            0b0110,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0110,
            0b0110,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0110,
            0b0110,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0110,
            0b0110,
            0b0000
        ),
    },
    [TET_T] = {
        TET_SHP(
            0b0010,
            0b0111,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0110,
            0b0010,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0111,
            0b0010,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0011,
            0b0010,
            0b0000
        ),
    },
    [TET_S] = {
        TET_SHP(
            0b0110,
            0b0011,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0110,
            0b0100,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0110,
            0b0011,
            0b0000
        ),
        TET_SHP(
            0b0001,
            0b0011,
            0b0010,
            0b0000
        ),
    },
    [TET_Z] = {
        TET_SHP(
            0b0011,
            0b0110,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0100,
            0b0110,
            0b0010,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0011,
            0b0110,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0011,
            0b0001,
            0b0000
        ),
    },
    [TET_J] = {
        TET_SHP(
            0b0001,
            0b0111,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0110,
            0b0010,
            0b0010,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0111,
            0b0100,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0010,
            0b0011,
            0b0000
        ),
    },
    [TET_L] = {
        TET_SHP(
            0b0100,
            0b0111,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0010,
            0b0010,
            0b0110,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0111,
            0b0001,
            0b0000
        ),
        TET_SHP(
            0b0011,
            0b0010,
            0b0010,
            0b0000
        ),
    },
};

// A set of collision checks that are performed when dropping a T piece to determine if a T-spin was performed.
// Two opposite checks are performed based on the orientation of the piece when it is locked to the grid.
static const u16 sTspinChecks[4] = {
    TET_SHP(
            0b0101,
            0b0000,
            0b0000,
            0b0000
        ),
        TET_SHP(
            0b0100,
            0b0000,
            0b0100,
            0b0000
        ),
        TET_SHP(
            0b0000,
            0b0000,
            0b0101,
            0b0000
        ),
        TET_SHP(
            0b0001,
            0b0000,
            0b0001,
            0b0000
        ),
};

// Color values for every type of tetrimino
static const u16 sTetCols[TETRIMINO_COUNT] = {
    [TET_I] = GPACK_RGBA5551(0x33, 0xCC, 0xCC, 1),
    [TET_O] = GPACK_RGBA5551(0xCC, 0xCC, 0x33, 1),
    [TET_T] = GPACK_RGBA5551(0xCC, 0x33, 0xCC, 1),
    [TET_S] = GPACK_RGBA5551(0x33, 0xCC, 0x33, 1),
    [TET_Z] = GPACK_RGBA5551(0xCC, 0x33, 0x33, 1),
    [TET_J] = GPACK_RGBA5551(0x33, 0x33, 0xCC, 1),
    [TET_L] = GPACK_RGBA5551(0xCC, 0x99, 0x33, 1),
};

// Color values for the ghost of every type of tetrimino
static const u16 sGhostCols[TETRIMINO_COUNT] = {
    [TET_I] = GPACK_RGBA5551(0x19, 0x66, 0x66, 1),
    [TET_O] = GPACK_RGBA5551(0x66, 0x66, 0x19, 1),
    [TET_T] = GPACK_RGBA5551(0x66, 0x19, 0x66, 1),
    [TET_S] = GPACK_RGBA5551(0x19, 0x66, 0x19, 1),
    [TET_Z] = GPACK_RGBA5551(0x66, 0x19, 0x19, 1),
    [TET_J] = GPACK_RGBA5551(0x19, 0x19, 0x66, 1),
    [TET_L] = GPACK_RGBA5551(0x66, 0x4C, 0x19, 1),
};

// Clockwise wall kick tests for the J, L, S, T, and Z pieces.
// Counterclockwise kicks are the same but inverted.
// Kick data taken from the tetris wiki here: https://tetris.wiki/Super_Rotation_System#Wall_Kicks
static const s8 sNormalKickData[TET_ORIENTATIONS][4][2] = {
    {{-1, 0}, {-1, -1}, {0, 2}, {-1, 2}},
    {{1, 0}, {1, 1}, {0, -2}, {1, -2}},
    {{1, 0}, {1, -1}, {0, 2}, {1, 2}},
    {{-1, 0}, {-1, 1}, {0, -2}, {-1, -2}},
};

// Clockwise wall kick tests for the I piece.
// Counterclockwise kicks are the same but inverted.
// Kick data taken from the tetris wiki here: https://tetris.wiki/Super_Rotation_System#Wall_Kicks
static const s8 sSpecialKickData[TET_ORIENTATIONS][4][2] = {
    {{-2, 0}, {1, 0}, {-2, 1}, {1, -2}},
    {{-1, 0}, {2, 0}, {-1, -2}, {2, 1}},
    {{2, 0}, {-1, 0}, {2, -1}, {-1, -2}},
    {{1, 0}, {-2, 0}, {1, 2}, {-2, -1}},
};

// Each frame, a certain amount of gravity is applied to the piece based on the current level.
// Values are taken from the tetris wiki here: https://tetris.wiki/Marathon#Speed_curve
// Everything is multiplied by 2 since the values given assume 60fps and we only run at 30.
static const f32 sGravityTable[TET_MAX_LEVEL] = {
    0.01667f * 2.0f,
    0.021017f * 2.0f,
    0.026977f * 2.0f,
    0.035256f * 2.0f,
    0.04693f * 2.0f,
    0.06361f * 2.0f,
    0.0879f * 2.0f,
    0.1236f * 2.0f,
    0.1775f * 2.0f,
    0.2598f * 2.0f,
    0.388f * 2.0f,
    0.59f * 2.0f,
    0.92f * 2.0f,
    1.46f * 2.0f,
    2.36f * 2.0f,
    3.91f * 2.0f,
    6.61f * 2.0f,
    11.43f * 2.0f,
    20.23f * 2.0f, 
    36.6f * 2.0f,
};

static struct Tetris sTetris;

// Dimensions of drawn image
#define TET_IMG_W   16
#define TET_IMG_H   ALIGN8(TETRIS_VISIBLE_HEIGHT)

// (x, y) for side piece displays
#define TET_SIDE_X  11
#define TET_NEXT_Y  4
#define TET_HOLD_Y  0

// rgba16 texture used to draw the tetris grid
ALIGNED8 static u16 sTetrisImg[TET_IMG_H][TET_IMG_W] = {
    0
};

// Checks a shape against a corresponding 4x4 section of the grid with (x, y) in the upper left corner
u16 tetris_check_collision(u16 shape, s32 x, s32 y) {
    // Encode a 4x4 cell of the collision into a u16
    u16 cell = 0;
    for(int i = y; i < (y + 4); i++) {
        u32 mask = (0x0F << x);
        u16 row = ((sTetris.collision[i] & mask) >> (x)) << ((i - y) * 4);
        cell |= row;
    }

    return (shape & cell);
}

// Returns TRUE if the piece can fit, FALSE if it does not
s32 tetris_fit_piece(struct Tetrimino *p) {
    u16 shape = sTetrisShapes[p->type][p->orientation];

    return !tetris_check_collision(shape, p->x + TET_COL_OFF, p->y);
}

// Finds the lowest point on the grid that a tetris piece can fall to in its current orientation
s8 tetris_find_floor(struct Tetrimino *p) {
    struct Tetrimino t = *p;

    while(tetris_fit_piece(&t)) {
        t.y++;
    }

    return t.y - 1;
}

// Shuffle one of the two bags, ensuring that all 7 types of tetriminos will appear in some random order.
void tetris_shuffle_bag(s32 bag) {
    assert(bag == 0 || bag == 1, "tetris_shuffle_bag:\nBad bag!");

    s32 high = (TETRIMINO_COUNT * (bag + 1)) - 1;
    s32 low = TETRIMINO_COUNT * bag;

    for(int i = high; i > low; i--) {
        s32 swap = (random_u16() % (i - low)) + low;
        s32 t = sTetris.bag[i];
        sTetris.bag[i] = sTetris.bag[swap];
        sTetris.bag[swap] = t;
    }
}

// Initializes and returns a new tetrimino of the specified type
struct Tetrimino tetris_init_piece(enum Tetriminos type) {
    struct Tetrimino piece = {0};
    piece.type = type;
    piece.orientation = 0;
    piece.x = (TETRIS_GRID_WIDTH / 2) - 2;
    piece.y = TETRIS_GRID_VANISH - 2;

    return piece;
}

// Grab and initialize new piece from the bag, and if neccesary shuffle the next bag.
struct Tetrimino tetris_next_piece(void) {
    enum Tetriminos type = sTetris.bag[sTetris.bagIndex];
    struct Tetrimino piece = tetris_init_piece(type);

    sTetris.floor = tetris_find_floor(&piece);
    sTetris.gravity = 1.0f;

    // If the piece preview would show the first index of the next bag, shuffle the contents of the next bag
    s32 piecesLeft = (sTetris.bagIndex + TET_BAG_PREVIEW) % TETRIMINO_COUNT;
    if(!(piecesLeft)) {
        if(sTetris.bagIndex < TETRIMINO_COUNT) {
            // We're still in bag 1, shuffle bag 2
            tetris_shuffle_bag(1);
        } else {
            // We're still in bag 2, shuffle bag 1
            tetris_shuffle_bag(0);
        }
    }

    if(++sTetris.bagIndex >= TET_BAG_SIZE) {
        sTetris.bagIndex = 0;
    }

    return piece;
}

// Drops down all lines above the given line
void tetris_drop_lines_above(s32 line) {
    for(int y = line; y >= 0; y--) {
        sTetris.collision[y] = sTetris.collision[y - 1];
        sTetris.tile[y] = sTetris.tile[y - 1];
    }

    // Zero out top line
    sTetris.collision[0] = TET_EMPTY_LINE;
    sTetris.tile[0] = 0;
}

// Clear any lines that are full.
void tetris_update_cleared_lines(void) {
    for(int y = 0; y < TETRIS_GRID_HEIGHT; y++) {
        if(sTetris.collision[y] == TET_FULL_LINE) {
            tetris_drop_lines_above(y);
        }
    }
}

// Adds to the game score based on the action and number of affected lines
// Scoring rules based on the information given here https://tetris.wiki/Scoring#Recent_guideline_compatible_games
void tetris_score_action(enum TetrisActions action, s32 lines) {
    u32 score = 0;
    s32 level = sTetris.level;
    s32 combo = sTetris.combo;
    s32 backToBack = sTetris.backToBack;
    s32 difficult = FALSE;

    switch (action) {
        case TET_ACT_LOCK:
            if(lines < 4) {
                if(lines > 0) {
                    score = (100 + (200 * (lines - 1))) * level;
                }
                backToBack = 0;
            } else {
                score = 800 * level;
                difficult = TRUE;
            }

            break;
        case TET_ACT_T_SPIN:
            if(lines > 0) {
                difficult = TRUE;
            }

            score = 400 * (lines + 1) * level;
            break;
        case TET_ACT_MINI_T_SPIN:
            if(lines > 0) {
                difficult = TRUE;
                score = 200 * lines * level;
            } else {
                score = 100 * level;
            }
            break;
        case TET_ACT_HARD_DROP:
            score = 2 * lines;
            break;
        case TET_ACT_SOFT_DROP:
            score = lines;
            break;
    }

    if(action != TET_ACT_SOFT_DROP && action != TET_ACT_HARD_DROP) {
        if(difficult) {
            if(backToBack) {
                score *= 1.5f;
            }
            backToBack++;
        }

        if(lines > 0) {
            combo++;
            score += (50 * combo * level);
        } else {
            combo = -1;
        }
    }

    sTetris.score += score;
    sTetris.combo = combo;
    sTetris.backToBack = backToBack;
}

// Returns how many lines are ready to clear
s32 tetris_get_cleared_lines(void) {
    s32 clearedLines = 0;
    for(int y = 0; y < TETRIS_GRID_HEIGHT; y++) {
        if(sTetris.collision[y] == TET_FULL_LINE) {
            clearedLines++;
        }
    }

    return clearedLines;
}

// Locks a tetris piece onto the stack.
// Also triggers scoring, line clears, and level progression based on the result of the lock down.
// Returns the next state for the game to enter
s32 tetris_lock_piece(struct Tetrimino *p) {
    u16 shape = sTetrisShapes[p->type][p->orientation];
    enum TetrisActions action = TET_ACT_LOCK;
    s32 state = TET_STATE_NEW_PIECE;

    // Check T spin
    if(p->type == TET_T && (p->lastManeuver & TET_MV_ROTATE)) {
        u16 frontTest = sTspinChecks[p->orientation];
        u16 backTest =  sTspinChecks[(p->orientation > 2) ? (p->orientation - 2) : (p->orientation + 2)];
        u16 frontResult = tetris_check_collision(frontTest, p->x + TET_COL_OFF, p->y);
        u16 backResult = tetris_check_collision(backTest, p->x + TET_COL_OFF, p->y);

        if(frontResult == frontTest && backResult) {
            action = TET_ACT_T_SPIN;
        } else if(backResult == backTest && frontResult) {
            if(p->lastManeuver & TET_MV_TIGHT) {
                action = TET_ACT_T_SPIN;
            } else {
                action = TET_ACT_MINI_T_SPIN;
            }
        }
    }

    for(int i = 0; i < 4; i++) {
        // Add new collision
        u32 mask = (0x0F << (i * 4));
        u16 collision = ((shape & mask) >> (i * 4));
        sTetris.collision[p->y + i] |= (collision << (p->x + TET_COL_OFF));

        // Add new tiles
        u32 tile = 0;
        u16 col = p->type;
        for(int x = 0; x < 4; x++) {
            s32 shift = ((p->x + x) * 3);
            if(shift < 0) continue;

            if(collision & (1 << x)) {
                tile |= (col << shift);
            }
        }
        sTetris.tile[p->y + i] |= tile;
    }

    s32 lines = tetris_get_cleared_lines();

    if(lines > 0) {
        // Update total lines and level
        sTetris.clearedLines += lines;
        if((sTetris.clearedLines >= 5 * sTetris.level) && (sTetris.level < TET_MAX_LEVEL)) {
            sTetris.level++;
        }

        state = TET_STATE_LINE_CLEAR;
        tetris_score_action(action, lines);
    }

    play_sound(SOUND_MENU_TETRIS_LOCK, gGlobalSoundSource);

    return state;
}

// Attempts to update the orientation of a tetrimino.
// Will also attempt to "kick" the tetrimino into a place that will fit if it cannot do so normally.
// -1 = counterclockwise
// +1 = clockwise
void tetris_rotate_piece(struct Tetrimino *p, s8 dir) {
    struct Tetrimino t = *p;
    s16 orientation = p->orientation + dir;

    if(orientation < 0) {
        orientation = TET_ORIENTATIONS - 1;
    } else if (orientation >= TET_ORIENTATIONS) {
        orientation = 0;
    }

    t.orientation = orientation;
    t.lastManeuver = TET_MV_ROTATE;

    if(t.lockDownTimer > 0 && t.lockDownSteps < 15) {
        t.lockDownTimer = 0;
        t.lockDownSteps++;
    }
    
    // Test to see if orientation fits
    if(t.type != TET_O && !tetris_fit_piece(&t)) {
        // Attempt to wall kick
        s32 kicked = FALSE;
        const s8 (*tests)[TET_ORIENTATIONS][4][2] = (t.type == TET_I) ? &sSpecialKickData : &sNormalKickData;

        for(int i = 0; i < 4; i++) {
            s8 offX = (*tests)[orientation][i][0] * dir;
            s8 offY = (*tests)[orientation][i][1] * dir;
            t.x = offX + p->x;
            t.y = offY + p->y;

            if(tetris_fit_piece(&t)) {
                kicked = TRUE;

                if(i == 3) {
                    t.lastManeuver |= TET_MV_TIGHT;
                }
                break;
            }
        }

        // Can't rotate, reset
        if(!kicked) {
            t = *p;
        } else {
            play_sound(SOUND_MENU_TETRIS_ROTATE, gGlobalSoundSource);
        }
    } else {
        play_sound(SOUND_MENU_TETRIS_ROTATE, gGlobalSoundSource);
    }

    *p = t;
}

// Attempts to move a tetrimino laterally or horizontally
void tetris_move_piece(struct Tetrimino *p, s32 x, s32 y) {
    struct Tetrimino t = *p;
    t.x += x;
    t.y += y;

    if(t.lockDownTimer > 0 && t.lockDownSteps < 15) {
        t.lockDownTimer = 0;
        t.lockDownSteps++;
    }

    // We assume any lateral movement should be considered a move rather than a drop (not really relevant in practice)
    t.lastManeuver = (x != 0) ? TET_MV_MOVE : TET_MV_DROP;

    if(!tetris_fit_piece(&t)) {
        t = *p;
    }

    *p = t;
}

// Resets the state of the game
void tetris_reset(void) {
    bzero(&sTetris, sizeof(struct Tetris));

    sTetris.level = 1;
    sTetris.combo = -1;
    sTetris.held = -1;
    sTetris.state = TET_STATE_DEFAULT;

    // Initial bag fill
    for(int i = 0; i < TET_BAG_SIZE; i++) {
        sTetris.bag[i] = i % TETRIMINO_COUNT;
    }
    tetris_shuffle_bag(0);

    // Initial collision
    for(int i = 0; i < ARRAY_COUNT(sTetris.collision); i++) {
        if(i < TETRIS_GRID_HEIGHT) {
            sTetris.collision[i] = TET_EMPTY_LINE;
        } else {
            sTetris.collision[i] = TET_FULL_LINE;
        }
    }

    sTetris.piece = tetris_next_piece();
}

void tetris_state_default(void) {
    struct Tetrimino *piece = &sTetris.piece;
    u32 pressed = gPlayer1Controller->buttonPressed;
    u32 down = gPlayer1Controller->buttonDown;
    
    s32 autoRepeat = 0;

    if(gMarioState->chaosStateFlags & CHAOS_STATE_CONTROLLING_TETRIS) {
        if((down & (L_JPAD | R_JPAD | L_CBUTTONS | R_CBUTTONS))) {
            if(sTetris.autoRepeatTimer++ > 5) {
                autoRepeat |= (down & (L_JPAD | R_JPAD | L_CBUTTONS | R_CBUTTONS));
                sTetris.autoRepeatTimer -= 2;
            }
        } else {
            sTetris.autoRepeatTimer = 0;
        }

        if(pressed & B_BUTTON) {
            // Rotate counterclockwise
            tetris_rotate_piece(piece, -1);
        } else if(pressed & A_BUTTON) {
            // Rotate clockwise
            tetris_rotate_piece(piece, 1);
        } else if(pressed & (Z_TRIG | L_TRIG) && !sTetris.usedHold) {
            s32 held = sTetris.held;

            sTetris.held = sTetris.piece.type;
            if(held != -1) {
                sTetris.piece = tetris_init_piece(held);
                sTetris.gravity = 1.0f;
                sTetris.floor = tetris_find_floor(&sTetris.piece);
            } else {
                sTetris.piece = tetris_next_piece();
            }

            sTetris.usedHold = TRUE;
            play_sound(SOUND_MENU_TETRIS_ROTATE, gGlobalSoundSource);
        } else if ((autoRepeat & (L_JPAD | L_CBUTTONS)) || (pressed & (L_JPAD | L_CBUTTONS))) {
            // Move left
            tetris_move_piece(piece, -1, 0);
        } else if ((autoRepeat & (R_JPAD | R_CBUTTONS)) || (pressed & (R_JPAD | R_CBUTTONS))) {
            // Move right
            tetris_move_piece(piece, 1, 0);
        } else if (pressed & (U_JPAD | U_CBUTTONS)) {
            // Hard drop
            s32 lines = piece->y;
            piece->y = sTetris.floor;
            lines = piece->y - lines;
            
            piece->lockDownTimer = 15;
            piece->lockDownSteps = 15;
            piece->lastManeuver = TET_MV_DROP;

            tetris_score_action(TET_ACT_HARD_DROP, lines);
        } else if (down & (D_JPAD | D_CBUTTONS)) {
            // Soft drop
            sTetris.gravity += 1.0f;
            tetris_score_action(TET_ACT_SOFT_DROP, 1);
        }
    } else {
        sTetris.autoRepeatTimer = 0;
    }

    // Handle gravity
    sTetris.gravity += sGravityTable[sTetris.level - 1];
    while(sTetris.gravity >= 1.0f) {
        tetris_move_piece(piece, 0, 1);
        sTetris.gravity -= 1.0f;
    }

    // Update ghost
    sTetris.floor = tetris_find_floor(piece);

    if((piece->y == sTetris.floor)) {
        if(piece->lockDownTimer++ == 15) {
            sTetris.state = tetris_lock_piece(&sTetris.piece);
        }
    }
}

void tetris_state_line_clear(void) {
    if(sTetris.timer == 0) {
        play_sound(SOUND_MENU_TETRIS_CLEAR_LINE, gGlobalSoundSource);
    }

    if(sTetris.timer++ > TET_LINE_CLEAR_ANIM) {
        tetris_update_cleared_lines();
        sTetris.state = TET_STATE_NEW_PIECE;
    }
}

void tetris_state_new_piece(void) {
    sTetris.piece = tetris_next_piece();
    sTetris.usedHold = FALSE;

    if(!tetris_fit_piece(&sTetris.piece)) {
        gMarioState->health = 0;
        sTetris.state = TET_STATE_GAME_OVER;
    } else {
        sTetris.state = TET_STATE_DEFAULT;
    }
}

void tetris_state_game_over(void) {
    if(!(sTetris.timer % 2) && sTetris.timer < TET_GAME_OVER_ANIM) {
        play_sound(SOUND_MENU_TETRIS_LOCK, gGlobalSoundSource);
    }

    if(sTetris.timer++ > TET_GAME_OVER_ANIM) {
        if(gMarioState->health != 0) {
            tetris_reset();
        }
    }
}

void chs_act_tetris(void) {
    tetris_reset();
}

void chs_deact_tetris(void) {
    gMarioStates->chaosStateFlags &= ~CHAOS_STATE_CONTROLLING_TETRIS;
    sTetrisHoldRTimer = 0;
    gTetrisTriggerCameraR = FALSE;
}

void chs_update_tetris(void) {
    if (!check_moving_play_mode(sCurrPlayMode) || gInActSelect) {
        return;
    }

    // Keeps track of how long the R trigger was held for, so that it doesn't interfere with the player control of the camera
    if(gPlayer1Controller->buttonDown & R_TRIG) {
        gTetrisTriggerCameraR = FALSE;
        if(sTetrisHoldRTimer++ > TETRIS_HOLD_R_FRAMES) {
            sTetrisHoldRTimer = TETRIS_HOLD_R_FRAMES;
            gMarioStates->chaosStateFlags |= CHAOS_STATE_CONTROLLING_TETRIS;
        }
    } else {
        if((sTetrisHoldRTimer > 0) && (sTetrisHoldRTimer < TETRIS_HOLD_R_FRAMES)) {
            gTetrisTriggerCameraR = TRUE;
        } else {
            gTetrisTriggerCameraR = FALSE;
        }

        gMarioStates->chaosStateFlags &= ~CHAOS_STATE_CONTROLLING_TETRIS;
        sTetrisHoldRTimer = 0;
    }

    s32 prev = sTetris.state;

    switch(sTetris.state) {
        case TET_STATE_DEFAULT:
            tetris_state_default();
            break;
        case TET_STATE_NEW_PIECE:
            tetris_state_new_piece();
            break;
        case TET_STATE_LINE_CLEAR:
            tetris_state_line_clear();
            break;
        case TET_STATE_GAME_OVER:
            tetris_state_game_over();
            break;
    }

    if(sTetris.state != prev) {
        sTetris.timer = 0;
    }
}

// Draw a tetrimino by shape at the given x, y, coordinates of sTetrisImg
// clear arg determines if the non-collision parts of the shape should clear pixels, used for bag display
void tetris_draw_tetrimino(s32 startX, s32 startY, u16 shape, u16 col, s32 clear) {
    for(int y = 0; y < 4; y++) {
        s32 shift = (y * 4);
        u16 row = (shape & (0x0F << shift)) >> shift;
        for(int x = 0; x < 4; x++) {
            if(row & (1 << x)) {
                sTetrisImg[startY + y][startX + x] = col;
            } else if (clear) {
                sTetrisImg[startY + y][startX + x] = 0;
            }
        }
    }
}

void tetris_draw_grid(void) {
    s32 x = 20;
    s32 y = 36;

    s32 imgX = x;
    s32 imgY = SCREEN_HEIGHT - y - (TETRIS_VISIBLE_HEIGHT * 4);

    s32 gridW = (TETRIS_GRID_WIDTH * 4) + 6;
    s32 gridX = ((TETRIS_GRID_WIDTH * 4) / 2) + x;

    s32 gridH = (TETRIS_VISIBLE_HEIGHT * 4) + 6;
    s32 gridY = ((TETRIS_VISIBLE_HEIGHT * 4) / 2) + y + 1;

    s32 sideW = 6 * 4;
    s32 sideX = gridX + (gridW / 2) + (sideW / 2) - 3;

    s32 holdH = sideW;
    s32 holdY = gridY + (gridH / 2) - (holdH / 2); 

    s32 bagH = gridH - holdH;
    s32 bagY = gridY - (gridH / 2) + (bagH / 2); 

    // Draw grid bg
    create_dl_ortho_matrix(&gDisplayListHead);

    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, gridX, gridY, 0.0f);
    Gfx *grid = menu_create_chaos_text_bg(gridX, gridY, gridW, gridH, 255);
    gSPDisplayList(gDisplayListHead++, grid);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);
    
    // Draw hold bg
    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, sideX, holdY, 0.0f);
    Gfx *hold = menu_create_chaos_text_bg(sideX, holdY, sideW, holdH, 255);
    gSPDisplayList(gDisplayListHead++, hold);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // Draw bag bg
    create_dl_translation_matrix(&gDisplayListHead, MENU_MTX_PUSH, sideX, bagY, 0.0f);
    Gfx *bag = menu_create_chaos_text_bg(sideX, bagY, sideW, bagH, 255);
    gSPDisplayList(gDisplayListHead++, bag);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    // Draw tetris image
    gDPPipeSync(gDisplayListHead++);
	gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
    gDPSetRenderMode(gDisplayListHead++, G_RM_TEX_EDGE, G_RM_TEX_EDGE2);
    gDPSetCombineMode(gDisplayListHead++, G_CC_DECALRGBA, G_CC_DECALRGBA);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_NONE);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_THRESHOLD);
    gDPSetTextureFilter(gDisplayListHead++, G_TF_POINT);
    gDPPipeSync(gDisplayListHead++);
    gSPTexture(gDisplayListHead++, 65535, 65535, 0, G_TX_RENDERTILE, G_ON);

    gDPLoadTextureBlock(gDisplayListHead++, &sTetrisImg, G_IM_FMT_RGBA, G_IM_SIZ_16b, TET_IMG_W, TET_IMG_H, 0, 0, 0, 5, 6, 0, 0);
    gSPTextureRectangle(gDisplayListHead++, imgX << 2, imgY << 2, (imgX + (16 * 4)) << 2, (imgY + (TETRIS_VISIBLE_HEIGHT * 4)) << 2,
                        G_TX_RENDERTILE, 0, 0, 256, 256);
    
    gDPPipeSync(gDisplayListHead++);
	gSPTexture(gDisplayListHead++, 65535, 65535, 0, G_TX_RENDERTILE, G_OFF);
	gDPSetTexturePersp(gDisplayListHead++, G_TP_PERSP);
	gDPSetAlphaCompare(gDisplayListHead++, G_AC_NONE);

    // Draw text
    char buf[64];
    sprintf(buf, "LV-%02d LN-%02d", sTetris.level, sTetris.clearedLines);
    fasttext_setup_textrect_rendering(&gDisplayListHead, FT_FONT_SMALL_BOLD);
    fasttext_draw_texrect(&gDisplayListHead, imgX, imgY - 17, buf, FT_FLAG_ALIGN_LEFT, 0xF0, 0xF0, 0xF0, 0xFF);
    sprintf(buf, "%010d", sTetris.score);
    fasttext_draw_texrect(&gDisplayListHead, imgX, imgY + (TETRIS_VISIBLE_HEIGHT * 4) - 1, buf, FT_FLAG_ALIGN_LEFT, 0xF0, 0xF0, 0xF0, 0xFF);

    if(sTetris.state == TET_STATE_GAME_OVER) {
        fasttext_draw_texrect(&gDisplayListHead, gridX, SCREEN_HEIGHT - gridY - 14, "GAME\nOVER", FT_FLAG_ALIGN_CENTER, 0xFF, 0xFF, 0xFF, 0xFF);
    } else if(!(gMarioStates->chaosStateFlags & CHAOS_STATE_CONTROLLING_TETRIS) && (gGlobalTimer % 60) < 30) {
        menu_single_button_prompt(&gDisplayListHead, x + (gridW / 2) + 16, imgY + (TETRIS_VISIBLE_HEIGHT * 4) + 9, MENU_PROMPT_R_TRIG, "Hold", FALSE);
        fasttext_setup_textrect_rendering(&gDisplayListHead, FT_FONT_SMALL_BOLD);
    }

    fasttext_finished_rendering(&gDisplayListHead);
}

void draw_tetris(void) {
    if (gInActSelect) {
        return;
    }
    
    // Render the grid
    for(int y = TETRIS_GRID_VANISH; y < TETRIS_GRID_HEIGHT; y++) {
        if((sTetris.state == TET_STATE_LINE_CLEAR) && (sTetris.collision[y] == TET_FULL_LINE)) {
            // Line clear animation
            s32 clearIndex = (TETRIS_GRID_WIDTH / 2) - (sTetris.timer / (TETRIS_GRID_WIDTH / TET_LINE_CLEAR_ANIM));
            for(int x = 0; x < TETRIS_GRID_WIDTH; x++) {
                if((x > clearIndex) && (x < TETRIS_GRID_WIDTH - clearIndex)) {
                    sTetrisImg[y - TETRIS_GRID_VANISH][x] = 0;
                } else {
                    sTetrisImg[y - TETRIS_GRID_VANISH][x] = 0xFFFF;
                }
            }
        } else if((sTetris.state == TET_STATE_GAME_OVER)){ 
            // Game over animation
            s32 eraseIndex = TETRIS_GRID_HEIGHT - (sTetris.timer / (TET_GAME_OVER_ANIM / TETRIS_VISIBLE_HEIGHT));
            if(y == eraseIndex) {
                for(int x = 0; x < TETRIS_GRID_WIDTH; x++) {
                    sTetrisImg[y - TETRIS_GRID_VANISH][x] = 0;
                }
            }
        } else {
            // Normal display
            for(int x = 0; x < TETRIS_GRID_WIDTH; x++) {
                if(sTetris.collision[y] & (1 << (x + TET_COL_OFF))) {
                    s32 shift = (x * 3);
                    u32 mask = (0x7 << shift);
                    u32 tile = (sTetris.tile[y] & mask) >> shift;
                    sTetrisImg[y - TETRIS_GRID_VANISH][x] = sTetCols[tile];
                } else {
                    sTetrisImg[y - TETRIS_GRID_VANISH][x] = 0;
                }
            }
        }
    }

    struct Tetrimino *p = &sTetris.piece;
    u16 shape = sTetrisShapes[p->type][p->orientation];

    if(sTetris.state == TET_STATE_DEFAULT) {
        // Render ghost
        tetris_draw_tetrimino(p->x, sTetris.floor - TETRIS_GRID_VANISH, shape, sGhostCols[p->type], FALSE);
        // Render active
        tetris_draw_tetrimino(p->x, p->y - TETRIS_GRID_VANISH, shape, sTetCols[p->type], FALSE);
    }

    if(sTetris.held != -1) {
        tetris_draw_tetrimino(TET_SIDE_X, TET_HOLD_Y, sTetrisShapes[sTetris.held][1], sTetCols[sTetris.held], TRUE);
    } else {
        tetris_draw_tetrimino(TET_SIDE_X, TET_HOLD_Y, 0, 0, TRUE);
    }

    // Render bag
    for(int i = 0; i < TET_BAG_PREVIEW; i++) {
        s32 prev = sTetris.bag[(sTetris.bagIndex + i) % TET_BAG_SIZE];
        tetris_draw_tetrimino(TET_SIDE_X, (TET_NEXT_Y + (5 * i) + 2), sTetrisShapes[prev][1], sTetCols[prev], TRUE);
    }

    tetris_draw_grid();
}