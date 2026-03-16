/*
 * Number Muncher - NES Math Education Game
 *
 * A grid-based game where the player controls a "muncher" that must
 * eat numbers matching a mathematical rule (multiples or factors)
 * while avoiding troggle enemies.
 *
 * Controls: D-pad to move, SELECT to eat a number
 * Game modes: 1=Multiples, 2=Factors
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "number_muncher.h"
#include "number_muncher_sprite.h"

#define BOUNDRY_X_INITIAL 1
#define BOUNDRY_Y_INITIAL 40
#define BOUNDRY_X_FINAL 230
#define BOUNDRY_Y_FINAL 170
#define MUNCHER_SPEED 12
#define MAX_NUMBER 114

#define BACKGROUND_OPERATION_TEXT 107
#define BACKGROUND_LIFE_TEXT 885

/* Shared variables with assembly NMI handler for tile updates */
extern volatile unsigned char tile_display_high;
extern volatile unsigned char tile_position_high;
extern volatile unsigned char tile_position_low;
extern volatile unsigned char tile_flag;
extern volatile unsigned char tile_display_low;
extern volatile unsigned char multiple_display;
extern volatile unsigned char tile_series_value[];

/* master_prng, prng, delay, init_prng declared via nesi65-misc.h */

int nm_strlen(char *str);

/* Grid cell screen positions (pixels) */
byte grid_number_positions_x[6] = {10, 50, 90, 130, 170, 210};
byte grid_number_positions_y[5] = {43, 80, 110, 140, 170};

/* Nametable tile offsets for each grid cell */
int grid_tile_pos[5][6] = {
	{227, 232, 237, 242, 247, 252},
	{355, 360, 365, 370, 375, 380},
	{483, 488, 493, 498, 503, 508},
	{611, 616, 621, 626, 631, 636},
	{739, 744, 749, 754, 759, 764}
};

int grid_possible_jumps_x[6] = {10, 50, 90, 130, 170, 210};
int grid_possible_jumps_y[5] = {43, 80, 100, 150, 175};

int grid_tile_positions[] = {
	227, 232, 239, 242, 247, 252,
	355, 361, 366, 370, 376, 380,
	483, 488, 493, 503,
	611, 616, 621, 626, 631, 636,
	739, 744, 749, 754, 760, 765
};

byte grid_positions[30];

int no_lives;
int score;

/* Current grid number values (-1 = eaten) */
int grid_number[5][6] = {
	{1, 3, 5, 6, 2, 8},
	{2, 5, 9, 4, 1, 7},
	{4, 9, 1, 3, 4, 8},
	{4, 9, 7, 6, 5, 8},
	{8, 8, 2, 3, 1, 4}
};


int nm_abs(int n)
{
	if (n < 0)
		return (-1 * n);
	else
		return n;
}

int abc(char a, char b)
{
	return b;
}

void reset_latch()
{
	int addr;
	addr = a_addr(0x2002);
}

/* Draw a numeric value as sprite tiles at (x, y) */
void draw_number(nesi_sprite *sprite, int number, int x, int y)
{
	int dig = number;
	int cnt = 0;

	while (dig > 0) {
		++sprite;
		sprite->y = y;
		sprite->tile = (dig % 10) + 0x90;
		sprite->x = x - (cnt * 8);
		sprite->attr = 0x01;
		dig = dig / 10;
		cnt++;
	}
}

/* Blank a horizontal strip of background tiles */
void remove_line(int start, int no_line)
{
	int blank_pos = (nesi_name_table_0 + start);
	int i;

	for (i = 0; i < no_line; i++) {
		blank_pos = blank_pos + (i * 32);
		nesi_blank_background(blank_pos, 32);
	}
}

/* Fill the 5x6 grid with random numbers using the PRNG */
void init_grid(int m)
{
	int i, j;
	int val;

	for (i = 0; i < 6; i++) {
		for (j = 0; j < 5; j++) {
			val = master_prng(m);
			if (val < 0)
				val = val * -1;
			else if (val == 0)
				val = 1;
			grid_number[j][i] = val;
		}
	}
}

/* Write a text string to the background nametable during VBlank */
void draw_background_text(int pos, char *text, int len)
{
	int i;
	byte char_tile;

	for (i = 0; i < len; i++) {
		if (*text == ' ')
			char_tile = 0xff;
		else if ((*text >= 48) && (*text <= 57))
			char_tile = 0x30 + (*text - 48);
		else
			char_tile = ((*text - 65) + 0x41);

		tile_display_high = char_tile;
		tile_position_high = highbyte((nesi_name_table_0 + pos + i));
		tile_position_low = lowbyte((nesi_name_table_0 + pos + i));
		tile_flag = 1;
		while (tile_flag != 0)
			;
		++text;
	}
}

/* Redraw numbers in grid rows [x, y) to fix display after error screens */
void section_grid(int x, int y)
{
	int i, j;
	int pos;
	int cnt = 0;
	int dig;

	for (i = x; i < y; i++)
		for (j = 0; j < 6; j++) {
			pos = nesi_name_table_0 + grid_tile_pos[i][j];

			if (grid_number[i][j] == -1) {
				tile_display_high = 0xff;
				tile_position_high = highbyte(pos);
				tile_position_low = lowbyte(pos);
				tile_flag = 1;
				while (tile_flag != 0)
					;
			} else {
				dig = grid_number[i][j];
				while (dig > 0) {
					tile_display_high = 0x30 + (dig % 10);
					tile_position_high = highbyte((pos - cnt));
					tile_position_low = lowbyte((pos - cnt));
					tile_flag = 1;
					while (tile_flag != 0)
						;
					dig = dig / 10;
					cnt++;
				}
			}
			cnt = 0;
		}
}

/* Render the 4x4 tile muncher sprite at (x, y) */
void draw_muncher(nesi_sprite *muncher_sprite, int x, int y)
{
	int i;

	for (i = 0; i < 16; i++) {
		++muncher_sprite;
		muncher_sprite->y = y + ((i / 4) * 8);
		muncher_sprite->tile = number_muncher_sprite[i];
		muncher_sprite->attr = 0x01;
		muncher_sprite->x = x + ((i % 4) * 8);
	}
}

int nm_strlen(char *str)
{
	int cnt = 0;

	while (*(str + cnt) != '\0')
		cnt++;
	return cnt;
}

/* Display "eaten by troggle" or "game over" error message */
void display_error_troggle(int error_no)
{
	int addr;
	int i;
	unsigned char tile_char;
	char ch;
	char error_text[6][40] = {
		"AARGH YOU WERE EATEN BY A",
		"TROGGLE",
		"PRESS SELECT TO CONTINUE",
		"YOU HAVE LOST ALL YOUR LIFE",
		"PRESS SELECT TO GO BACK",
		"TO THE MAIN MENU"
	};

	addr = a_addr(0x2006);
	addr = a_addr(0x2006);

	for (i = 416; i < 640; i++) {
		nesi_blank_background((nesi_name_table_0 + i), 32);
		i = i + 32;
	}

	addr = a_addr(0x2006);

	if (error_no == 0) {
		draw_background_text(451, error_text[0], nm_strlen(error_text[0]));
		draw_background_text(493, error_text[1], nm_strlen(error_text[1]));
		draw_background_text(548, error_text[2], nm_strlen(error_text[2]));
	} else if (error_no == 1) {
		draw_background_text(451, error_text[3], nm_strlen(error_text[3]));
		draw_background_text(486, error_text[4], nm_strlen(error_text[4]));
		draw_background_text(520, error_text[5], nm_strlen(error_text[5]));
	}

	while (1) {
		ch = nesi_get_joy(0x4016);
		if (ch == nesi_SELECT) {
			for (i = 405; i < 640; i++) {
				tile_display_high = nametable[i];
				tile_position_high = highbyte(nesi_name_table_0 + i);
				tile_position_low = lowbyte(nesi_name_table_0 + i);
				tile_flag = 1;
				while (tile_flag != 0)
					;
			}
			section_grid(2, 4);
			return;
		}
	}
}

/* Display "wrong number" error message */
void display_error(char num)
{
	int addr;
	int i;
	unsigned char tile_char;
	char of[4];
	char ch;

	addr = a_addr(0x2006);
	addr = a_addr(0x2006);

	for (i = 416; i < 640; i++) {
		nesi_blank_background((nesi_name_table_0 + i), 32);
		i = i + 32;
	}

	addr = a_addr(0x2006);

	draw_background_text(451, "OOPS THIS IS NOT MULTIPLE", 25);
	of[0] = 'O';
	of[1] = 'F';
	of[2] = ' ';
	of[3] = num + 48;
	draw_background_text(493, of, 4);

	draw_background_text(548, "PRESS SELECT TO CONTINUE", 24);

	while (1) {
		ch = nesi_get_joy(0x4016);
		if (ch == nesi_SELECT) {
			for (i = 410; i < 640; i++) {
				tile_display_high = nametable[i];
				tile_position_high = highbyte(nesi_name_table_0 + i);
				tile_position_low = lowbyte(nesi_name_table_0 + i);
				tile_flag = 1;
				while (tile_flag != 0)
					;
			}
			section_grid(2, 4);
			return;
		}
	}
}

/* Render a 4x4 troggle enemy sprite (hide=0 to make invisible) */
void draw_troggle(nesi_sprite *troggle_sprite, byte troggle[], int x, int y, int hide)
{
	int i;

	for (i = 0; i < 16; i++) {
		++troggle_sprite;
		troggle_sprite->y = y + ((i / 4) * 8);
		troggle_sprite->tile = ((hide != 0) ? troggle[i] : 0xff);
		troggle_sprite->attr = 0x01;
		troggle_sprite->x = x + ((i % 4) * 8);
	}
}

/* Draw remaining lives as small muncher sprites */
void display_lives(nesi_sprite *muncher_sprite)
{
	int x, y, i, j;

	x = 150;
	y = 200;

	for (j = 0; j < no_lives; j++) {
		for (i = 0; i < 16; i++) {
			++muncher_sprite;
			muncher_sprite->y = y + ((i / 4) * 8);
			muncher_sprite->tile = number_muncher_sprite[i];
			muncher_sprite->attr = 0x01;
			muncher_sprite->x = (j * 30) + (x + ((i % 4) * 8));
		}
	}
}

/* Count how many correct numbers remain to finish the level */
int score_to_finish(int level_no, int type)
{
	int i, j;
	int score = 0;

	for (i = 0; i < 5; i++)
		for (j = 0; j < 6; j++) {
			if (grid_number[i][j] != -1 && grid_number[i][j] != 0) {
				if ((((grid_number[i][j] % level_no) == 0) && type == 0) ||
				    (((level_no % grid_number[i][j]) == 0) && type == 1))
					score = score + 10;
			}
		}

	return score;
}

/* Play the muncher eating animation at current position */
void animate_muncher_eating(nesi_sprite *muncher_sprite, int x, int y)
{
	int i;

	for (i = 0; i < 16; i++) {
		++muncher_sprite;
		muncher_sprite->y = y + ((i / 4) * 8);
		muncher_sprite->tile = number_muncher_eatsprite[i];
		muncher_sprite->attr = 0x01;
		muncher_sprite->x = x + ((i % 4) * 8);
	}

	nesi_sprite_dma(2, 0);
	delay(4);
}

/*
 * Main gameplay loop.
 * Handles player input, number validation, troggle spawning/movement,
 * collision detection, and score tracking.
 *   level: the target number for multiples/factors
 *   type:  0=multiples, 1=factors
 */
void display_level(int x, int y, nesi_sprite **muncher_sprite,
                   nesi_sprite **scre, nesi_sprite **no_of_lives,
                   nesi_sprite **troggle_sprite, int level, int type)
{
	char ch;
	int i;
	int time2 = 0;
	int troggle_x;
	int troggle_y;
	int troggle_active = 0;
	int last_troggle_pos = 0;
	int last_troggle_pos_y = 0;
	int last_time2 = 0;

	int last_score = score;
	int present_score = 0;
	int score_level = score_to_finish(level, type);

	while (1) {
		ch = nesi_get_joy(0x4016);

		if (ch == nesi_RIGHT) {
			if (x < 5) x++;
		} else if (ch == nesi_LEFT) {
			if (x > 0) x--;
		} else if (ch == nesi_DOWN) {
			if (y < 4) y++;
		} else if (ch == nesi_UP) {
			if (y > 0) y--;
		} else if (ch == nesi_SELECT) {
			animate_muncher_eating(*muncher_sprite,
				grid_number_positions_x[x], grid_number_positions_y[y]);

			if ((((grid_number[y][x] % level) == 0) && type == 0) ||
			    (((level % grid_number[y][x]) == 0) && type == 1) &&
			    grid_number[y][x] != -1) {
				int dig;
				int to_disappear;
				int cnt;

				dig = grid_number[y][x];
				score = score + 10;
				to_disappear = nesi_name_table_0 + grid_tile_pos[y][x];
				to_disappear_high = highbyte(to_disappear);
				to_disappear_low = lowbyte(to_disappear);
				cnt = 0;

				while (dig > 0) {
					dig = dig / 10;
					cnt++;
				}

				no_disappear = cnt;
				to_disappear_low = to_disappear_low - cnt;
				vblank_flag = 1;
				grid_number[y][x] = -1;

				present_score += 10;
				if (present_score == score_level)
					return;
			} else if (grid_number[y][x] != -1) {
				display_error(2);
				if (grid_number[y][x] != -1)
					no_lives--;
			}
		}

		draw_muncher(*muncher_sprite,
			grid_number_positions_x[x], grid_number_positions_y[y]);

		/* Check collision with troggle */
		if ((grid_number_positions_x[x] == troggle_x) &&
		    (grid_number_positions_y[y] == troggle_y)) {
			x = 0;
			y = 0;
			no_lives--;
			display_error_troggle(0);
		}

		draw_number(*no_of_lives, no_lives, 210, 215);
		draw_number(*scre, score, 78, 215);

		for (i = 0; i < 192; i++)
			;

		time2 = time_cnt;

		/* Spawn vertical troggle every 40 ticks */
		if (((time2 % 40) == 0) && (time2 > 0) && (troggle_active == 0)) {
			int start;

			init_prng(x * 4, y * 10);
			start = master_prng(5);
			if (start == last_troggle_pos_y) {
				init_prng(x * 3, y * x);
				start = master_prng(5);
				if (start == last_troggle_pos_y)
					start = (start + 2) % 5;
			}

			troggle_active = 2;
			troggle_x = grid_number_positions_x[start];
			troggle_y = grid_number_positions_y[0];
			draw_troggle(*troggle_sprite, reggies_troggle_sprite,
				troggle_x, troggle_y, troggle_active);
			draw_background_text(BACKGROUND_LIFE_TEXT - 8, "TROGGLE", 7);
			last_troggle_pos_y = start;

		/* Spawn horizontal troggle every 25 ticks */
		} else if (((time2 % 25) == 0) && (time2 > 0) && (troggle_active == 0)) {
			int start;

			init_prng(x * 4, y * 10);
			start = master_prng(4);
			if (start == last_troggle_pos) {
				init_prng(x * 3, y * x);
				start = master_prng(4);
				if (start == last_troggle_pos)
					start = (start + 3) % 4;
			}

			troggle_active = 1;
			troggle_y = grid_number_positions_y[start];
			if (troggle_y > 170)
				troggle_y = 170;
			troggle_x = grid_number_positions_x[0];

			draw_troggle(*troggle_sprite, reggies_troggle_sprite,
				troggle_x, troggle_y, troggle_active);
			draw_background_text(BACKGROUND_LIFE_TEXT - 8, "TROGGLE", 7);
			last_troggle_pos = start;

		/* Move active troggle every 7 ticks */
		} else if (((time2 % 7) == 0) && (time2 != 0) &&
		           (time2 != last_time2) && (troggle_active > 0)) {
			if (troggle_active == 1)
				troggle_x = troggle_x + 40;
			else if (troggle_active == 2)
				troggle_y = troggle_y + 30;

			if ((troggle_x > 210 && troggle_active == 1) ||
			    (troggle_y > 175 && troggle_active == 2)) {
				troggle_active = 0;
				draw_background_text(BACKGROUND_LIFE_TEXT - 8, "       ", 7);
			}

			draw_troggle(*troggle_sprite,
				(troggle_active == 1) ? reggies_troggle_sprite : reggies_troggle_sprite,
				troggle_x, troggle_y, troggle_active);
			last_time2 = time2;
		}

		if (no_lives <= 0) {
			display_error_troggle(1);
			return;
		}

		nesi_sprite_dma(2, 0);
	}
}

/* Draw text as sprites (used for HUD elements) */
void draw_text(nesi_sprite *sprite, char *text, int len, int x, int y)
{
	int i;

	for (i = 0; i < len; i++) {
		++sprite;
		sprite->y = y;
		if (*text == ' ')
			sprite->tile = 0x70;
		else if ((*text >= 48) && (*text <= 57))
			sprite->tile = 0x90 + (*text - 48);
		else
			sprite->tile = (*text - 65) + 0xA1;
		sprite->x = x + i * 8;
		sprite->attr = 0x01;
		++text;
	}
}

/* Write number tiles directly to the nametable VRAM */
void draw_grid_nametable()
{
	int i, j;
	int pos;
	int cnt = 0;
	int dig;

	for (i = 0; i < 5; i++)
		for (j = 0; j < 6; j++) {
			pos = nesi_name_table_0 + grid_tile_pos[i][j];

			if (grid_number[i][j] == -1) {
				nesi_put_vram(pos, 0xff);
			} else {
				dig = grid_number[i][j];
				while (dig > 0) {
					nesi_put_vram(pos - cnt, 0x30 + (dig % 10));
					dig = dig / 10;
					cnt++;
				}
			}
			cnt = 0;
		}
}

void eat_number(int i, int j)
{
}


int main()
{
	int i, j;
	int x, y;
	char ch;
	int temp;
	int cnt = 0;
	int option_chosen = -1;
	int last_level = 2;

	nesi_sprite *muncher_sprite;
	nesi_sprite *text_sprite;
	nesi_sprite *number;
	nesi_sprite *muncher_lives;
	nesi_sprite *no_of_lives;
	nesi_sprite *scre;
	nesi_sprite *highlight;
	nesi_sprite *troggle_sprite;

	struct ppu_parameters p;

	score = 0;

	nesi_init();
	nesi_disable_graphics();

	count_freq = 15;

	p.sprite_address = 1;
	p.background_address = 0;
	p.turn_nmi = 1;
	p.sprite_size = 0;
	p.name_table_address = 0;

	temp = nesi_ppu_parameter_address(&p);

	nesi_copy_vram(bkg_pal, 16, nesi_back_palette);
	nesi_copy_vram(spr_pal, 16, nesi_sprite_palette);

	nesi_enable_graphics();

	nesi_fill_ram(0x0200, 0x0200, 0);
	highlight = (nesi_sprite *)0x0200;

	cnt = 0;
	x = 0;

	muncher_sprite = highlight + 2;
	text_sprite = muncher_sprite + 16;
	troggle_sprite = text_sprite + 6;
	scre = troggle_sprite + 16;
	no_of_lives = text_sprite + 6;

	x = 0;
	y = 0;

	nesi_enable_graphics();
	nesi_sprite_dma(2, 0);

	/* Main game loop */
	while (1) {
		if (option_chosen == -1) {
			/* Title/menu screen */
			nesi_disable_graphics();
			nesi_fill_ram(0x0200, 0x0200, 0);

			no_lives = 3;
			score = 0;

			highlight->y = 78;
			highlight->attr = 0x01;
			highlight->x = 8;
			highlight->tile = 0x9E;

			nesi_put_background(nametable_open, i);
			nesi_enable_graphics();

			while (1) {
				ch = nesi_get_joy(0x4016);

				if (ch == nesi_SELECT) {
					if ((highlight->y) > 75 && (highlight->y) <= 84) {
						reset_latch();
						nesi_disable_graphics();
						option_chosen = 0;
						break;
					} else if ((highlight->y) >= 85 && (highlight->y) <= 95) {
						reset_latch();
						nesi_disable_graphics();
						option_chosen = 1;
						last_level = 3;
						break;
					}
				} else if (ch == nesi_DOWN) {
					if ((highlight->y) < 155)
						highlight->y = highlight->y + 4;
				} else if (ch == nesi_UP) {
					if ((highlight->y) > 80) {
						highlight->y = highlight->y - 4;
						cnt--;
					}
				}

				waitvblank();
				nesi_sprite_dma(2, 0);
			}
		} else if (option_chosen == 0) {
			/* Multiples mode */
			nesi_disable_graphics();
			nesi_put_background(nametable, i);
			nesi_fill_ram(0x0200, 0x0200, 0);

			if (((x + y) % 2) == 0)
				init_prng(12, 8 + (x + y));
			else
				init_prng(12, 8 + (x + y + 1));

			init_grid(MAX_NUMBER);
			draw_grid_nametable();

			nesi_enable_graphics();
			draw_background_text(BACKGROUND_OPERATION_TEXT, "MULTIPLES OF", 12);
			draw_background_text(BACKGROUND_LIFE_TEXT, "LIFE:", 5);

			draw_number(text_sprite, last_level, 195, 23);
			draw_number(text_sprite + 3, last_level - 1, 70, 23);
			display_level(x, y, &muncher_sprite, &scre, &no_of_lives,
				&troggle_sprite, last_level, 0);

			if (no_lives > 0)
				last_level++;
			else
				option_chosen = -1;

		} else if (option_chosen == 1) {
			/* Factors mode */
			nesi_disable_graphics();
			nesi_put_background(nametable, i);
			nesi_fill_ram(0x0200, 0x0200, 0);

			if (((x + y) % 2) == 0)
				init_prng(12, 8 + (x + y));
			else
				init_prng(12, 8 + (x + y + 1));

			init_grid(last_level + 1);
			draw_grid_nametable();

			nesi_enable_graphics();
			draw_background_text(BACKGROUND_OPERATION_TEXT, "FACTORS OF", 10);
			draw_background_text(BACKGROUND_LIFE_TEXT, "LIFE:", 5);

			draw_number(text_sprite, last_level, 195, 23);
			draw_number(text_sprite + 3, last_level - 1, 70, 23);
			display_level(x, y, &muncher_sprite, &scre, &no_of_lives,
				&troggle_sprite, last_level, 1);

			if (no_lives > 0)
				last_level++;
			else
				option_chosen = -1;
		}
	}
}
