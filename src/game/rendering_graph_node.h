#ifndef RENDERING_GRAPH_NODE_H
#define RENDERING_GRAPH_NODE_H

#include <PR/ultratypes.h>

#include "engine/graph_node.h"

extern struct GraphNodeRoot *gCurGraphNodeRoot;
extern struct GraphNodeMasterList *gCurGraphNodeMasterList;
extern struct GraphNodePerspective *gCurGraphNodeCamFrustum;
extern struct GraphNodeCamera *gCurGraphNodeCamera;
extern struct GraphNodeObject *gCurGraphNodeObject;
extern struct GraphNodeHeldObject *gCurGraphNodeHeldObject;
extern u16 gAreaUpdateCounter;
extern u8 isGameFlipped;
// after processing an object, the type is reset to this
#define ANIM_TYPE_NONE                  0

// Not all parts have full animation: to save space, some animations only
// have xz, y, or no translation at all. All animations have rotations though
#define ANIM_TYPE_TRANSLATION           1
#define ANIM_TYPE_VERTICAL_TRANSLATION  2
#define ANIM_TYPE_LATERAL_TRANSLATION   3
#define ANIM_TYPE_NO_TRANSLATION        4

// Every animation includes rotation, after processing any of the above
// translation types the type is set to this
#define ANIM_TYPE_ROTATION              5

void geo_process_node_and_siblings(struct GraphNode *firstNode);
void geo_process_root(struct GraphNodeRoot *node, Vp *b, Vp *c, s32 clearColor);


Gfx *geo_invert_cond(s32 callContext, UNUSED struct GraphNode *node, UNUSED f32 mtx[4][4]);
Gfx *geo_invert_cond_off(s32 callContext, UNUSED struct GraphNode *node, UNUSED f32 mtx[4][4]);
Gfx *geo_invert(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx);
Gfx *geo_invert_off(s32 callContext, UNUSED struct GraphNode *node, UNUSED Mat4 mtx);

#endif // RENDERING_GRAPH_NODE_H
