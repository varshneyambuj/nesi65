/*
 * NESI65 - NES Keyboard Input (Family Basic / SUBOR)
 *
 * Reads keyboard input by scanning the keyboard matrix through
 * the controller ports ($4016/$4017). Supports both Family Basic
 * and SUBOR keyboard layouts.
 *
 * Author:  Ambuj Varshney <ambuj_varshney(at)daiict.ac.in>
 * License: GNU GPL v3
 */

#include "nesi65-keyboard.h"

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
