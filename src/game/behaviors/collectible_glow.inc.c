// collectible_glow.inc.c

// Glow background properties
#define NUM_GLOW_BGS 3
#define NUM_GLOW_BGS_CONSOLE 3
#define BG_VERT_COUNT 4
#define BG_CYCLE_FRAMES 200
#define BG_CYCLE_WAIT_FRAMES 25
#define BG_CYCLE_TOTAL_FRAMES (BG_CYCLE_FRAMES + BG_CYCLE_WAIT_FRAMES)
#define BG_MIN_LENGTH 6
#define BG_MAX_LENGTH 225

#define MAX_NUM_BGS_PER_CMD ((u32) (32 / (BG_VERT_COUNT)))
#define MAX_NUM_BG_VERTS_PER_CMD ((MAX_NUM_BGS_PER_CMD) * (BG_VERT_COUNT))

// Ray properties
#define NUM_RAYS 10
#define RAY_VERT_COUNT 4
#define NUM_RAYS_CONSOLE 8
#define RAY_CYCLE_FRAMES 50
#define RAY_CYCLE_WAIT_FRAMES 0
#define RAY_CYCLE_TOTAL_FRAMES (RAY_CYCLE_FRAMES + RAY_CYCLE_WAIT_FRAMES)
#define RAY_MIN_LENGTH 105
#define RAY_MAX_LENGTH 330
#define RAY_MIN_CENTER_OFFSET 0
#define RAY_MAX_CENTER_OFFSET 60
#define RAY_MIN_ANGLE_WIDTH 0x40
#define RAY_MAX_ANGLE_WIDTH 0x500
#define RAY_VERT_EDGE_FAVOR 0.925f

#define MAX_NUM_RAYS_PER_CMD ((u32) (32 / (RAY_VERT_COUNT)))
#define MAX_NUM_RAY_VERTS_PER_CMD ((MAX_NUM_RAYS_PER_CMD) * (RAY_VERT_COUNT))

#define CALCULATE_RANDOM(min, max) ((min) + (random_float() * ((max) - (min))))
#define DIVIDE_AND_CEIL(numerator, divisor) ( ( (u32)(numerator) + ((u32)(divisor)-1) ) / ((u32)(divisor)) )

enum CollectibleGlowType {
    CG_TYPE_BACKGROUND_GLOW,
    CG_TYPE_RAY,

    CG_TYPE_COUNT,
};

enum CollectibleGlowColorType {
    CG_COLOR_MIN,
    CG_COLOR_MAX,

    CG_COLOR_COUNT,
};

enum CollectibleGlowVertType {
    CG_VERT_NORMAL,

    CG_VERT_COUNT,
};

struct CollectibleGlowColorProps {
    u8 rgbaStart[CG_VERT_COUNT][4];
    u8 rgbaEnd[CG_VERT_COUNT][4];
};

struct CollectibleGlowColorBaselines {
    f32 rgbaStart[CG_VERT_COUNT][CG_COLOR_COUNT][4];
    f32 rgbaEnd[CG_VERT_COUNT][CG_COLOR_COUNT][4];
};

struct CollectibleGlowBGProps {
    struct CollectibleGlowColorProps color;

    u16 currentFrame;
    u8 cleared;
};

struct CollectibleGlowRayProps {
    struct CollectibleGlowColorProps color;

    u16 currentFrame;
    u16 angle;
    u16 angleWidth;
    u16 length;
    u16 centerOffset;
    u8 cleared;
};

static const struct CollectibleGlowColorBaselines baselineColors[CG_TYPE_COUNT] = {
    [CG_TYPE_BACKGROUND_GLOW] = {
        .rgbaStart[CG_VERT_NORMAL][CG_COLOR_MIN] = { 0.45f, 0.45f, 0.45f, 0.92f },
        .rgbaStart[CG_VERT_NORMAL][CG_COLOR_MAX] = { 0.85f, 0.85f, 0.85f, 0.92f },
          .rgbaEnd[CG_VERT_NORMAL][CG_COLOR_MIN] = { 0.55f, 0.55f, 0.55f, 0.92f },
          .rgbaEnd[CG_VERT_NORMAL][CG_COLOR_MAX] = { 0.95f, 0.95f, 0.95f, 0.92f },
    },
    [CG_TYPE_RAY] = {
        .rgbaStart[CG_VERT_NORMAL][CG_COLOR_MIN] = { 0.45f, 0.45f, 0.90f, 0.90f },
        .rgbaStart[CG_VERT_NORMAL][CG_COLOR_MAX] = { 0.95f, 0.70f, 1.00f, 1.00f },
          .rgbaEnd[CG_VERT_NORMAL][CG_COLOR_MIN] = { 0.45f, 0.90f, 0.45f, 0.90f },
          .rgbaEnd[CG_VERT_NORMAL][CG_COLOR_MAX] = { 0.95f, 1.00f, 0.70f, 1.00f },
    }
};

// Glow Background
ALIGNED16 static Vtx collectibleGlowBGVerts[2][CG_VERT_COUNT][BG_VERT_COUNT * MAX(NUM_GLOW_BGS, NUM_GLOW_BGS_CONSOLE)];
ALIGNED16 static struct CollectibleGlowBGProps collectibleGlowBGProps[MAX(NUM_GLOW_BGS, NUM_GLOW_BGS_CONSOLE)];

// Rays
ALIGNED16 static Vtx collectibleGlowRayVerts[2][CG_VERT_COUNT][RAY_VERT_COUNT * MAX(NUM_RAYS, NUM_RAYS_CONSOLE)];
ALIGNED16 static struct CollectibleGlowRayProps collectibleGlowRayProps[MAX(NUM_RAYS, NUM_RAYS_CONSOLE)];

u32 collectibleGlowVertsIndex = 0;

static void collectible_glow_calculate_colors(enum CollectibleGlowType type, struct CollectibleGlowColorProps *color) {
    const struct CollectibleGlowColorBaselines *baseline;

    baseline = &baselineColors[type];
    for (s32 i = 0; i < CG_VERT_COUNT; i++) {
        for (s32 j = 0; j < 4; j++) {
            // rgbaStart
            color->rgbaStart[i][j] = (u8) (CALCULATE_RANDOM(baseline->rgbaStart[i][CG_COLOR_MIN][j], baseline->rgbaStart[i][CG_COLOR_MAX][j]) * 255.0f + 0.5f);

            // rgbaEnd
            color->rgbaEnd[i][j] = (u8) (CALCULATE_RANDOM(baseline->rgbaEnd[i][CG_COLOR_MIN][j], baseline->rgbaEnd[i][CG_COLOR_MAX][j]) * 255.0f + 0.5f);
        }
    }
}

static void new_glow_bg(s32 index) {
    struct CollectibleGlowBGProps *props = &collectibleGlowBGProps[index];
    collectible_glow_calculate_colors(CG_TYPE_BACKGROUND_GLOW, &props->color);

    props->currentFrame = 0;
    props->cleared = FALSE;
}

static void new_glow_ray(s32 index) {
    struct CollectibleGlowRayProps *props = &collectibleGlowRayProps[index];
    collectible_glow_calculate_colors(CG_TYPE_RAY, &props->color);

    props->currentFrame = 0;
    props->cleared = FALSE;
    props->angle        = random_u16();
    props->angleWidth   = (u32) (CALCULATE_RANDOM(RAY_MIN_ANGLE_WIDTH, RAY_MAX_ANGLE_WIDTH) + 0.5f);
    props->length       = (u32) (CALCULATE_RANDOM(RAY_MIN_LENGTH, RAY_MAX_LENGTH) + 0.5f);
    props->centerOffset = (u32) (CALCULATE_RANDOM(RAY_MIN_CENTER_OFFSET, RAY_MAX_CENTER_OFFSET) + 0.5f);
}

void collectible_glow_init(void) {
    s32 numRays = NUM_RAYS;
    s32 numGlowBGs = NUM_GLOW_BGS;
    if (gIsConsole) {
        numRays = NUM_RAYS_CONSOLE;
        numGlowBGs = NUM_GLOW_BGS_CONSOLE;
    }

    bzero(collectibleGlowRayVerts, sizeof(collectibleGlowRayVerts));
    bzero(collectibleGlowBGVerts, sizeof(collectibleGlowBGVerts));

    // Offset all new glow BGs and rays so they're spread out
    for (s32 i = 0; i < numGlowBGs; i++) {
        new_glow_bg(i);
        collectibleGlowBGProps[i].currentFrame = (BG_CYCLE_TOTAL_FRAMES + roundf(BG_CYCLE_TOTAL_FRAMES * (f32) (numGlowBGs - i) / (f32) numGlowBGs)) % BG_CYCLE_TOTAL_FRAMES;
    }
    for (s32 i = 0; i < numRays; i++) {
        new_glow_ray(i);
        collectibleGlowRayProps[i].currentFrame = (RAY_CYCLE_TOTAL_FRAMES + roundf(RAY_CYCLE_TOTAL_FRAMES * (f32) (numRays - i) / (f32) numRays)) % RAY_CYCLE_TOTAL_FRAMES;
    }

    // Update all features of BG texture coords that will never change (i.e. UVs)
    for (s32 i = 0; i < ARRAY_COUNT(collectibleGlowBGVerts); i++) {
        for (s32 j = 0; j < ARRAY_COUNT(collectibleGlowBGVerts[0]); j++) {
            for (s32 k = 0; k < ARRAY_COUNT(collectibleGlowBGVerts[0][0]); k++) {
                Vtx *vertex = &collectibleGlowBGVerts[i][j][k];

                if (k % 2 == 0) {
                    vertex->v.ob[0] = -128;
                    vertex->v.tc[0] = 0;
                } else {
                    vertex->v.ob[0] = 128;
                    vertex->v.tc[0] = 2018;
                }

                if ((k % 4) < 2) {
                    vertex->v.ob[1] = 128;
                    vertex->v.tc[1] = 2016;
                } else {
                    vertex->v.ob[1] = -128;
                    vertex->v.tc[1] = 0;
                }
            }
        }
    }

    // Update all features of ray texture coords that will never change (i.e. UVs)
    for (s32 i = 0; i < ARRAY_COUNT(collectibleGlowRayVerts); i++) {
        for (s32 j = 0; j < ARRAY_COUNT(collectibleGlowRayVerts[0]); j++) {
            for (s32 k = 0; k < ARRAY_COUNT(collectibleGlowRayVerts[0][0]); k++) {
                Vtx *vertex = &collectibleGlowRayVerts[i][j][k];

                switch (k % RAY_VERT_COUNT) {
                    case 0:
                    default:
                        vertex->v.ob[0] = 384;
                        vertex->v.ob[1] = 12;
                        vertex->v.tc[0] = 4080;
                        vertex->v.tc[1] = 16;
                        break;
                    case 1:
                        vertex->v.ob[0] = 384;
                        vertex->v.ob[1] = -12;
                        vertex->v.tc[0] = 4080;
                        vertex->v.tc[1] = 16;
                        break;
                    case 2:
                        vertex->v.tc[0] = -16;
                        vertex->v.tc[1] = -16;
                        break;
#if RAY_VERT_COUNT == 4
                    case 3:
                        vertex->v.ob[0] = 320;
                        vertex->v.ob[1] = 0;
                        vertex->v.tc[0] = 4080;
                        vertex->v.tc[1] = 16;
                        break;
#endif
                }
            }
        }
    }
}

static void collectible_glow_update_color(struct CollectibleGlowColorProps *color, f32 linearMultiplier, u8 rgbaOut[CG_VERT_COUNT][4]) {
    f32 inverseLinearMultiplier = (1.0f - linearMultiplier);
    f32 sinMultiplier = linearMultiplier * sins(0x8000 * linearMultiplier);
    f32 inverseSinMultiplier = inverseLinearMultiplier * sins(0x8000 * linearMultiplier);

    // Update colors
    for (s32 i = 0; i < CG_VERT_COUNT; i++) {
        rgbaOut[i][0] = (u8) ( ( color->rgbaStart[i][0] * inverseLinearMultiplier ) + ( color->rgbaEnd[i][0] * linearMultiplier ) + 0.5f );
        rgbaOut[i][1] = (u8) ( ( color->rgbaStart[i][1] * inverseLinearMultiplier ) + ( color->rgbaEnd[i][1] * linearMultiplier ) + 0.5f );
        rgbaOut[i][2] = (u8) ( ( color->rgbaStart[i][2] * inverseLinearMultiplier ) + ( color->rgbaEnd[i][2] * linearMultiplier ) + 0.5f );
        rgbaOut[i][3] = (u8) ( ( color->rgbaStart[i][3] *    inverseSinMultiplier ) + ( color->rgbaEnd[i][3] *    sinMultiplier ) + 0.5f );
    }
}

static void collectible_glow_update_bg(s32 index) {
    s32 vertIndex = index * BG_VERT_COUNT;
    f32 linearMultiplier;

    u8 rgba[CG_VERT_COUNT][4];
    Vec2f vertOffsets[BG_VERT_COUNT];
    f32 newLength;

    struct CollectibleGlowBGProps *props = &collectibleGlowBGProps[index];

    props->currentFrame++;
    if (props->currentFrame >= BG_CYCLE_TOTAL_FRAMES) {
        new_glow_bg(index);
    } else if (props->currentFrame > BG_CYCLE_FRAMES) { // Intentionally not >=, verts should be updated to 0 transparency
        collectible_glow_update_color(&props->color, 1.0f, rgba);
        return;
    }

    linearMultiplier = ((f32) props->currentFrame / (f32) BG_CYCLE_FRAMES);
    collectible_glow_update_color(&props->color, linearMultiplier, rgba);

    // Length
    newLength = BG_MIN_LENGTH + (linearMultiplier * (BG_MAX_LENGTH - BG_MIN_LENGTH));

    // Compute new vert coordinates
    for (s32 i = 0; i < BG_VERT_COUNT; i++) {
        f32 *vertOffset = vertOffsets[i];
        if (i % 2 == 0) {
            vertOffset[0] = -newLength;
        } else {
            vertOffset[0] = newLength;
        }

        if ((i % 4) < 2) {
            vertOffset[1] = -newLength;
        } else {
            vertOffset[1] = newLength;
        }
    }
    
    // Update verts
    for (s32 j = 0; j < CG_VERT_COUNT; j++) {
        for (s32 k = 0; k < BG_VERT_COUNT; k++) {
            Vtx_t *vertex = &collectibleGlowBGVerts[collectibleGlowVertsIndex][j][vertIndex + k].v;

            vertex->ob[0] = vertOffsets[k][0];
            vertex->ob[1] = vertOffsets[k][1];

            vertex->cn[0] = rgba[j][0];
            vertex->cn[1] = rgba[j][1];
            vertex->cn[2] = rgba[j][2];
            vertex->cn[3] = rgba[j][3];
        }
    }
}

static void collectible_glow_update_ray(s32 index) {
    s32 vertIndex = index * RAY_VERT_COUNT;
    f32 linearMultiplier;

    u8 rgba[CG_VERT_COUNT][4];
    Vec2f vertOffsets[RAY_VERT_COUNT];
    f32 newLength;
    f32 newCenterOffset;

    struct CollectibleGlowRayProps *props = &collectibleGlowRayProps[index];

    props->currentFrame++;
    if (props->currentFrame >= RAY_CYCLE_TOTAL_FRAMES) {
        new_glow_ray(index);
    } else if (props->currentFrame > RAY_CYCLE_FRAMES) { // Intentionally not >=, verts should be updated to 0 transparency
        collectible_glow_update_color(&props->color, 1.0f, rgba);
        return;
    }

    linearMultiplier = ((f32) props->currentFrame / (f32) RAY_CYCLE_FRAMES);
    collectible_glow_update_color(&props->color, linearMultiplier, rgba);

    // Length and offset
    newCenterOffset = props->centerOffset * (1.0f + linearMultiplier);
    newLength = newCenterOffset + (props->length * (1.0f + linearMultiplier));

    // Compute new vert coordinates
    vertOffsets[0][0] = newLength * sins(props->angle + props->angleWidth);
    vertOffsets[0][1] = newLength * coss(props->angle + props->angleWidth);
    vertOffsets[1][0] = newLength * sins(props->angle - props->angleWidth);
    vertOffsets[1][1] = newLength * coss(props->angle - props->angleWidth);
    vertOffsets[2][0] = newCenterOffset * sins(props->angle);
    vertOffsets[2][1] = newCenterOffset * coss(props->angle);
#if RAY_VERT_COUNT == 4
    vertOffsets[3][0] = (newLength * RAY_VERT_EDGE_FAVOR) * sins(props->angle);
    vertOffsets[3][1] = (newLength * RAY_VERT_EDGE_FAVOR) * coss(props->angle);
#endif
    
    // Update verts
    for (s32 j = 0; j < CG_VERT_COUNT; j++) {
        for (s32 k = 0; k < RAY_VERT_COUNT; k++) {
            Vtx_t *vertex = &collectibleGlowRayVerts[collectibleGlowVertsIndex][j][vertIndex + k].v;

            vertex->ob[0] = vertOffsets[k][0];
            vertex->ob[1] = vertOffsets[k][1];

            vertex->cn[0] = rgba[j][0];
            vertex->cn[1] = rgba[j][1];
            vertex->cn[2] = rgba[j][2];
            vertex->cn[3] = rgba[j][3];
        }
    }
}

void collectible_glow_update(void) {
    s32 numRays = NUM_RAYS;
    s32 numGlowBGs = NUM_GLOW_BGS;
    if (gIsConsole) {
        numRays = NUM_RAYS_CONSOLE;
        numGlowBGs = NUM_GLOW_BGS_CONSOLE;
    }

    collectibleGlowVertsIndex = (collectibleGlowVertsIndex + 1) % ARRAY_COUNT(collectibleGlowBGVerts);

    for (s32 i = 0; i < numGlowBGs; i++) {
        collectible_glow_update_bg(i);
    }

    // Sort bg verts for rendering (place newer cycles at the end)
    for (s32 i = 0; i < ARRAY_COUNT(collectibleGlowBGProps) - 1; i++) {
        for (s32 j = i + 1; j < ARRAY_COUNT(collectibleGlowBGProps); j++) {
            if (collectibleGlowBGProps[i].currentFrame < collectibleGlowBGProps[j].currentFrame) {
                struct CollectibleGlowBGProps tmpProps = collectibleGlowBGProps[i];
                collectibleGlowBGProps[i] = collectibleGlowBGProps[j];
                collectibleGlowBGProps[j] = tmpProps;

                // monkaS
                for (s32 k = 0; k < CG_VERT_COUNT; k++) {
                    for (s32 l = i * BG_VERT_COUNT, m = j * BG_VERT_COUNT; l < (i + 1) * BG_VERT_COUNT; l++, m++) {
                        Vtx tmpVert = collectibleGlowBGVerts[collectibleGlowVertsIndex][k][l];
                        collectibleGlowBGVerts[collectibleGlowVertsIndex][k][l] = collectibleGlowBGVerts[collectibleGlowVertsIndex][k][m];
                        collectibleGlowBGVerts[collectibleGlowVertsIndex][k][m] = tmpVert;
                    }
                }
            }
        }
    }

    for (s32 i = 0; i < numRays; i++) {
        collectible_glow_update_ray(i);
    }
}

void collectible_glow_clear(void) {
    for (s32 i = 0; i < ARRAY_COUNT(collectibleGlowBGProps); i++) {
        collectibleGlowBGProps[i].cleared = TRUE;
    }

    for (s32 i = 0; i < ARRAY_COUNT(collectibleGlowRayProps); i++) {
        collectibleGlowRayProps[i].cleared = TRUE;
    }
}

Gfx *geo_obj_render_collectible_glow_bgs(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx) {
    Gfx *gfxHead = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        Gfx *gfx;
        struct GraphNodeGenerated *currentGraphNode = (struct GraphNodeGenerated *) node;
        s32 layer = currentGraphNode->parameter;
        s32 numGlowBGs;
        s32 numDLs;
        Vtx *verts;
        
        numGlowBGs = NUM_GLOW_BGS;
        if (gIsConsole) {
            numGlowBGs = NUM_GLOW_BGS_CONSOLE;
        }

        verts = collectibleGlowBGVerts[collectibleGlowVertsIndex][CG_VERT_NORMAL];

        numDLs = 1; // gSPEndDisplayList
        numDLs += DIVIDE_AND_CEIL(numGlowBGs, MAX_NUM_BGS_PER_CMD); // gSPVertex
        numDLs += numGlowBGs; // gSP2Triangles

        gfxHead = alloc_display_list(numDLs * sizeof(Gfx));
        gfx = gfxHead;

        SET_GRAPH_NODE_LAYER(currentGraphNode->fnNode.node.flags, layer);

        for (s32 i = 0; i < numGlowBGs; i++) {
            s32 vertOffset = BG_VERT_COUNT * (i % MAX_NUM_BGS_PER_CMD);
            if (vertOffset == 0) {
                gSPVertex(gfx++, VIRTUAL_TO_PHYSICAL(verts + (BG_VERT_COUNT * i)), MIN(MAX_NUM_BG_VERTS_PER_CMD, BG_VERT_COUNT * (u32) (numGlowBGs - i)), 0);
            }

            // Do not render if cycle hasn't been initiated from the start
            if (collectibleGlowBGProps[i].cleared || collectibleGlowBGProps[i].currentFrame >= BG_CYCLE_FRAMES) {
                continue;
            }

	        gSP2Triangles(gfx++, vertOffset+0, vertOffset+1, vertOffset+3, 0, vertOffset+0, vertOffset+3, vertOffset+2, 0);
        }

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}

Gfx *geo_obj_render_collectible_glow_rays(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx) {
    Gfx *gfxHead = NULL;

    if (callContext == GEO_CONTEXT_RENDER) {
        Gfx *gfx;
        struct GraphNodeGenerated *currentGraphNode = (struct GraphNodeGenerated *) node;
        s32 layer = currentGraphNode->parameter;
        s32 numRays;
        s32 numDLs;
        Vtx *verts;

        numRays = NUM_RAYS;
        if (gIsConsole) {
            numRays = NUM_RAYS_CONSOLE;
        }

         verts = collectibleGlowRayVerts[collectibleGlowVertsIndex][CG_VERT_NORMAL];

        numDLs = 1; // gSPEndDisplayList
        numDLs += DIVIDE_AND_CEIL(numRays, MAX_NUM_RAYS_PER_CMD); // gSPVertex
#if RAY_VERT_COUNT == 4
        numDLs += numRays; // gSP2Triangles
#elif RAY_VERT_COUNT == 3
        numDLs += numRays; // gSP1Triangle (cannot easily optimize for gSP2Triangles due to ray skipping logic)
#else
        STATIC_ASSERT(FALSE, "RAY_VERT_COUNT unimplemented!")
#endif

        gfxHead = alloc_display_list(numDLs * sizeof(Gfx));
        gfx = gfxHead;

        SET_GRAPH_NODE_LAYER(currentGraphNode->fnNode.node.flags, layer);

        for (s32 i = 0; i < numRays; i++) {
            s32 vertOffset = RAY_VERT_COUNT * (i % MAX_NUM_RAYS_PER_CMD);
            if (vertOffset == 0) {
                gSPVertex(gfx++, VIRTUAL_TO_PHYSICAL(verts + (RAY_VERT_COUNT * i)), MIN(MAX_NUM_RAY_VERTS_PER_CMD, RAY_VERT_COUNT * (u32) (numRays - i)), 0);
            }

            // Do not render if cycle hasn't been initiated from the start
            if (collectibleGlowRayProps[i].cleared || collectibleGlowRayProps[i].currentFrame >= RAY_CYCLE_FRAMES) {
                continue;
            }

#if RAY_VERT_COUNT == 4
	        gSP2Triangles(gfx++, vertOffset+0, vertOffset+3, vertOffset+2, 0, vertOffset+3, vertOffset+1, vertOffset+2, 0);
#elif RAY_VERT_COUNT == 3
	        gSP1Triangle(gfx++, vertOffset+0, vertOffset+1, vertOffset+2, 0);
#else
            STATIC_ASSERT(FALSE, "RAY_VERT_COUNT unimplemented!")
#endif
        }

        gSPEndDisplayList(gfx);
    }

    return gfxHead;
}
