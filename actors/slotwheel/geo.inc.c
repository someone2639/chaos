#include "src/game/envfx_snow.h"

const GeoLayout slotwheel_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_BILLBOARD_WITH_PARAMS_AND_DL(LAYER_OPAQUE, 0, 0, 0, slotwheel_slotwheel_mesh_layer_1),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
