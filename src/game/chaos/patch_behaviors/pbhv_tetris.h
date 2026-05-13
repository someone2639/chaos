#pragma once

#include <PR/ultratypes.h>
#include <PR/gbi.h>
#include "types.h"

// Playable columns in the tetris grid
#define TETRIS_GRID_WIDTH   10

// Playable rows of the tetris grid
#define TETRIS_GRID_HEIGHT  32

// Visible rows in the tetris grid
#define TETRIS_VISIBLE_HEIGHT   20

// The point on the tetris grid where pieces will become visible.
// New pieces will also spawn two units above this point.
#define TETRIS_GRID_VANISH  (TETRIS_GRID_HEIGHT - TETRIS_VISIBLE_HEIGHT)

#define TET_BAG_SIZE    (TETRIMINO_COUNT * 2) // We need 2 bags to effectively preview more than one piece
#define TET_BAG_PREVIEW 3

#define TET_ORIENTATIONS   4

#define TET_MAX_LEVEL   20

#define TET_EMPTY_LINE      0xE007
#define TET_FULL_LINE       0xFFFF

#define TET_LINE_CLEAR_ANIM 10
#define TET_GAME_OVER_ANIM  60

// Collision offset, left and right borders of collision are padded with 3 thick walls, but tetrimino piece positioning + image drawing does not account for this
#define TET_COL_OFF     3   

enum Tetriminos {
    TET_I,
    TET_O,
    TET_T,
    TET_S,
    TET_Z,
    TET_J,
    TET_L,

    TETRIMINO_COUNT,
};

enum TetrisManeuvers {
    TET_MV_MOVE     = (1 << 0),
    TET_MV_DROP     = (1 << 1),
    TET_MV_ROTATE   = (1 << 2),
    TET_MV_TIGHT    = (1 << 3),
};

enum TetrisActions {
    TET_ACT_LOCK,
    TET_ACT_T_SPIN,
    TET_ACT_MINI_T_SPIN,
    TET_ACT_HARD_DROP,
    TET_ACT_SOFT_DROP,
};

enum TetrisStates {
    TET_STATE_DEFAULT,
    TET_STATE_NEW_PIECE,
    TET_STATE_LINE_CLEAR,
    TET_STATE_GAME_OVER,
};

// A tetrimino and all associated qualities
struct Tetrimino {
    // Tetrimino type
    u8 type : 3;

    // Current orientation
    u8 orientation : 2;

    // Last maneuver the tetrimino made
    u8 lastManeuver : 3;

    // x position of the upper left corner of the tetrimino's 4x4 collision grid
    s8 x;

    // y position of the upper left corner of the tetrimino's 4x4 collision grid
    s8 y;

    // How long the tetrimino has been touching the stack. Snaps to the grid at 15 frames.
    u8 lockDownTimer : 4;

    // How many times the tetrimino has had its lock down timer reset by moving or rotating. Maximum 15.
    u8 lockDownSteps : 4;
};

// The main tetris game and all associated qualities
struct Tetris {
    // The current game state
    u8 state;

    // A generic timer with a different use case per state
    u16 timer;

    // Score for the tetris game. Entirely superficial in this hack but it's still fun to include!
    u32 score;

    // Back to back tracking for "difficult" actions
    u8 backToBack;

    // Tracking for combo bonus points
    s8 combo;

    // Current level of the game. Controls speed of gravity.
    u8 level;

    // How many lines have been cleared. Clear more to progress level.
    u16 clearedLines;

    // Gravity for the current piece. Builds up every frame. When greater than 1, drop piece and lower by 1.
    f32 gravity;

    // Bitfield used for testing collision with incoming pieces, padded with 3 layers of collision on the left, right, and bottom
    u16 collision[TETRIS_GRID_HEIGHT + TET_COL_OFF];

    // Tile map used for coloration of the grid
    u32 tile[TETRIS_GRID_HEIGHT];

    // The active piece
    struct Tetrimino piece;

    // The type of piece in the hold slot. Initially -1 to indicate it is empty.
    s8 held;

    // Prevents using the hold feature until the active piece is locked down
    u8 usedHold;

    // The bag of upcoming tetriminos. Technically this is two bags, the inactive bag gets shuffled when the current bag is almost out.
    // Only one array is used because it makes the logic simpler for the upcoming pieces display.
    u8 bag[TET_BAG_SIZE];

    // The index of the next tetrimino to pull from the bag.
    u8 bagIndex;

    // The y position directly underneath the active piece
    s8 floor;

    // Repeats movement if a direction is held
    u8 autoRepeatTimer;
};
