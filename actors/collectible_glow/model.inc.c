// Collectible Glow

ALIGNED8 static const Texture collectible_glow_texture[] = {
#include "actors/collectible_glow/custom_glow_quarter.ia16.inc.c"
};

ALIGNED8 static const u8 ray_texture[] = {
#include "actors/collectible_glow/custom_ray.ia16.inc.c"
};

const Gfx collectible_glow_bg_dl[] = {
    gsDPPipeSync(),
    gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b, 1, collectible_glow_texture),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_MIRROR, 5, G_TX_NOLOD, G_TX_MIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPEndDisplayList(),
};

const Gfx collectible_glow_ray_dl[] = {
	gsDPPipeSync(),
	gsDPSetTextureImage(G_IM_FMT_IA, G_IM_SIZ_16b_LOAD_BLOCK, 1, ray_texture),
	gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 32, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 7, 0),
    gsDPSetTileSize(0, 0, 0, (128 - 1) << G_TEXTURE_IMAGE_FRAC, (1 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsDPLoadSync(),
	gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 128 * 1 - 1, CALC_DXT(128, G_IM_SIZ_16b_BYTES)),
    gsSPEndDisplayList(),
};

const Gfx collectible_glow_dl_begin[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATEIA, G_CC_MODULATEIA),
    gsSPClearGeometryMode(G_LIGHTING),
    gsDPSetTile(G_IM_FMT_IA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsSPEndDisplayList(),
};

const Gfx collectible_glow_dl_end[] = {
    gsDPPipeSync(),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING),
    gsSPEndDisplayList(),
};
