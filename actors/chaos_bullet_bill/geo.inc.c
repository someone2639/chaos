#include "src/game/envfx_snow.h"

const GeoLayout chaos_bullet_bill_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_NODE_START(),
		GEO_OPEN_NODE(),
			GEO_SHADOW(0, 400, 700),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE, chaos_bullet_bill_000_displaylist_mesh_layer_1),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
