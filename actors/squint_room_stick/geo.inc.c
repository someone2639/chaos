#include "src/game/envfx_snow.h"

const GeoLayout squint_room_stick_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, squint_room_stick_squint_room_stick_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
