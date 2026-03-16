/*
 * NESI65 - Collision Detection
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-collision.h"

byte nesi_bbox_overlap(nesi_bbox *a, nesi_bbox *b)
{
	if (a->x + a->width <= b->x)  return 0;
	if (b->x + b->width <= a->x)  return 0;
	if (a->y + a->height <= b->y) return 0;
	if (b->y + b->height <= a->y) return 0;
	return 1;
}

byte nesi_point_in_bbox(byte px, byte py, nesi_bbox *box)
{
	if (px < box->x) return 0;
	if (py < box->y) return 0;
	if (px >= box->x + box->width) return 0;
	if (py >= box->y + box->height) return 0;
	return 1;
}

byte nesi_sprite_collide_32(byte x1, byte y1, byte x2, byte y2)
{
	/* 32x32 sprite collision: simple AABB check */
	if (x1 + 32 <= x2) return 0;
	if (x2 + 32 <= x1) return 0;
	if (y1 + 32 <= y2) return 0;
	if (y2 + 32 <= y1) return 0;
	return 1;
}

byte nesi_sprite_collide(byte x1, byte y1, byte w1, byte h1,
                         byte x2, byte y2, byte w2, byte h2)
{
	if (x1 + w1 <= x2) return 0;
	if (x2 + w2 <= x1) return 0;
	if (y1 + h1 <= y2) return 0;
	if (y2 + h2 <= y1) return 0;
	return 1;
}

byte nesi_offscreen(byte x, byte y, byte w, byte h)
{
	if (x == 0 && w == 0) return 1;
	if (y == 0 && h == 0) return 1;
	if (x + w > 248) return 2;
	if (y + h > 232) return 2;
	return 0;
}
