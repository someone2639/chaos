#include "src/game/envfx_snow.h"

const GeoLayout inert_star_transparent_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SHADOW(0, 155, 100),
		GEO_OPEN_NODE(),
			GEO_SCALE(LAYER_FORCE, 16384),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_TRANSPARENT, inert_star_transparent_000_displaylist_mesh_layer_5),
				GEO_BILLBOARD_WITH_PARAMS(LAYER_OPAQUE, 0, 0, 0),
				GEO_OPEN_NODE(),
					GEO_SCALE(LAYER_OPAQUE, 16384),
					GEO_OPEN_NODE(),
						GEO_ASM(0, geo_update_inert_star_indicator),
						GEO_DISPLAY_LIST(LAYER_TRANSPARENT, inert_star_transparent_001_no_dl_mesh_layer_5_with_revert),
					GEO_CLOSE_NODE(),
				GEO_CLOSE_NODE(),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
