/*
 * NESI65 - PPU Initialization and Graphics Control
 *
 * Functions to initialize the PPU, wait for VBlank, and
 * enable/disable sprite and background rendering.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_INIT_H
#define NESI65_INIT_H

#include "nesi65.h"

/* Check if VBlank is occurring (bit 7 of PPU status register) */
#define nesi_vblank(PPU_status) (PPU_status & bin(1,0,0,0,0,0,0,0))

/* Read PPU status register ($2002) */
#define nesi_return_PPU_status() a_addr(0x2002)

void nesi_waitvblank(void);
void __fastcall__ waitvblank(void);
void nesi_init(void);
void nesi_disable_graphics_state(int state);
void nesi_enable_graphics_state(int state);
void nesi_enable_graphics(void);
void nesi_disable_graphics(void);
byte nesi_ppu_parameter_address(struct ppu_parameters *parm);

#endif /* NESI65_INIT_H */
