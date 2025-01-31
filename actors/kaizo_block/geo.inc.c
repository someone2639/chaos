#include "src/game/envfx_snow.h"

const GeoLayout kaizo_block_switch_opt1[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, kaizo_block_Object_mesh_layer_1_mat_override_hitTexture_0),
	GEO_CLOSE_NODE(),
	GEO_RETURN(),
};
const GeoLayout kaizo_block_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SWITCH_CASE(2, geo_switch_anim_state),
		GEO_OPEN_NODE(),
			GEO_NODE_START(),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, kaizo_block_Object_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_BRANCH(1, kaizo_block_switch_opt1),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
