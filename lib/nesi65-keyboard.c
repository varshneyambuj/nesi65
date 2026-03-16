/*
 * NESI65 - NES Keyboard Input (Family Basic / SUBOR)
 *
 * Reads keyboard input by scanning the keyboard matrix through
 * the controller ports ($4016/$4017). Supports both Family Basic
 * and SUBOR keyboard layouts.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-keyboard.h"

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

/* Initialize keyboard interface by sending the strobe sequence */
void nesi_initkeyb()
{
	byte b1 = 4;
	byte b2 = 5;
	byte b3 = 4;

	a_addr(0x4016) = b1;
	a_addr(0x4016) = b2;
	a_addr(0x4016) = b3;
}

/* Read a keypress and return the SUBOR keyboard character */
char nesi_readkeyb_subor()
{
	return keyboard_subor[nesi_readkeyb()];
}

/* Read a keypress and return the Family Basic keyboard character */
char nesi_readkeyb_family()
{
	return keyboard[nesi_readkeyb()];
}

/*
 * Scan the keyboard matrix and return a key index.
 * Reads 13 rows of the keyboard matrix from $4017, toggling
 * $4016 to advance rows. Returns the encoded key position
 * (row*8 + column) or '\0' if no key is pressed.
 */
byte nesi_readkeyb()
{
	int i;
	int found = 0;
	byte read_word[13];
	byte t;
	byte t1;
	t = 0;

	for (i = 0; i < 13; i++) {
		t = 0;
		t = a_addr(0x4017);
		t = t >> 1;
		t1 = (t & 0x0F);
		t = 6;

		a_addr(0x4016) = t;
		t = a_addr(0x4017);
		t = t << 3;

		t1 = t1 | (t & 0xF0);
		read_word[i] = t1;

		t = 4;
		a_addr(0x4016) = t;
	}

	for (i = 0; i < 13; i++) {
		t = 1;
		for (t1 = 0; t1 < 8; t1++) {
			if ((read_word[i] & t) == 0) {
				found = 1;
				goto exit;
			}
			t = t << 1;
		}
	}

exit:
	if (found == 1) {
		t = i;
		t = t << 3;
		t = t | t1;
		return t;
	} else {
		return '\0';
	}
}
