/*
 * NESI65 - VRAM and Memory Manipulation
 *
 * Functions and macros for writing to PPU VRAM, copying tile data,
 * sprite DMA, and general RAM fill operations. Low-level routines
 * are implemented in nesi65-memoryc.s (hand-optimized 6502 asm).
 *
 * Author:  Ambuj Varshney <ambuj_varshney(at)daiict.ac.in>
 * License: GNU GPL v3
 */

#ifndef NESI65_MEMORY_H
#define NESI65_MEMORY_H

#include "nesi65.h"

/* Shared variables with the NMI assembly handler for VBlank operations */
extern volatile unsigned char to_disappear_high;
extern volatile unsigned char to_disappear_low;
extern volatile unsigned char no_disappear;
extern volatile unsigned char vblank_flag;

#define highbyte(value) ((byte)(value >> 8))
#define lowbyte(value)  ((byte)value)

/*
 * Blank background tiles during VBlank.
 * Sets up shared variables and waits for the NMI handler to
 * fill tiles with 0xFF at the given VRAM location.
 */
#define nesi_blank_background(start_location, no_to_disappear) \
	to_disappear_high = highbyte(start_location); \
	to_disappear_low = lowbyte(start_location); \
	no_disappear = no_to_disappear; \
	vblank_flag = 1; \
	while ((vblank_flag != 0));

/*
 * Write an entire nametable array (960 tiles) to VRAM.
 *   nametable: array of 960 tile indices
 *   i: loop variable (must be declared by caller)
 */
#define nesi_put_background(nametable, i) \
	for (i = 0; i < 960; i++) { \
		nesi_put_vram((nesi_name_table_0 + i), nametable[i]); \
	}

/* Copy tile pattern data to PPU pattern table */
void nesi_copy_tiles(byte *tiles, word start_from, word start_to, word quantity);

/* Copy data from CPU RAM to PPU VRAM (implemented in nesi65-memoryc.s) */
void nesi_copy_vram(byte *source, word length, word target);

/* Fill a region of RAM with a byte value (implemented in nesi65-memoryc.s) */
void nesi_fill_ram(word address, word length, byte fill);

/* Trigger sprite DMA transfer to PPU OAM (implemented in nesi65-memoryc.s) */
void nesi_sprite_dma(byte page, byte start);

/* Write a single byte to PPU VRAM (implemented in nesi65-memoryc.s) */
void nesi_put_vram(int address, byte value);

#endif /* NESI65_MEMORY_H */
