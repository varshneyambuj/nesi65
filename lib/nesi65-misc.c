/*
 * NESI65 - Miscellaneous Utilities
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-misc.h"

static int seeda = 0;
static int seedb = 0;

/* Initialize the PRNG with two seed values */
void init_prng(int sa, int sb)
{
	seeda = sa;
	seedb = sb;
}

/*
 * Linear congruential pseudo-random number generator.
 * Returns (sda * last + sdb) % m, or -1 if seeds are zero.
 */
int prng(int sda, int sdb, int m)
{
	int random_number;
	static int last_random_number = 0;

	if (sda == 0 || sdb == 0)
		return -1;

	random_number = (sda * last_random_number + sdb) % m;
	last_random_number = random_number;

	return random_number;
}

/*
 * Master PRNG that chains two calls to prng() for better distribution.
 * Returns a value in range [0, m).
 */
int master_prng(int m)
{
	int random_number;
	int res;

	random_number = prng(seeda, seedb, 25);
	res = prng(seeda + random_number, seedb + random_number, m);

	seeda = random_number + seeda;
	seedb = (random_number - 2);

	return res;
}

/* Busy-wait delay loop */
void delay(int ms)
{
	int i, j;
	for (i = 0; i < 255; i++)
		for (j = 0; j < ms; j++)
			;
}
