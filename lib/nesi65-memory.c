/*
 * NESI65 - VRAM and Memory Manipulation (C routines)
 *
 * Most memory functions are implemented in nesi65-memoryc.s.
 * This file contains the C implementation of nesi_copy_tiles().
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-memory.h"

/*
 * Copy tile data from CPU memory into PPU pattern table via $2006/$2007.
 *   tiles:      pointer to tile data in CPU memory
 *   start_from: first tile index to copy (in tile units)
 *   start_to:   destination tile index in pattern table (in tile units)
 *   quantity:    number of tiles to copy
 */
void nesi_copy_tiles(byte *tiles, word start_from, word start_to, word quantity)
{
	byte *top;
	int addr;

	/* Each tile is 16 bytes, convert quantity to bytes */
	addr = a_addr(0x2002);
	quantity <<= 4;

	/* Offset source pointer to start_from tile */
	tiles += start_from << 4;

	top = tiles;
	top = top + quantity;

	/* Set PPU VRAM write address */
	a_addr(0x2006) = (byte)(start_to >> 4);
	a_addr(0x2006) = (byte)(start_to << 4);

	/* Write tile data byte-by-byte to PPU */
	while (tiles != top) {
		a_addr(0x2007) = *tiles;
		++tiles;
	}
}
