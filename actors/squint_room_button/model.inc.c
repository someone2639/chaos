Lights1 squint_room_button_the_lights = gdSPDefLights1(
	0x7F, 0x7F, 0x7F,
	0xFF, 0xFF, 0xFF, 0x49, 0x49, 0x49);

Vtx squint_room_button_squint_room_button_mesh_layer_1_vtx_cull[8] = {
	{{ {-4, -4, 4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-4, 4, 4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-4, 4, -4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {-4, -4, -4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {4, -4, 4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {4, 4, 4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {4, 4, -4}, 0, {0, 0}, {0, 0, 0, 0} }},
	{{ {4, -4, -4}, 0, {0, 0}, {0, 0, 0, 0} }},
};

Vtx squint_room_button_squint_room_button_mesh_layer_1_vtx_0[40] = {
	{{ {0, -4, -4}, 0, {1008, 496}, {49, 0, 139, 255} }},
	{{ {0, 4, -4}, 0, {1008, -16}, {49, 0, 139, 255} }},
	{{ {3, 4, -3}, 0, {880, -16}, {49, 0, 139, 255} }},
	{{ {3, -4, -3}, 0, {880, 496}, {49, 0, 139, 255} }},
	{{ {3, -4, -3}, 0, {880, 496}, {117, 0, 207, 255} }},
	{{ {3, 4, -3}, 0, {880, -16}, {117, 0, 207, 255} }},
	{{ {4, 4, 0}, 0, {752, -16}, {117, 0, 207, 255} }},
	{{ {4, -4, 0}, 0, {752, 496}, {117, 0, 207, 255} }},
	{{ {4, -4, 0}, 0, {752, 496}, {117, 0, 49, 255} }},
	{{ {4, 4, 0}, 0, {752, -16}, {117, 0, 49, 255} }},
	{{ {3, 4, 3}, 0, {624, -16}, {117, 0, 49, 255} }},
	{{ {3, -4, 3}, 0, {624, 496}, {117, 0, 49, 255} }},
	{{ {3, -4, 3}, 0, {624, 496}, {49, 0, 117, 255} }},
	{{ {3, 4, 3}, 0, {624, -16}, {49, 0, 117, 255} }},
	{{ {0, 4, 4}, 0, {496, -16}, {49, 0, 117, 255} }},
	{{ {0, -4, 4}, 0, {496, 496}, {49, 0, 117, 255} }},
	{{ {0, -4, 4}, 0, {496, 496}, {207, 0, 117, 255} }},
	{{ {0, 4, 4}, 0, {496, -16}, {207, 0, 117, 255} }},
	{{ {-3, 4, 3}, 0, {368, -16}, {207, 0, 117, 255} }},
	{{ {-3, -4, 3}, 0, {368, 496}, {207, 0, 117, 255} }},
	{{ {-3, -4, 3}, 0, {368, 496}, {139, 0, 49, 255} }},
	{{ {-3, 4, 3}, 0, {368, -16}, {139, 0, 49, 255} }},
	{{ {-4, 4, 0}, 0, {240, -16}, {139, 0, 49, 255} }},
	{{ {-4, -4, 0}, 0, {240, 496}, {139, 0, 49, 255} }},
	{{ {4, 4, 0}, 0, {486, 752}, {0, 127, 0, 255} }},
	{{ {3, 4, -3}, 0, {414, 578}, {0, 127, 0, 255} }},
	{{ {0, 4, -4}, 0, {240, 506}, {0, 127, 0, 255} }},
	{{ {-4, 4, 0}, 0, {-6, 752}, {0, 127, 0, 255} }},
	{{ {-3, 4, -3}, 0, {66, 578}, {0, 127, 0, 255} }},
	{{ {0, 4, 4}, 0, {240, 998}, {0, 127, 0, 255} }},
	{{ {-3, 4, 3}, 0, {66, 926}, {0, 127, 0, 255} }},
	{{ {3, 4, 3}, 0, {414, 926}, {0, 127, 0, 255} }},
	{{ {-4, -4, 0}, 0, {240, 496}, {139, 0, 207, 255} }},
	{{ {-4, 4, 0}, 0, {240, -16}, {139, 0, 207, 255} }},
	{{ {-3, 4, -3}, 0, {112, -16}, {139, 0, 207, 255} }},
	{{ {-3, -4, -3}, 0, {112, 496}, {139, 0, 207, 255} }},
	{{ {-3, -4, -3}, 0, {112, 496}, {207, 0, 139, 255} }},
	{{ {-3, 4, -3}, 0, {112, -16}, {207, 0, 139, 255} }},
	{{ {0, 4, -4}, 0, {-16, -16}, {207, 0, 139, 255} }},
	{{ {0, -4, -4}, 0, {-16, 496}, {207, 0, 139, 255} }},
};

Gfx squint_room_button_squint_room_button_mesh_layer_1_tri_0[] = {
	gsSPVertex(squint_room_button_squint_room_button_mesh_layer_1_vtx_0 + 0, 32, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
	gsSP2Triangles(8, 9, 10, 0, 8, 10, 11, 0),
	gsSP2Triangles(12, 13, 14, 0, 12, 14, 15, 0),
	gsSP2Triangles(16, 17, 18, 0, 16, 18, 19, 0),
	gsSP2Triangles(20, 21, 22, 0, 20, 22, 23, 0),
	gsSP2Triangles(24, 25, 26, 0, 24, 26, 27, 0),
	gsSP2Triangles(26, 28, 27, 0, 27, 29, 24, 0),
	gsSP2Triangles(27, 30, 29, 0, 29, 31, 24, 0),
	gsSPVertex(squint_room_button_squint_room_button_mesh_layer_1_vtx_0 + 32, 8, 0),
	gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0),
	gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0),
	gsSPEndDisplayList(),
};


Gfx mat_squint_room_button_the[] = {
	gsSPSetLights1(squint_room_button_the_lights),
	gsDPPipeSync(),
	gsDPSetCombineLERP(ENVIRONMENT, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT, ENVIRONMENT, 0, SHADE, 0, 0, 0, 0, ENVIRONMENT),
	gsDPSetAlphaDither(G_AD_NOISE),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPEndDisplayList(),
};

Gfx mat_revert_squint_room_button_the[] = {
	gsDPPipeSync(),
	gsDPSetAlphaDither(G_AD_DISABLE),
	gsSPEndDisplayList(),
};

Gfx squint_room_button_squint_room_button_mesh_layer_1[] = {
	gsSPClearGeometryMode(G_LIGHTING),
	gsSPVertex(squint_room_button_squint_room_button_mesh_layer_1_vtx_cull + 0, 8, 0),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPCullDisplayList(0, 7),
	gsSPDisplayList(mat_squint_room_button_the),
	gsSPDisplayList(squint_room_button_squint_room_button_mesh_layer_1_tri_0),
	gsSPDisplayList(mat_revert_squint_room_button_the),
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

