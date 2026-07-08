#include "src/game/envfx_snow.h"

const GeoLayout squintroom_dpad_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, squintroom_dpad_squintroom_dpad_mesh_layer_1),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, squintroom_dpad_final_revert_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
