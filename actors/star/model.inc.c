// Star

// 0x0302A6D8
static const Lights1 star_seg3_lights_0302A6D8 = gdSPDefLights1(
    0x3f, 0x3f, 0x3f,
    0xff, 0xff, 0xff, 0x28, 0x28, 0x28
);

// 0x0302A6F0
ALIGNED8 static const Texture star_seg3_texture_0302A6F0[] = {
#include "actors/star/star_surface.rgba16.inc.c"
};

// 0x0302AEF0
ALIGNED8 static const Texture star_seg3_texture_0302AEF0[] = {
#include "actors/star/star_eye.rgba16.inc.c"
};

Gfx rainbow_star_star_surface_specular_i8_aligner[] = {gsSPEndDisplayList()};
u8 rainbow_star_star_surface_specular_i8[] = {
	#include "actors/star/star_surface_specular_custom.i8.inc.c"
};

Gfx rainbow_star_star_surface_rainbow_rgba16_aligner[] = {gsSPEndDisplayList()};
u8 rainbow_star_star_surface_rainbow_rgba16[] = {
	#include "actors/star/star_surface_rainbow_custom.rgba16.inc.c"
};

Vtx rainbow_star_skinned_mesh_vtx_0[20] = {
	{{ {0, 8, -89}, 0, {-8, -14}, {0, 3, 129, 255} }},
	{{ {190, -201, 0}, 0, {-401, -372}, {86, 162, 0, 255} }},
	{{ {0, -129, 0}, 0, {-265, -14}, {0, 129, 0, 255} }},
	{{ {146, -42, 0}, 0, {-102, -289}, {125, 233, 0, 255} }},
	{{ {257, 84, 0}, 0, {135, -498}, {119, 44, 0, 255} }},
	{{ {96, 99, 0}, 0, {163, -195}, {55, 114, 0, 255} }},
	{{ {0, 246, 0}, 0, {440, -14}, {0, 127, 0, 255} }},
	{{ {-95, 99, 0}, 0, {163, 164}, {200, 114, 0, 255} }},
	{{ {-256, 84, 0}, 0, {135, 467}, {137, 44, 0, 255} }},
	{{ {-145, -42, 0}, 0, {-102, 258}, {131, 233, 0, 255} }},
	{{ {-189, -201, 0}, 0, {-401, 341}, {171, 162, 0, 255} }},
	{{ {190, -201, 0}, 0, {-401, 343}, {86, 162, 0, 255} }},
	{{ {0, 8, 90}, 0, {-8, -14}, {0, 3, 127, 255} }},
	{{ {146, -42, 0}, 0, {-102, 260}, {125, 233, 0, 255} }},
	{{ {257, 84, 0}, 0, {135, 469}, {119, 44, 0, 255} }},
	{{ {96, 99, 0}, 0, {163, 166}, {55, 114, 0, 255} }},
	{{ {-95, 99, 0}, 0, {163, -193}, {200, 114, 0, 255} }},
	{{ {-256, 84, 0}, 0, {135, -496}, {137, 44, 0, 255} }},
	{{ {-145, -42, 0}, 0, {-102, -287}, {131, 233, 0, 255} }},
	{{ {-189, -201, 0}, 0, {-401, -370}, {171, 162, 0, 255} }},
};

Gfx rainbow_star_skinned_mesh_tri_0[] = {
	gsSPVertex(rainbow_star_skinned_mesh_vtx_0 + 0, 20, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
	gsSP2Triangles(4, 3, 0, 0, 5, 4, 0, 0),
	gsSP2Triangles(5, 0, 6, 0, 0, 7, 6, 0),
	gsSP2Triangles(0, 8, 7, 0, 0, 9, 8, 0),
	gsSP2Triangles(0, 10, 9, 0, 0, 2, 10, 0),
	gsSP2Triangles(2, 11, 12, 0, 11, 13, 12, 0),
	gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
	gsSP2Triangles(6, 12, 15, 0, 6, 16, 12, 0),
	gsSP2Triangles(16, 17, 12, 0, 17, 18, 12, 0),
	gsSP2Triangles(18, 19, 12, 0, 19, 2, 12, 0),
	gsSPEndDisplayList(),
};

Vtx rainbow_star_skinned_mesh_vtx_1[22] = {
	{{ {0, 8, -89}, 0, {469, 496}, {255, 255, 255, 255} }},
	{{ {190, -201, 0}, 0, {19, 866}, {255, 255, 255, 255} }},
	{{ {0, -129, 0}, 0, {174, 496}, {255, 255, 255, 255} }},
	{{ {146, -42, 0}, 0, {361, 780}, {255, 255, 255, 255} }},
	{{ {257, 84, 0}, 0, {632, 996}, {255, 255, 255, 255} }},
	{{ {96, 99, 0}, 0, {665, 683}, {255, 255, 255, 255} }},
	{{ {0, 246, 0}, 0, {981, 496}, {255, 255, 255, 255} }},
	{{ {-95, 99, 0}, 0, {665, 311}, {255, 255, 255, 255} }},
	{{ {-256, 84, 0}, 0, {632, -2}, {255, 255, 255, 255} }},
	{{ {-145, -42, 0}, 0, {361, 214}, {255, 255, 255, 255} }},
	{{ {-189, -201, 0}, 0, {19, 129}, {255, 255, 255, 255} }},
	{{ {0, -129, 0}, 0, {206, 498}, {255, 255, 255, 255} }},
	{{ {190, -201, 0}, 0, {51, 867}, {255, 255, 255, 255} }},
	{{ {0, 8, 90}, 0, {501, 498}, {255, 255, 255, 255} }},
	{{ {146, -42, 0}, 0, {393, 782}, {255, 255, 255, 255} }},
	{{ {257, 84, 0}, 0, {664, 998}, {255, 255, 255, 255} }},
	{{ {96, 99, 0}, 0, {697, 685}, {255, 255, 255, 255} }},
	{{ {0, 246, 0}, 0, {1013, 498}, {255, 255, 255, 255} }},
	{{ {-95, 99, 0}, 0, {697, 313}, {255, 255, 255, 255} }},
	{{ {-256, 84, 0}, 0, {664, 0}, {255, 255, 255, 255} }},
	{{ {-145, -42, 0}, 0, {393, 216}, {255, 255, 255, 255} }},
	{{ {-189, -201, 0}, 0, {51, 131}, {255, 255, 255, 255} }},
};

Gfx rainbow_star_skinned_mesh_tri_1[] = {
	gsSPVertex(rainbow_star_skinned_mesh_vtx_1 + 0, 22, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0),
	gsSP2Triangles(4, 3, 0, 0, 5, 4, 0, 0),
	gsSP2Triangles(5, 0, 6, 0, 0, 7, 6, 0),
	gsSP2Triangles(0, 8, 7, 0, 0, 9, 8, 0),
	gsSP2Triangles(0, 10, 9, 0, 0, 2, 10, 0),
	gsSP2Triangles(11, 12, 13, 0, 12, 14, 13, 0),
	gsSP2Triangles(13, 14, 15, 0, 13, 15, 16, 0),
	gsSP2Triangles(17, 13, 16, 0, 17, 18, 13, 0),
	gsSP2Triangles(18, 19, 13, 0, 19, 20, 13, 0),
	gsSP2Triangles(20, 21, 13, 0, 21, 11, 13, 0),
	gsSPEndDisplayList(),
};

Gfx mat_rainbow_star_rainbow_star_specular[] = {
	gsSPGeometryMode(G_SHADE, G_TEXTURE_GEN),
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT),
	gsDPSetAlphaDither(G_AD_NOISE),
	gsSPTexture(1984, 1984, 0, 0, 1),
	gsDPSetEnvColor(255, 255, 255, 48),
	gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 1, rainbow_star_star_surface_specular_i8),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadBlock(7, 0, 0, 511, 512),
	gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 4, 0, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 5, 0, G_TX_CLAMP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPEndDisplayList(),
};

Gfx mat_revert_rainbow_star_rainbow_star_specular[] = {
	gsSPGeometryMode(G_TEXTURE_GEN, G_SHADE),
	gsDPPipeSync(),
	gsDPSetAlphaDither(G_AD_DISABLE),
	gsSPEndDisplayList(),
};

Gfx mat_rainbow_star_rainbow_star_surface[] = {
	gsSPGeometryMode(G_SHADE | G_LIGHTING, 0),
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT, 0, 0, 0, TEXEL0, 0, 0, 0, ENVIRONMENT),
	gsDPSetAlphaDither(G_AD_NOISE),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 1, rainbow_star_star_surface_rainbow_rgba16),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b_LOAD_BLOCK, 0, 0, 7, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 0, 0),
	gsDPLoadBlock(7, 0, 0, 1023, 256),
	gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, 0, 0, G_TX_CLAMP | G_TX_NOMIRROR, 5, 0, G_TX_CLAMP | G_TX_NOMIRROR, 5, 0),
	gsDPSetTileSize(0, 0, 0, 124, 124),
	gsSPEndDisplayList(),
};

Gfx mat_revert_rainbow_star_rainbow_star_surface[] = {
	gsSPGeometryMode(0, G_SHADE | G_LIGHTING),
	gsDPPipeSync(),
	gsDPSetAlphaDither(G_AD_DISABLE),
	gsSPEndDisplayList(),
};

// 0x0302B6F0
static const Vtx star_seg3_vertex_0302B6F0[] = {
    {{{     0,      8,    -89}, 0, {     0,      0}, {0x00, 0x07, 0x82, 0xff}}},
    {{{   190,   -201,      0}, 0, {     0,      0}, {0x55, 0xa3, 0x00, 0xff}}},
    {{{     0,   -129,      0}, 0, {     0,      0}, {0x00, 0x82, 0x00, 0xff}}},
    {{{   146,    -42,      0}, 0, {     0,      0}, {0x7c, 0xe7, 0x00, 0xff}}},
    {{{     0,      8,     90}, 0, {     0,      0}, {0x00, 0x07, 0x7e, 0xff}}},
    {{{   257,     84,      0}, 0, {     0,      0}, {0x68, 0x47, 0x00, 0xff}}},
    {{{  -145,    -42,      0}, 0, {     0,      0}, {0x84, 0xe7, 0x00, 0xff}}},
    {{{  -189,   -201,      0}, 0, {     0,      0}, {0xaa, 0xa3, 0x00, 0xff}}},
    {{{  -256,     84,      0}, 0, {     0,      0}, {0x97, 0x47, 0x00, 0xff}}},
    {{{     0,    246,      0}, 0, {     0,      0}, {0x00, 0x7e, 0x00, 0xff}}},
    {{{    96,     99,      0}, 0, {     0,      0}, {0x3d, 0x6f, 0x00, 0xff}}},
    {{{   -95,     99,      0}, 0, {     0,      0}, {0xc3, 0x6f, 0x00, 0xff}}},
};

// 0x0302B7B0 - 0x0302B870
const Gfx star_seg3_dl_0302B7B0[] = {
    gsSPLight(&star_seg3_lights_0302A6D8.l, 1),
    gsSPLight(&star_seg3_lights_0302A6D8.a, 2),
    gsSPVertex(star_seg3_vertex_0302B6F0, 12, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 2,  1,  4, 0x0,  1,  3,  4, 0x0),
    gsSP2Triangles( 5,  3,  0, 0x0,  4,  3,  5, 0x0),
    gsSP2Triangles( 6,  7,  4, 0x0,  7,  2,  4, 0x0),
    gsSP2Triangles( 8,  6,  4, 0x0,  9,  4, 10, 0x0),
    gsSP2Triangles( 9, 11,  4, 0x0,  4,  5, 10, 0x0),
    gsSP2Triangles(11,  8,  4, 0x0,  0,  2,  7, 0x0),
    gsSP2Triangles( 0,  7,  6, 0x0,  0,  6,  8, 0x0),
    gsSP2Triangles( 0,  8, 11, 0x0,  0, 11,  9, 0x0),
    gsSP2Triangles(10,  5,  0, 0x0, 10,  0,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302B870 - 0x0302B908
const Gfx star_seg3_dl_0302B870[] = {
    gsDPPipeSync(),
    gsSPSetGeometryMode(G_TEXTURE_GEN),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_DECALFADE, G_CC_DECALFADE),
    gsDPLoadTextureBlock(star_seg3_texture_0302A6F0, G_IM_FMT_RGBA, G_IM_SIZ_16b, 32, 64, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_WRAP | G_TX_NOMIRROR, 5, 6, G_TX_NOLOD, G_TX_NOLOD), //! Dimensions loaded as 32x64 despite this texture having only 32x32 dimensions, harmless due to environment mapping (G_TEXTURE_GEN & gsSPTexture values)
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_ON),
    gsSPDisplayList(star_seg3_dl_0302B7B0),
    gsDPPipeSync(),
    gsSPTexture(0x07C0, 0x07C0, 0, G_TX_RENDERTILE, G_OFF),
    gsSPClearGeometryMode(G_TEXTURE_GEN),
    gsDPSetEnvColor(255, 255, 255, 255),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPEndDisplayList(),
};

const Gfx rainbow_star_dl_spec[] = {
    gsSPDisplayList(mat_rainbow_star_rainbow_star_specular),
	gsSPDisplayList(rainbow_star_skinned_mesh_tri_0),
	gsSPDisplayList(mat_revert_rainbow_star_rainbow_star_specular),
    gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsSPEndDisplayList(),
};

// 0x0302B870 - 0x0302B908
const Gfx rainbow_star_dl[] = {
	gsSPDisplayList(mat_rainbow_star_rainbow_star_surface),
	gsSPDisplayList(rainbow_star_skinned_mesh_tri_1),
	gsSPDisplayList(mat_revert_rainbow_star_rainbow_star_surface),
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsSPEndDisplayList(),
};

// 0x0302B908
static const Lights1 star_seg3_lights_0302B908 = gdSPDefLights1(
    0x3f, 0x3f, 0x3f,
    0xff, 0xff, 0xff, 0x28, 0x28, 0x28
);

// 0x0302B920
static const Vtx star_seg3_vertex_0302B920[] = {
    {{{    67,     86,     63}, 0, {   990,      0}, {0x18, 0x15, 0x7a, 0xff}}},
    {{{     0,    -27,     96}, 0, {     0,    990}, {0x18, 0x15, 0x7a, 0xff}}},
    {{{    67,    -29,     83}, 0, {   990,    990}, {0x18, 0x15, 0x7a, 0xff}}},
    {{{     0,     89,     76}, 0, {     0,      0}, {0x18, 0x15, 0x7a, 0xff}}},
    {{{     0,     89,     76}, 0, {   990,      0}, {0xe8, 0x15, 0x7a, 0xff}}},
    {{{   -66,    -29,     83}, 0, {     0,    990}, {0xe8, 0x15, 0x7a, 0xff}}},
    {{{     0,    -27,     96}, 0, {   990,    990}, {0xe8, 0x15, 0x7a, 0xff}}},
    {{{     0,     89,     76}, 0, {   990,      0}, {0xe7, 0x15, 0x7a, 0xff}}},
    {{{   -66,     86,     63}, 0, {     0,      0}, {0xe7, 0x15, 0x7a, 0xff}}},
    {{{   -66,    -29,     83}, 0, {     0,    990}, {0xe7, 0x15, 0x7a, 0xff}}},
};

// 0x0302B9C0 - 0x0302BA18
const Gfx star_seg3_dl_0302B9C0[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, star_seg3_texture_0302AEF0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPLight(&star_seg3_lights_0302B908.l, 1),
    gsSPLight(&star_seg3_lights_0302B908.a, 2),
    gsSPVertex(star_seg3_vertex_0302B920, 10, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  0,  3,  1, 0x0),
    gsSP2Triangles( 4,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSPEndDisplayList(),
};

// 0x0302BA18 - 0x0302BA88
const Gfx star_seg3_dl_0302BA18[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_MODULATERGBA, G_CC_MODULATERGBA),
    gsSPClearGeometryMode(G_SHADING_SMOOTH),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_CLAMP, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(star_seg3_dl_0302B9C0),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_SHADING_SMOOTH),
    gsSPEndDisplayList(),
};
