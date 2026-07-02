#include "src/game/envfx_snow.h"

const GeoLayout star_medallion_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_SHADOW(1, 153, 150),
		GEO_OPEN_NODE(),
			GEO_RENDER_RANGE(/*Min*/ 3500, /*Max*/ 32767),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE_2ND_PASS, star_medallion_dl_lowpoly_mesh_layer_1),
			GEO_CLOSE_NODE(),
			GEO_RENDER_RANGE(/*Min*/ -2048, /*Max*/ 3500),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE_2ND_PASS, star_medallion_dl_normal_mesh_layer_1_with_revert),
				GEO_DISPLAY_LIST(LAYER_TRANSPARENT_DECAL_2ND_PASS, star_medallion_dl_normal_mesh_layer_6_with_revert),
			GEO_CLOSE_NODE(),
			GEO_TRANSLATE_NODE(LAYER_OPAQUE_2ND_PASS, 0, 100, 0),
			GEO_OPEN_NODE(),
				GEO_JUMP_TO_EXISTING_GRAPH_NODE(/*Model ID*/ MODEL_COLLECTIBLE_GLOW, /*Display Shadow*/ FALSE),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
