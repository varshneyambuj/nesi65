/*
 * NESI65 - Text Rendering Utilities
 *
 * Functions for rendering text strings and numbers to the NES
 * background nametable. Uses the standard ASCII-mapped tile layout
 * where tile 0x41='A', 0x30='0', etc.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_TEXT_H
#define NESI65_TEXT_H

#include "nesi65.h"
#include "nesi65-memory.h"

/* NES screen is 32 tiles wide, 30 tiles tall */
#define NESI_SCREEN_COLS 32
#define NESI_SCREEN_ROWS 30

/* Convert (row, col) to nametable offset */
#define nesi_rowcol(row, col) ((row) * NESI_SCREEN_COLS + (col))

/*
 * Write a null-terminated string to nametable at tile position.
 * Position is an offset from nametable 0 base (0-959).
 * Characters are mapped: A-Z -> 0x41-0x5A, 0-9 -> 0x30-0x39, space -> 0xFF.
 */
void nesi_text_write(word position, const char *text);

/*
 * Write a string of known length to nametable.
 * Does not require null termination.
 */
void nesi_text_write_len(word position, const char *text, byte len);

/*
 * Write an unsigned integer (0-9999) to nametable at position.
 * Right-aligned within 'width' characters (padded with spaces).
 * If width is 0, uses the minimum number of digits.
 */
void nesi_text_number(word position, word number, byte width);

/*
 * Clear a rectangular region of the nametable to blank tiles (0xFF).
 *   position: top-left nametable offset
 *   width:    number of columns to clear
 *   height:   number of rows to clear
 */
void nesi_text_clear_rect(word position, byte width, byte height);

/*
 * Clear an entire row of the nametable.
 */
void nesi_text_clear_row(byte row);

/*
 * Convert an ASCII character to its tile index.
 */
byte nesi_char_to_tile(char ch);

#endif /* NESI65_TEXT_H */
