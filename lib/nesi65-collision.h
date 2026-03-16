/*
 * NESI65 - Collision Detection
 *
 * Bounding box and sprite collision utilities for NES games.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_COLLISION_H
#define NESI65_COLLISION_H

#include "nesi65.h"

/* Axis-aligned bounding box */
typedef struct {
	byte x;      /* Left edge */
	byte y;      /* Top edge */
	byte width;  /* Width in pixels */
	byte height; /* Height in pixels */
} nesi_bbox;

/*
 * Test overlap between two bounding boxes.
 * Returns non-zero if boxes overlap, 0 otherwise.
 */
byte nesi_bbox_overlap(nesi_bbox *a, nesi_bbox *b);

/*
 * Test if a point (px, py) is inside a bounding box.
 * Returns non-zero if inside, 0 otherwise.
 */
byte nesi_point_in_bbox(byte px, byte py, nesi_bbox *box);

/*
 * Test collision between two 4x4-tile sprites (32x32 pixels).
 * Takes the top-left (x, y) of each sprite.
 * Returns non-zero if they overlap.
 */
byte nesi_sprite_collide_32(byte x1, byte y1, byte x2, byte y2);

/*
 * Test collision between two sprites of arbitrary size.
 * (x1, y1, w1, h1) and (x2, y2, w2, h2) define the sprite rectangles.
 * Returns non-zero if they overlap.
 */
byte nesi_sprite_collide(byte x1, byte y1, byte w1, byte h1,
                         byte x2, byte y2, byte w2, byte h2);

/*
 * Test if a sprite (x, y, w, h) is out of the visible screen area.
 * Returns non-zero if any part is off-screen.
 *   0 = fully on screen
 *   1 = off left/top
 *   2 = off right/bottom
 */
byte nesi_offscreen(byte x, byte y, byte w, byte h);

#endif /* NESI65_COLLISION_H */
