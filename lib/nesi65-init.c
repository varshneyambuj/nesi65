/*
 * NESI65 - PPU Initialization and Graphics Control
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-init.h"

extern volatile unsigned char vblank_flag;

/*
 * Wait for the PPU vertical blank period.
 * Called during initialization and when synchronization with
 * the PPU rendering cycle is needed.
 */
void nesi_waitvblank()
{
}

/*
 * Initialize the PPU by waiting for two VBlank periods.
 * The NES PPU requires this stabilization after power-on/reset.
 */
void nesi_init()
{
	nesi_waitvblank();
	nesi_waitvblank();
}

/*
 * Disable sprite or background rendering via PPU mask register ($2001).
 *   state=0: disable sprites (clear bit 4)
 *   state=1: disable background (clear bit 3)
 *   state=2: disable both sprites and background
 */
void nesi_disable_graphics_state(int state)
{
	byte temp;

	if (state == 0) {
		temp = a_addr(0x2001);
		temp = temp & bin(1,1,1,0,1,1,1,1);
		a_addr(0x2001) = temp;
	} else if (state == 1) {
		temp = a_addr(0x2001);
		temp = temp & bin(1,1,1,1,0,1,1,1);
		a_addr(0x2001) = temp;
	} else if (state == 2) {
		temp = a_addr(0x2001);
		temp = bin(1,1,1,0,0,1,1,1);
		a_addr(0x2001) = temp;
	}
}

/*
 * Enable sprite or background rendering via PPU mask register ($2001).
 *   state=0: enable sprites (set bit 4)
 *   state=1: enable background (set bit 3)
 *   state=2: enable both sprites and background
 */
void nesi_enable_graphics_state(int state)
{
	byte temp;

	if (state == 0) {
		temp = a_addr(0x2001);
		temp = temp | bin(0,0,0,1,0,0,0,0);
		a_addr(0x2001) = temp;
	} else if (state == 1) {
		temp = a_addr(0x2001);
		temp = temp | bin(0,0,0,0,1,0,0,0);
		a_addr(0x2001) = temp;
	} else if (state == 2) {
		temp = a_addr(0x2001);
		temp = bin(0,0,0,1,1,0,0,0);
		a_addr(0x2001) = temp;
	}
}

/*
 * Configure PPU control register ($2000).
 * Sets nametable address, sprite/background pattern tables,
 * sprite size, and NMI enable.
 */
byte nesi_ppu_parameter_address(struct ppu_parameters *parm)
{
	int ppu_cntrl_state_1;
	ppu_cntrl_state_1 = parm->name_table_address;
	ppu_cntrl_state_1 = ppu_cntrl_state_1 | ((parm->sprite_address) << 3);
	ppu_cntrl_state_1 = ppu_cntrl_state_1 | ((parm->background_address) << 4);
	ppu_cntrl_state_1 = ppu_cntrl_state_1 | ((parm->sprite_size) << 5);
	ppu_cntrl_state_1 = ppu_cntrl_state_1 | ((parm->turn_nmi) << 7);

	a_addr(0x2000) = ppu_cntrl_state_1;

	return ppu_cntrl_state_1;
}

/* Enable all rendering (sprites + background + left-column clipping) */
void nesi_enable_graphics()
{
	a_addr(0x2001) = bin(0,0,0,1,1,1,1,0);
}

/* Disable all rendering */
void nesi_disable_graphics()
{
	a_addr(0x2001) = 0;
}
