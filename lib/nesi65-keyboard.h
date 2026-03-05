/*
 * NESI65 - NES Keyboard Input (Family Basic / SUBOR)
 *
 * Support for the Family Basic keyboard and SUBOR-compatible keyboards
 * connected to the NES expansion port ($4016/$4017).
 *
 * Author:  Ambuj Varshney <ambuj_varshney(at)daiict.ac.in>
 * License: GNU GPL v3
 */

#ifndef NESI65_KEYBOARD_H
#define NESI65_KEYBOARD_H

#include "nesi65.h"

/* Family Basic keyboard layout map (128 bytes) */
byte keyboard[] = {
	0x08, 0xA7, 0x41, 0x41, 0x41, 0xBB, 0x41, 0xAE,
	0x07, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41, 0x41,
	0x06, 0x4F, 'L',  'K',  '>',  '<',  'P',  ')',
	0x05, 'I',  'U',  'J',  'M',  'N',  '(',  '*',
	0x04, 'Y',  'G',  'H',  'B',  'V',  '&',  '^',
	0x03, 'T',  'R',  'D',  'F',  'C',  '%',  '$',
	0x02, 'W',  'S',  'A',  'X',  'Z',  'E',  '#',
	0x01, 0x0C, 'Q',  0xA9, 0xAB, 0x0B, '!',  '@',
	0x09, 0x15, 0x18, 0x17, 0x16, 0x41, 0xD0, 0x86,
	0,    0x15, 0x17, 0x18, 0x16, 0,    0,    0,
	0,    0,    0,    0,    0,    0,    0,    0
};

/* SUBOR-compatible keyboard layout map */
char keyboard_subor[] = {
	'4', 'G', 'F', 'C', '2', 'E', '5', 'V',
	'2', 'D', 'S', 'E', '1', 'W', '3', 'X',
	'I', 'B', 'N', 'R', '8', 'P', 'D', 'H',
	'9', 'I', 'L', ',', '5', 'O', '0', '.',
	'[', 'R', 'U', 'L', '7', '0', '0', 'D',
	'Q', 'C', 'Z', 'T', 'E', 'A', '1', 'C',
	'7', 'Y', 'K', 'M', '4', 'U', '8', 'J',
	'0', '0', '0', '0', '6', 'P', '0', 'S',
	'T', 'H', 'N', ' ', '3', 'R', '6', 'B',
	0,   0,   0,   0,   0,   0,   0,   0,
	'M', '4', '7', '1', '2', '1', '2', '8',
	'-', '+', '*', '9', '0', '5', '/', 'N',
	'`', '6', 'P', ' ', '9', '3', '.', '0'
};

void nesi_initkeyb(void);
byte nesi_readkeyb(void);
char nesi_readkeyb_family(void);
char nesi_readkeyb_subor(void);

#endif /* NESI65_KEYBOARD_H */
