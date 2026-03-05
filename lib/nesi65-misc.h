/*
 * NESI65 - Miscellaneous Utilities
 *
 * Pseudo-random number generator (linear congruential) and delay function.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_MISC_H
#define NESI65_MISC_H

#include "nesi65.h"

void init_prng(int sa, int sb);
int prng(int sda, int sdb, int m);
int master_prng(int m);
void delay(int ms);

#endif /* NESI65_MISC_H */
