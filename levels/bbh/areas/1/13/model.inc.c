// 0x0700D178 - 0x0700D278
static const Vtx bbh_seg7_vertex_0700D178[] = {
    {{{ -2037,   1024,   2316}, 0, { -1470,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1863,   1126,   2142}, 0, {   990,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -2037,   1126,   2316}, 0, { -1470,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1310,    819,   1126}, 0, {  4980,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1310,    922,   1126}, 0, {  4980,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1310,    922,   1629}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1310,    819,   1629}, 0, {     0,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1310,    922,   1629}, 0, {   990,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{  -805,    922,   2032}, 0, { -5454,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{  -805,    819,   2032}, 0, { -5454,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1310,    819,   1629}, 0, {   990,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1863,   1024,   1126}, 0, { 10104,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1863,   1126,   2142}, 0, {     0,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1863,   1024,   2142}, 0, {     0,    990}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1863,   1126,   1126}, 0, { 10104,      0}, {0xff, 0xff, 0xff, 0xff}}},
    {{{ -1863,   1024,   2142}, 0, {   990,    990}, {0xff, 0xff, 0xff, 0xff}}},
};

// 0x0700D278 - 0x0700D2E0
static const Gfx bbh_seg7_dl_0700D278[] = {
    gsDPSetTextureImage(G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, spooky_09008000),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 32 * 32 - 1, CALC_DXT(32, G_IM_SIZ_16b_BYTES)),
    gsSPVertex(bbh_seg7_vertex_0700D178, 16, 0),
    gsSP2Triangles( 0,  1,  2, 0x0,  3,  4,  5, 0x0),
    gsSP2Triangles( 3,  5,  6, 0x0,  7,  8,  9, 0x0),
    gsSP2Triangles( 7,  9, 10, 0x0, 11, 12, 13, 0x0),
    gsSP2Triangles(11, 14, 12, 0x0,  0, 15,  1, 0x0),
    gsSPEndDisplayList(),
};

// 0x0700D2E0 - 0x0700D350
const Gfx bbh_seg7_dl_0700D2E0[] = {
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
    gsSPClearGeometryMode(G_LIGHTING | G_CULL_BACK | G_CULL_FRONT),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_ON),
    gsDPTileSync(),
    gsDPSetTile(G_IM_FMT_RGBA, G_IM_SIZ_16b, 8, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 5, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, 5, G_TX_NOLOD),
    gsDPSetTileSize(0, 0, 0, (32 - 1) << G_TEXTURE_IMAGE_FRAC, (32 - 1) << G_TEXTURE_IMAGE_FRAC),
    gsSPDisplayList(bbh_seg7_dl_0700D278),
    gsSPTexture(0xFFFF, 0xFFFF, 0, G_TX_RENDERTILE, G_OFF),
    gsDPPipeSync(),
    gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
    gsSPSetGeometryMode(G_LIGHTING | G_CULL_BACK),
    gsSPEndDisplayList(),
};
