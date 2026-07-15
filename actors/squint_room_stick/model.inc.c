Lights1 squint_room_stick_stick_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFF, 0xFF, 0xFF, 0x49, 0x49, 0x49);

Vtx squint_room_stick_squint_room_stick_mesh_layer_1_vtx_cull[8] = {
	{{ {-5, 0, 5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-5, 17, 5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-5, 17, -5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-5, 0, -5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5, 0, 5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5, 17, 5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5, 17, -5}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {5, 0, -5}, 0, {0, 0}, {0, 0, 0, 0} }},
};

Vtx squint_room_stick_squint_room_stick_mesh_layer_1_vtx_0[37] = {
	{{ {0, 15, -5}, 0, {1008, 496}, {0, 0, 129, 255} }},
	{{ {0, 17, -5}, 0, {1008, -16}, {0, 80, 157, 255} }},
	{{ {4, 17, -4}, 0, {880, -16}, {70, 80, 186, 255} }},
	{{ {4, 15, -4}, 0, {880, 496}, {90, 0, 166, 255} }},
	{{ {5, 17, 0}, 0, {752, -16}, {99, 80, 0, 255} }},
	{{ {5, 15, 0}, 0, {752, 496}, {127, 0, 0, 255} }},
	{{ {4, 17, 4}, 0, {624, -16}, {70, 80, 70, 255} }},
	{{ {4, 15, 4}, 0, {624, 496}, {90, 0, 90, 255} }},
	{{ {0, 17, 5}, 0, {496, -16}, {0, 80, 99, 255} }},
	{{ {0, 15, 5}, 0, {496, 496}, {0, 0, 127, 255} }},
	{{ {-4, 17, 4}, 0, {368, -16}, {186, 80, 70, 255} }},
	{{ {-4, 15, 4}, 0, {368, 496}, {166, 0, 90, 255} }},
	{{ {-5, 17, 0}, 0, {240, -16}, {157, 80, 0, 255} }},
	{{ {-5, 15, 0}, 0, {240, 496}, {129, 0, 0, 255} }},
	{{ {-4, 17, -4}, 0, {112, -16}, {186, 80, 186, 255} }},
	{{ {-4, 15, -4}, 0, {112, 496}, {166, 0, 166, 255} }},
	{{ {0, 17, -5}, 0, {-16, -16}, {0, 80, 157, 255} }},
	{{ {0, 15, -5}, 0, {-16, 496}, {0, 0, 129, 255} }},
	{{ {5, 17, 0}, 0, {486, 752}, {0, 127, 0, 255} }},
	{{ {4, 17, -4}, 0, {414, 578}, {0, 127, 0, 255} }},
	{{ {0, 17, -5}, 0, {240, 506}, {0, 127, 0, 255} }},
	{{ {-5, 17, 0}, 0, {-6, 752}, {0, 127, 0, 255} }},
	{{ {-4, 17, -4}, 0, {66, 578}, {0, 127, 0, 255} }},
	{{ {0, 17, 5}, 0, {240, 998}, {0, 127, 0, 255} }},
	{{ {-4, 17, 4}, 0, {66, 926}, {0, 127, 0, 255} }},
	{{ {4, 17, 4}, 0, {414, 926}, {0, 127, 0, 255} }},
	{{ {0, 15, -3}, 0, {240, 506}, {0, 231, 131, 255} }},
	{{ {0, 0, 0}, 0, {240, 752}, {0, 129, 0, 255} }},
	{{ {-2, 15, -2}, 0, {432, 599}, {159, 231, 178, 255} }},
	{{ {2, 15, -2}, 0, {48, 599}, {97, 231, 178, 255} }},
	{{ {3, 15, 1}, 0, {0, 807}, {121, 231, 28, 255} }},
	{{ {1, 15, 3}, 0, {133, 973}, {54, 231, 112, 255} }},
	{{ {-1, 15, 3}, 0, {347, 973}, {202, 231, 112, 255} }},
	{{ {0, 0, 0}, 0, {240, 752}, {0, 129, 0, 255} }},
	{{ {1, 15, 3}, 0, {133, 973}, {54, 231, 112, 255} }},
	{{ {-3, 15, 1}, 0, {480, 807}, {135, 231, 28, 255} }},
	{{ {-2, 15, -2}, 0, {432, 599}, {159, 231, 178, 255} }},
};

Gfx squint_room_stick_squint_room_stick_mesh_layer_1_tri_0[] = {
	gsSPVertex(squint_room_stick_squint_room_stick_mesh_layer_1_vtx_0 + 0, 32, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSP2Triangles(3, 2, 4, 0, 3, 4, 5, 0),
	gsSP2Triangles(5, 4, 6, 0, 5, 6, 7, 0),
	gsSP2Triangles(7, 6, 8, 0, 7, 8, 9, 0),
	gsSP2Triangles(9, 8, 10, 0, 9, 10, 11, 0),
	gsSP2Triangles(11, 10, 12, 0, 11, 12, 13, 0),
	gsSP2Triangles(13, 12, 14, 0, 13, 14, 15, 0),
	gsSP2Triangles(15, 14, 16, 0, 15, 16, 17, 0),
	gsSP2Triangles(18, 19, 20, 0, 18, 20, 21, 0),
	gsSP2Triangles(20, 22, 21, 0, 21, 23, 18, 0),
	gsSP2Triangles(21, 24, 23, 0, 23, 25, 18, 0),
	gsSP2Triangles(26, 27, 28, 0, 29, 27, 26, 0),
	gsSP2Triangles(30, 27, 29, 0, 31, 27, 30, 0),
	gsSPVertex(squint_room_stick_squint_room_stick_mesh_layer_1_vtx_0 + 32, 5, 0),
	gsSP2Triangles(0, 1, 2, 0, 3, 1, 0, 0),
	gsSP1Triangle(4, 1, 3, 0),
	gsSPEndDisplayList(),
};


Gfx mat_squint_room_stick_stick[] = {
	gsSPSetLights1(squint_room_stick_stick_lights),
	gsDPPipeSync(),
	gsDPSetCombineLERP(ENVIRONMENT, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT, ENVIRONMENT, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT),
	gsDPSetAlphaDither(G_AD_NOISE),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsDPSetEnvColor(225, 225, 225, 255),
	gsSPEndDisplayList(),
};

Gfx mat_revert_squint_room_stick_stick[] = {
	gsDPPipeSync(),
	gsDPSetAlphaDither(G_AD_DISABLE),
	gsSPEndDisplayList(),
};

Gfx squint_room_stick_squint_room_stick_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(squint_room_stick_squint_room_stick_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_squint_room_stick_stick),
	gsSPDisplayList(squint_room_stick_squint_room_stick_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_squint_room_stick_stick),
	gsSPEndDisplayList(),
};

Gfx squint_room_stick_final_revert_mesh_layer_1[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

