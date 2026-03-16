/*
 * NESI65 - Math Utilities
 *
 * Common math macros and functions for NES game development.
 * Optimized for the 6502: avoids division and floating-point.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_MATH_H
#define NESI65_MATH_H

#include "nesi65.h"

/* Absolute value */
#define nesi_abs(x)  ((x) < 0 ? -(x) : (x))

/* Minimum of two values */
#define nesi_min(a, b) ((a) < (b) ? (a) : (b))

/* Maximum of two values */
#define nesi_max(a, b) ((a) > (b) ? (a) : (b))

/* Clamp value to range [lo, hi] */
#define nesi_clamp(val, lo, hi) \
	((val) < (lo) ? (lo) : ((val) > (hi) ? (hi) : (val)))

/* Sign of a value: -1, 0, or 1 */
#define nesi_sign(x) ((x) > 0 ? 1 : ((x) < 0 ? -1 : 0))

/*
 * Fast approximate distance using max(dx,dy) + min(dx,dy)/2.
 * Good enough for collision checks without sqrt.
 */
byte nesi_distance_approx(byte x1, byte y1, byte x2, byte y2);

/*
 * Linear interpolation: moves 'current' towards 'target' by 'step'.
 * Returns the new position.
 */
int nesi_approach(int current, int target, int step);

/*
 * Wrap a value around a range [0, max).
 * Useful for screen-edge wrapping.
 */
byte nesi_wrap(int val, byte max_val);

#endif /* NESI65_MATH_H */
