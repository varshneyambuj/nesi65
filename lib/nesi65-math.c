/*
 * NESI65 - Math Utilities
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-math.h"

byte nesi_distance_approx(byte x1, byte y1, byte x2, byte y2)
{
	byte dx, dy;
	byte mn, mx;

	dx = (x1 > x2) ? (x1 - x2) : (x2 - x1);
	dy = (y1 > y2) ? (y1 - y2) : (y2 - y1);

	if (dx > dy) {
		mx = dx;
		mn = dy;
	} else {
		mx = dy;
		mn = dx;
	}

	/* Approximation: max + min/2 (within ~8% of true distance) */
	return mx + (mn >> 1);
}

int nesi_approach(int current, int target, int step)
{
	if (current < target) {
		current += step;
		if (current > target)
			current = target;
	} else if (current > target) {
		current -= step;
		if (current < target)
			current = target;
	}
	return current;
}

byte nesi_wrap(int val, byte max_val)
{
	while (val < 0)
		val += max_val;
	while (val >= max_val)
		val -= max_val;
	return (byte)val;
}
