#include "src/game/envfx_snow.h"

const GeoLayout squint_room_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, squint_room_squint_room_mesh_layer_1_with_revert),
		GEO_DISPLAY_LIST(LAYER_OPAQUE_INTER, squint_room_squint_room_mesh_layer_3_with_revert),
		GEO_DISPLAY_LIST(LAYER_OPAQUE_DECAL, squint_room_squint_room_mesh_layer_2_with_revert),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
