/*
 * Hanuman Typing Warrior - NES Typing Game
 *
 * A port of Playpower Foundation's Hanuman Typing Warrior game.
 * The player types words displayed on screen using a Family Basic
 * keyboard. Each correct keystroke advances Hanuman towards a rock
 * tower, which he breaks after completing each word.
 *
 * Author:  Ambuj Varshney <ambuj_varshney(at)daiict.ac.in>
 * License: GNU GPL v3
 */

#include "hanuman_typing.h"

/* Delay between sprite changes during rock breaking animation */
#define DELAY 30

/* Delay between walking steps (larger = slower walk) */
#define WALK_DELAY 150

/* Number of attack animation cycles when breaking rock */
#define NO_OF_ATTACKS 10

/*
 * Hanuman walking animation tile data.
 * 3 animation states x 16 tiles each = 48 tiles total.
 * State 0: walking frame A
 * State 1: standing/idle
 * State 2: attack pose
 */
byte hanuman_movmnt[] = {
	0x79, 0x7A, 0x7B, 0x7C, 0x89, 0x8A, 0x7B, 0x8C,
	0x99, 0x9A, 0x9B, 0x9C, 0xA9, 0xAA, 0xAB, 0xAC,
	0x75, 0x76, 0x77, 0x78, 0x85, 0x86, 0x87, 0x88,
	0x95, 0x96, 0x97, 0x98, 0xA5, 0xA6, 0xA7, 0xA8,
	0xB5, 0xB6, 0xB7, 0xB8, 0xC5, 0xC6, 0xC7, 0xC8,
	0xD5, 0xD6, 0xD7, 0xD8, 0xE5, 0xE6, 0xE7, 0xE8
};

#define NO_WORDS 5

/* Starting index of each word in words_display[] */
byte start_pos[] = {0, 4, 8, 11, 16};

/* Length of each word */
byte wordlen[] = {4, 4, 3, 5, 3};

/* Characters for all words: ROCK, JACK, DOG, PAPER, RAT */
byte words_display[] = {
	'R', 'O', 'C', 'K',
	'J', 'A', 'C', 'K',
	'D', 'O', 'G',
	'P', 'A', 'P', 'E', 'R',
	'R', 'A', 'T'
};

/* Animate one walking step: update tiles and advance X position */
void hanmovmnt(nesi_sprite *hanuman, byte step_size, byte offset)
{
	int i;

	for (i = 0; i < 16; i++) {
		(hanuman + i)->tile = hanuman_movmnt[(offset * 16) + i];
		(hanuman + i)->x = (hanuman + i)->x + step_size;
	}
}

/* Animate Hanuman walking forward by step_size pixels */
void simulate_hanmovmnt(nesi_sprite *hanuman, byte step_size, byte offset)
{
	int i, j;
	byte PPU_status;

	for (i = 0; i < step_size - 16; i += 8) {
		hanmovmnt(hanuman, 8, offset);

		while (1) {
			PPU_status = nesi_return_PPU_status();
			if (nesi_vblank(PPU_status))
				break;
		}

		nesi_sprite_dma(2, 0);

		for (j = 0; j < WALK_DELAY; j++)
			;
	}
}

/* Animate Hanuman's rock-breaking attack by alternating sprite states */
void simulate_hanbreaking(nesi_sprite *hanuman)
{
	int i, j;

	for (i = 0; i < NO_OF_ATTACKS; i++) {
		if ((i % 2) == 0)
			hanmovmnt(hanuman, 0, 2);
		else
			hanmovmnt(hanuman, 0, 1);

		nesi_waitvblank();
		nesi_sprite_dma(2, 0);

		for (j = 0; j < DELAY; j++)
			;
	}
}

/* Animate rock tower destruction (pieces scatter right and down) */
void rockmovement(nesi_sprite *rock)
{
	int i, j;

	for (i = 0; i < 45; i++) {
		rock->x = rock->x + 1;

		for (j = 0; j < 21; j++) {
			if (j % 2 == 0) {
				if ((rock + j)->x > 248) {
					(rock + j)->x = (rock + j)->x + 1;
					(rock + j)->attr = 32;
				} else {
					(rock + j)->x = (rock + j)->x + 1;
				}
			} else if (j % 2 == 1) {
				if ((rock + j)->y > 232) {
					(rock + j)->y = (rock + j)->y + 2;
					(rock + j)->attr = 32;
				} else {
					(rock + j)->y = (rock + j)->y + 2;
				}
			}
		}

		nesi_waitvblank();
		nesi_sprite_dma(2, 0);
	}
}

/* Initialize Hanuman sprite from the hanuman_sprite[] data table */
void draw_hanuman(nesi_sprite *hanuman_sprites)
{
	int i, j;

	for (i = 1; i < 16; i++) {
		++hanuman_sprites;
		j = 4 * i;
		hanuman_sprites->y = hanuman_sprite[j];
		j++;
		hanuman_sprites->tile = hanuman_sprite[j];
		j++;
		hanuman_sprites->attr = hanuman_sprite[j];
		j++;
		hanuman_sprites->x = hanuman_sprite[j];
	}
}

/* Initialize rock tower sprite from the rock_sprite[] data table */
void draw_rock(nesi_sprite *rck_sprite)
{
	int i, j;

	for (i = 0; i < 20; i++) {
		++rck_sprite;
		j = 4 * i;
		rck_sprite->y = rock_sprite[j];
		j++;
		rck_sprite->tile = rock_sprite[j];
		j++;
		rck_sprite->attr = rock_sprite[j];
		j++;
		rck_sprite->x = rock_sprite[j];
	}
}


void main()
{
	int i, j;
	int step_size;
	int temp;

	nesi_sprite *hanuman_sprites;
	nesi_sprite *begin;
	nesi_sprite *charact;
	nesi_sprite *charact_begin;
	nesi_sprite *rck_sprite;

	byte ch;
	byte new_wrd = 0;
	byte PPU_status;
	int present_word = 0;
	int present_char = 0;

	nesi_init();
	nesi_disable_graphics();

	/* Load palettes */
	nesi_copy_vram(bkg_pal, 16, nesi_back_palette);
	nesi_copy_vram(spr_pal, 16, nesi_sprite_palette);

	/* Write background nametable to VRAM */
	for (i = 0; i < 1010; i++) {
		j = nesi_name_table_0 + i;
		nesi_put_vram(j, nametable[i]);
	}

	/* Initialize sprite RAM */
	nesi_fill_ram(0x0200, 0x0100, 0);

	hanuman_sprites = (nesi_sprite *)0x0200;
	begin = (nesi_sprite *)0x0200;

	hanuman_sprites->tile = 0x75;
	hanuman_sprites->attr = 0x01;
	hanuman_sprites->x = 0x10;
	hanuman_sprites->y = 0xA0;

	draw_hanuman(hanuman_sprites);

	rck_sprite = hanuman_sprites + 15;
	draw_rock(rck_sprite);

	charact = rck_sprite + 20;
	charact_begin = rck_sprite + 20;

	nesi_enable_graphics();
	nesi_sprite_dma(2, 0);
	nesi_waitvblank();

	/* Main game loop - process each word */
	while (1) {
		nesi_initkeyb();

		if (present_word >= NO_WORDS)
			break;

		/* Set up display for current word */
		if (new_wrd == 0) {
			for (i = start_pos[present_word];
			     i < (start_pos[present_word] + wordlen[present_word]); i++) {
				++charact;
				charact->y = 0xD8;
				if (words_display[i] < 80)
					charact->tile = 0x41 + (words_display[i] - 65);
				else
					charact->tile = 0x50 + (words_display[i] - 80);
				charact->x = 0x78 + 8 * (i - start_pos[present_word]);
				charact->attr = 0x00;
			}

			/* Hide leftover characters from previous longer word */
			if (present_word > 0 &&
			    (wordlen[present_word] < wordlen[present_word - 1])) {
				for (i = 0; i < (wordlen[present_word - 1] - wordlen[present_word]); i++) {
					++charact;
					charact->tile = 0;
				}
			}

			new_wrd = 1;
			present_char = start_pos[present_word];
		}

		ch = nesi_readkeyb();

		/* Check if typed character matches expected */
		if (ch == words_display[present_char]) {
			(charact - (wordlen[present_word] - (present_char - start_pos[present_word])) + 1)->attr = 3;
			present_char++;

			step_size = 0xD8 - 8;
			temp = wordlen[present_word];
			step_size = step_size / temp;

			if (((present_char - start_pos[present_word])) == wordlen[present_word]) {
				new_wrd = 0;
				present_word++;
				charact = charact_begin;
			}

			simulate_hanmovmnt(hanuman_sprites, step_size,
				((present_char - start_pos[present_word]) % 2));
		}

		nesi_waitvblank();
		nesi_sprite_dma(2, 0);

		/* After completing a word, animate rock breaking */
		if (new_wrd == 0) {
			simulate_hanbreaking(hanuman_sprites);
			rockmovement(rck_sprite);
			draw_rock(rck_sprite);
			draw_hanuman(hanuman_sprites);
		}
	}
}
