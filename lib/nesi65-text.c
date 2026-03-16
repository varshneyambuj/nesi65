/*
 * NESI65 - Text Rendering Utilities
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-text.h"

byte nesi_char_to_tile(char ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return (ch - 'A') + 0x41;
	if (ch >= 'a' && ch <= 'z')
		return (ch - 'a') + 0x41;
	if (ch >= '0' && ch <= '9')
		return 0x30 + (ch - '0');
	if (ch == '-')
		return 0x2D;
	if (ch == '.')
		return 0x2E;
	if (ch == '!')
		return 0x21;
	if (ch == ':')
		return 0x3A;
	/* space and anything else */
	return 0xFF;
}

void nesi_text_write(word position, const char *text)
{
	word addr;

	addr = nesi_name_table_0 + position;
	while (*text) {
		nesi_put_vram(addr, nesi_char_to_tile(*text));
		++text;
		++addr;
	}
}

void nesi_text_write_len(word position, const char *text, byte len)
{
	word addr;
	byte i;

	addr = nesi_name_table_0 + position;
	for (i = 0; i < len; i++) {
		nesi_put_vram(addr, nesi_char_to_tile(text[i]));
		++addr;
	}
}

void nesi_text_number(word position, word number, byte width)
{
	byte digits[5];
	byte count;
	byte i;
	word addr;
	word n;

	count = 0;
	n = number;

	/* Extract digits in reverse order */
	if (n == 0) {
		digits[0] = 0;
		count = 1;
	} else {
		while (n > 0 && count < 5) {
			digits[count] = (byte)(n % 10);
			n = n / 10;
			count++;
		}
	}

	/* Use minimum width if width is 0 */
	if (width == 0)
		width = count;

	addr = nesi_name_table_0 + position;

	/* Write leading spaces for right-alignment */
	for (i = 0; i < width - count; i++) {
		nesi_put_vram(addr, 0xFF);
		++addr;
	}

	/* Write digits (reverse order to get MSB first) */
	for (i = count; i > 0; i--) {
		nesi_put_vram(addr, 0x30 + digits[i - 1]);
		++addr;
	}
}

void nesi_text_clear_rect(word position, byte width, byte height)
{
	byte r, c;
	word addr;

	for (r = 0; r < height; r++) {
		addr = nesi_name_table_0 + position + (r * NESI_SCREEN_COLS);
		for (c = 0; c < width; c++) {
			nesi_put_vram(addr + c, 0xFF);
		}
	}
}

void nesi_text_clear_row(byte row)
{
	nesi_text_clear_rect(row * NESI_SCREEN_COLS, NESI_SCREEN_COLS, 1);
}
