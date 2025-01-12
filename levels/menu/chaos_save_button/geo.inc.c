#include "src/game/envfx_snow.h"

const GeoLayout chaos_save_button_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SCALE(LAYER_OPAQUE, 524288),
		GEO_OPEN_NODE(),
			GEO_ASM(0, geo_file_select_change_difficulty_color),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, chaos_save_button_save_button_mesh_layer_1),
			GEO_ASM(0, geo_file_select_change_challenge_texture),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, chaos_save_button_save_button_mesh_layer_6),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};

const GeoLayout chaos_save_button_score_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SCALE(LAYER_OPAQUE, 524288),
		GEO_OPEN_NODE(),
			GEO_ASM(0, geo_file_select_change_difficulty_color),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, chaos_save_button_score_button_mesh_layer_1),
			GEO_ASM(0, geo_file_select_change_challenge_texture),
			GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL, chaos_save_button_save_button_mesh_layer_6),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
