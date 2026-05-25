ALIGNED8 static const Texture wheel_border_quarter_image[] = {
#include "actors/gambling_wheel/custom_wheel_border_quarter.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture border_6_25_image[] = {
#include "actors/gambling_wheel/custom_border_6.25.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture border_12_5_image[] = {
#include "actors/gambling_wheel/custom_border_12.5.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture border_18_75_image[] = {
#include "actors/gambling_wheel/custom_border_18.75.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture border_25_image[] = {
#include "actors/gambling_wheel/custom_border_25.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture wedge_6_25_image[] = {
#include "actors/gambling_wheel/custom_wedge_6.25.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture wedge_12_5_image[] = {
#include "actors/gambling_wheel/custom_wedge_12.5.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture wedge_18_75_image[] = {
#include "actors/gambling_wheel/custom_wedge_18.75.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture wedge_25_image[] = {
#include "actors/gambling_wheel/custom_wedge_25.ia4.preswap.inc.c"
};

ALIGNED8 static const Texture icon_none[] = {
#include "actors/gambling_wheel/custom_icon_none.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_timer[] = {
#include "actors/gambling_wheel/custom_icon_timer.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_stars_3[] = {
#include "actors/gambling_wheel/custom_icon_stars_3.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_stars_1[] = {
#include "actors/gambling_wheel/custom_icon_stars_1.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_stars_m1[] = {
#include "actors/gambling_wheel/custom_icon_stars_m1.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_stars_m2[] = {
#include "actors/gambling_wheel/custom_icon_stars_m2.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_lives_12[] = {
#include "actors/gambling_wheel/custom_icon_lives_12.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_lives_5[] = {
#include "actors/gambling_wheel/custom_icon_lives_5.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_lives_m3[] = {
#include "actors/gambling_wheel/custom_icon_lives_m3.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_lives_m6[] = {
#include "actors/gambling_wheel/custom_icon_lives_m6.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_rank_1[] = {
#include "actors/gambling_wheel/custom_icon_rank_1.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_rank_2[] = {
#include "actors/gambling_wheel/custom_icon_rank_2.rgba16.inc.c"
};

ALIGNED8 static const Texture icon_rank_3[] = {
#include "actors/gambling_wheel/custom_icon_rank_3.rgba16.inc.c"
};

static Vtx wheel_border_quarter_vtx[16] = {
	{{ {-96,   0, 0}, 0, { 0 << 5, 84 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,   0, 0}, 0, {96 << 5, 84 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {96 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-96,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,   0, 0}, 0, {96 << 5, 84 << 5}, {  0,   0,   0, 255} }},
	{{ { 96,   0, 0}, 0, { 0 << 5, 84 << 5}, {  0,   0,   0, 255} }},
	{{ { 96,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {96 << 5,  0 << 5}, {  0,   0,   0, 255} }},
    
	{{ {-96, -84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0, -84, 0}, 0, {96 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,   0, 0}, 0, {96 << 5, 84 << 5}, {  0,   0,   0, 255} }},
	{{ {-96,   0, 0}, 0, { 0 << 5, 84 << 5}, {  0,   0,   0, 255} }},

	{{ {  0, -84, 0}, 0, {96 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ { 96, -84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ { 96,   0, 0}, 0, { 0 << 5, 84 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,   0, 0}, 0, {96 << 5, 84 << 5}, {  0,   0,   0, 255} }},
};

static Vtx border_6_25_vtx[8] = {
	{{ {-32,  77, 0}, 0, { 0 << 5,  7 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  77, 0}, 0, {32 << 5,  7 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-32,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  77, 0}, 0, {32 << 5,  7 << 5}, {  0,   0,   0, 255} }},
	{{ { 32,  77, 0}, 0, { 0 << 5,  7 << 5}, {  0,   0,   0, 255} }},
	{{ { 32,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx border_12_5_vtx[8] = {
	{{ {-48,  72, 0}, 0, { 0 << 5, 12 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  72, 0}, 0, {48 << 5, 12 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {48 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-48,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  72, 0}, 0, {48 << 5, 12 << 5}, {  0,   0,   0, 255} }},
	{{ { 48,  72, 0}, 0, { 0 << 5, 12 << 5}, {  0,   0,   0, 255} }},
	{{ { 48,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {48 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx border_18_75_vtx[8] = {
	{{ {-48,  65, 0}, 0, { 0 << 5, 19 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  65, 0}, 0, {48 << 5, 19 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {48 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-48,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  65, 0}, 0, {48 << 5, 19 << 5}, {  0,   0,   0, 255} }},
	{{ { 48,  65, 0}, 0, { 0 << 5, 19 << 5}, {  0,   0,   0, 255} }},
	{{ { 48,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {48 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx border_25_vtx[8] = {
	{{ {-64,  53, 0}, 0, { 0 << 5, 31 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  53, 0}, 0, {64 << 5, 31 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {64 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-64,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  53, 0}, 0, {64 << 5, 31 << 5}, {  0,   0,   0, 255} }},
	{{ { 64,  53, 0}, 0, { 0 << 5, 31 << 5}, {  0,   0,   0, 255} }},
	{{ { 64,  84, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  84, 0}, 0, {64 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx wedge_6_25_vtx[8] = {
	{{ {-16,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  -1, 0}, 0, {16 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  82, 0}, 0, {16 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-16,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  -1, 0}, 0, {16 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 16,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 16,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  82, 0}, 0, {16 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx wedge_12_5_vtx[8] = {
	{{ {-32,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  -1, 0}, 0, {32 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  82, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-32,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  -1, 0}, 0, {32 << 5, 83 << 5}, {127, 127, 127, 255}}},
	{{ { 32,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 32,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  82, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx wedge_18_75_vtx[8] = {
	{{ {-48,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  -1, 0}, 0, {48 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  82, 0}, 0, {48 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-48,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  -1, 0}, 0, {48 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 48,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 48,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  82, 0}, 0, {48 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx wedge_25_vtx[8] = {
	{{ {-64,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  -1, 0}, 0, {64 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ {  0,  82, 0}, 0, {64 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-64,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},

	{{ {  0,  -1, 0}, 0, {64 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 64,  -1, 0}, 0, { 0 << 5, 83 << 5}, {127, 127, 127, 255} }},
	{{ { 64,  82, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {  0,  82, 0}, 0, {64 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx border_line_slice_vtx[4] = {
	{{ { -1,  -1, 0}, 0, { 0 << 5, 84 << 5}, {127, 127, 127, 255} }},
	{{ {  1,  -1, 0}, 0, { 4 << 5, 84 << 5}, {127, 127, 127, 255} }},
	{{ {  1,  83, 0}, 0, { 4 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ { -1,  83, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

// static Vtx icon_48_48[4] = {
// 	{{ {-24, -24, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
// 	{{ { 24, -24, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
// 	{{ { 24,  24, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
// 	{{ {-24,  24, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
// };

// static Vtx icon_40_40[4] = {
// 	{{ {-20, -20, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
// 	{{ { 20, -20, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
// 	{{ { 20,  20, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
// 	{{ {-20,  20, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
// };

static Vtx icon_36_36[4] = {
	{{ {-18, -18, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 18, -18, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 18,  18, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-18,  18, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx icon_32_32[4] = {
	{{ {-16, -16, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 16, -16, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 16,  16, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-16,  16, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx icon_28_28[4] = {
	{{ {-14, -14, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 14, -14, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 14,  14, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-14,  14, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx icon_24_24[4] = {
	{{ {-12, -12, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 12, -12, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 12,  12, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-12,  12, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx icon_20_20[4] = {
	{{ {-10, -10, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 10, -10, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ { 10,  10, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ {-10,  10, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

static Vtx icon_18_18[4] = {
	{{ { -9,  -9, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ {  9,  -9, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
	{{ {  9,   9, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
	{{ { -9,   9, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
};

// static Vtx icon_16_16[4] = {
// 	{{ { -6,  -6, 0}, 0, { 0 << 5, 32 << 5}, {  0,   0,   0, 255} }},
// 	{{ {  6,  -6, 0}, 0, {32 << 5, 32 << 5}, {  0,   0,   0, 255} }},
// 	{{ {  6,   6, 0}, 0, {32 << 5,  0 << 5}, {  0,   0,   0, 255} }},
// 	{{ { -6,   6, 0}, 0, { 0 << 5,  0 << 5}, {  0,   0,   0, 255} }},
// };

Gfx gambling_wheel_dl_wheel_border_quarter[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(wheel_border_quarter_image, G_IM_FMT_IA, 96, 84, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(wheel_border_quarter_vtx, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  2,  3, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  4,  6,  7, 0x0),
    gsSP2Triangles( 8,  9, 10, 0x0,  8, 10, 11, 0x0),
    gsSP2Triangles(12, 13, 14, 0x0, 12, 14, 15, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_6_25[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(border_6_25_image, G_IM_FMT_IA, 32, 7, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(border_6_25_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_12_5[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(border_12_5_image, G_IM_FMT_IA, 48, 12, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(border_12_5_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_18_75[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(border_18_75_image, G_IM_FMT_IA, 48, 19, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(border_18_75_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_25[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(border_25_image, G_IM_FMT_IA, 64, 31, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(border_25_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_wedge_6_25[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(wedge_6_25_image, G_IM_FMT_IA, 16, 83, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(wedge_6_25_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_wedge_12_5[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(wedge_12_5_image, G_IM_FMT_IA, 32, 83, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(wedge_12_5_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_wedge_18_75[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(wedge_18_75_image, G_IM_FMT_IA, 48, 83, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(wedge_18_75_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_wedge_25[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock_4bS(wedge_25_image, G_IM_FMT_IA, 64, 83, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(wedge_25_vtx, 8, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSP2Triangles(4, 5, 6, 0x0, 4, 6, 7, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_start[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetCombineLERP(TEXEL0, SHADE, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, SHADE, PRIMITIVE, 0, 0, 0, 0, TEXEL0),
    // gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_end[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_line_slice_start[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetCombineLERP(1, SHADE, PRIMITIVE, 0, 0, 0, 0, 1, 1, SHADE, PRIMITIVE, 0, 0, 0, 0, 1),
    // gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPPipeSync(),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_line_slice[] = {
    gsDPPipeSync(),
    gsSPVertex(border_line_slice_vtx, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_border_line_slice_end[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_none_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_none, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_36_36, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_none_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_none, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_24_24, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_timer_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_timer, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_28_28, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_timer_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_timer, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_18_18, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_3_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_3, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_3_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_3, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_1_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_1_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_m1_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_m1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_m1_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_m1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_m2_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_m2, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_stars_m2_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_stars_m2, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_12_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_12, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_12_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_12, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_5_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_5, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_5_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_5, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_m3_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_m3, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_m3_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_m3, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_m6_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_m6, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_lives_m6_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_lives_m6, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_rank_1_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_rank_1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_rank_1_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_rank_1, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_rank_2_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_rank_2, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_rank_2_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_rank_2, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_rank_3_big[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_rank_3, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_32_32, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_rank_3_small[] = {
    gsDPPipeSync(),
    gsDPLoadTextureBlock(icon_rank_3, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 32, 0, G_TX_MIRROR | G_TX_WRAP, G_TX_MIRROR | G_TX_WRAP, 5, 5, G_TX_NOLOD, G_TX_NOLOD),
    gsSPVertex(icon_20_20, 4, 0),
    gsSP2Triangles(0, 1, 2, 0x0, 0, 2, 3, 0x0),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_start[] = {
    gsDPPipeSync(),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetCombineLERP(TEXEL0, SHADE, PRIMITIVE, 0, 0, 0, 0, TEXEL0, TEXEL0, SHADE, PRIMITIVE, 0, 0, 0, 0, TEXEL0),
    gsDPSetTextureFilter(G_TF_POINT),
    gsDPSetRenderMode(G_RM_TEX_EDGE, G_RM_TEX_EDGE2),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsSPEndDisplayList(),
};

Gfx gambling_wheel_dl_icon_end[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsDPSetTextureFilter(G_TF_BILERP),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsDPSetPrimColor(0, 0, 255, 255, 255, 255),
    gsSPEndDisplayList(),
};
