/*
 * NESI65 - NES Game Development Library for cc65
 *
 * Core types, macros, and constants for NES PPU programming.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_H
#define NESI65_H

/* Basic types */
typedef unsigned char  byte;
typedef unsigned int   word;

#define TRUE  1
#define FALSE 0

/*
 * Binary literal macros - convert individual bits to a byte value.
 * Usage: bin(1,0,0,0,0,0,0,1) => 0x81
 * Bits are ordered MSB to LSB (h=bit7, a=bit0).
 */
#define bin(h,g,f,e,d,c,b,a)  (a|b<<1|c<<2|d<<3|e<<4|f<<5|g<<6|h<<7)
#define bin8(h,g,f,e,d,c,b,a) (a|b<<1|c<<2|d<<3|e<<4|f<<5|g<<6|h<<7)
#define bin7(g,f,e,d,c,b,a)   (a|b<<1|c<<2|d<<3|e<<4|f<<5|g<<6)
#define bin6(f,e,d,c,b,a)     (a|b<<1|c<<2|d<<3|e<<4|f<<5)
#define bin5(e,d,c,b,a)       (a|b<<1|c<<2|d<<3|e<<4)
#define bin4(d,c,b,a)         (a|b<<1|c<<2|d<<3)
#define bin3(c,b,a)           (a|b<<1|c<<2)
#define bin2(b,a)             (a|b<<1)

/* PPU nametable base addresses */
#define nesi_name_table_0   0x2000
#define nesi_attr_table_0   0x23C0
#define nesi_name_table_1   0x2400
#define nesi_attr_table_1   0x27C0
#define nesi_name_table_2   0x2800
#define nesi_attr_table_2   0x2BC0
#define nesi_name_table_3   0x2C00
#define nesi_attr_table_3   0x2FC0

/* PPU palette addresses */
#define nesi_back_palette   0x3F00
#define nesi_sprite_palette 0x3F10

/* Table lengths */
#define nesi_name_table_len 0x03C0
#define nesi_attr_table_len 0x0040
#define nesi_palette_len    0x0010

/* Sprite pattern table offset for 8x8 sprites */
#define nesi_sprite_tiles   0x0100

/* Direct memory access macro */
#define a_addr(_addr) (*(unsigned char*) (_addr))

/* Sprite OAM entry (4 bytes per sprite) */
typedef struct {
	byte y;    /* Y coordinate (0xFF = top of screen, display is y+1) */
	byte tile; /* Tile index from pattern table */
	byte attr; /* Sprite attributes (palette, flip, priority) */
	byte x;    /* X coordinate */
} nesi_sprite;

/* PPU Control Register ($2000) configuration */
struct ppu_parameters {
	byte name_table_address;    /* 0-3: base nametable ($2000/$2400/$2800/$2C00) */
	byte sprite_address;        /* 0-1: sprite pattern table ($0000/$1000) */
	byte background_address;    /* 0-1: background pattern table ($0000/$1000) */
	byte turn_nmi;              /* 0-1: enable/disable NMI on VBlank */
	byte sprite_size;           /* 0-1: sprite size (8x8 / 8x16) */
};

/* Shared variables with assembly NMI handler */
extern volatile int time_cnt;
extern volatile unsigned char count_freq;

#endif /* NESI65_H */
