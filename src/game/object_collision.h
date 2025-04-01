#ifndef OBJECT_COLLISION_H
#define OBJECT_COLLISION_H

s32 detect_object_hitbox_overlap(struct Object *a, struct Object *b);
void detect_object_collisions(void);

#endif // OBJECT_COLLISION_H
