#include "src/game/envfx_snow.h"

// TODO: Placeholder
const GeoLayout star_medallion_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
      GEO_SCALE(0x00, 0x28000),
		GEO_OPEN_NODE(),
			GEO_TRANSLATE_ROTATE(LAYER_OPAQUE_2ND_PASS, 0, -4, 0, 0, 0, 0),
			GEO_OPEN_NODE(),
				GEO_DISPLAY_LIST(LAYER_OPAQUE_2ND_PASS, yellow_coin_3D_Cylinder_mesh_layer_1),
				GEO_DISPLAY_LIST(LAYER_OPAQUE_2ND_PASS, yellow_coin_3D_material_revert_render_settings),
			GEO_CLOSE_NODE(),
		GEO_CLOSE_NODE(),
		GEO_TRANSLATE_ROTATE(LAYER_OPAQUE_2ND_PASS, 0, 100, 0, 0, 0, 0),
		GEO_OPEN_NODE(),
			GEO_JUMP_TO_EXISTING_GRAPH_NODE(MODEL_COLLECTIBLE_GLOW, FALSE),
		GEO_CLOSE_NODE(),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
