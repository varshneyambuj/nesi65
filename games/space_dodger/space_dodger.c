/*
 * Space Dodger - NES Space Shooter Game
 *
 * Dodge and destroy falling asteroids in space!
 * Demonstrates use of NESI65 library modules:
 *   - nesi65-joystick: controller input
 *   - nesi65-collision: AABB hit detection
 *   - nesi65-sound: APU sound effects
 *   - nesi65-text: nametable text rendering
 *   - nesi65-math: utility math functions
 *   - nesi65-misc: PRNG and delay
 *
 * Controls: D-pad to move, A to shoot, START to begin/pause
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "space_dodger.h"

/* Shared variables with assembly NMI handler */
extern volatile unsigned char tile_display_high;
extern volatile unsigned char tile_position_high;
extern volatile unsigned char tile_position_low;
extern volatile unsigned char tile_flag;

/* Game state */
int ship_x;
int ship_y;
int score;
int lives;
int level;
int game_over;
int asteroid_speed;
int spawn_timer;
int spawn_rate;
int invincible_timer;

/* Asteroid pool */
int asteroid_x[MAX_ASTEROIDS];
int asteroid_y[MAX_ASTEROIDS];
byte asteroid_active[MAX_ASTEROIDS];
byte asteroid_type[MAX_ASTEROIDS]; /* 0 or 1 for visual variant */

/* Bullet pool */
int bullet_x[MAX_BULLETS];
int bullet_y[MAX_BULLETS];
byte bullet_active[MAX_BULLETS];

/* OAM sprite pointer base */
nesi_sprite *oam_base;

/* ----------------------------------------------------------------
 * Sprite drawing helpers
 * ---------------------------------------------------------------- */

/* Draw the player ship (2x3 tiles = 16x24 pixels) at (x, y) */
void draw_ship(nesi_sprite *spr, int x, int y, byte visible)
{
	int i, row, col;

	for (i = 0; i < 6; i++) {
		row = i / 2;
		col = i % 2;
		spr->y = y + (row * 8);
		spr->tile = visible ? ship_tiles[i] : 0xFF;
		spr->attr = 0x00; /* palette 0 */
		spr->x = x + (col * 8);
		++spr;
	}
}

/* Draw an asteroid (2x2 tiles = 16x16 pixels) at (x, y) */
void draw_asteroid(nesi_sprite *spr, int x, int y, byte type, byte visible)
{
	int i, row, col;
	byte *tiles;

	tiles = (type == 0) ? asteroid_tiles_a : asteroid_tiles_b;

	for (i = 0; i < 4; i++) {
		row = i / 2;
		col = i % 2;
		spr->y = y + (row * 8);
		spr->tile = visible ? tiles[i] : 0xFF;
		spr->attr = 0x02; /* palette 2 */
		spr->x = x + (col * 8);
		++spr;
	}
}

/* Draw a bullet (single 8x8 tile) */
void draw_bullet(nesi_sprite *spr, int x, int y, byte visible)
{
	spr->y = y;
	spr->tile = visible ? BULLET_TILE : 0xFF;
	spr->attr = 0x01; /* palette 1 */
	spr->x = x + 4; /* center on ship */
}

/* Draw an explosion effect (2x2 tiles) */
void draw_explosion(nesi_sprite *spr, int x, int y)
{
	int i, row, col;

	for (i = 0; i < 4; i++) {
		row = i / 2;
		col = i % 2;
		spr->y = y + (row * 8);
		spr->tile = explosion_tiles[i];
		spr->attr = 0x03; /* palette 3 */
		spr->x = x + (col * 8);
		++spr;
	}
}

/* Clear an explosion sprite off screen */
void clear_explosion(nesi_sprite *spr)
{
	int i;

	for (i = 0; i < 4; i++) {
		spr->y = 0xF0; /* off screen */
		spr->tile = 0xFF;
		++spr;
	}
}

/* ----------------------------------------------------------------
 * HUD update via VBlank tile writes
 * ---------------------------------------------------------------- */

/* Write a single background tile during VBlank using the NMI handler */
void write_bg_tile(word position, byte tile)
{
	tile_display_high = tile;
	tile_position_high = highbyte(nesi_name_table_0 + position);
	tile_position_low = lowbyte(nesi_name_table_0 + position);
	tile_flag = 1;
	while (tile_flag != 0)
		;
}

/* Update the score display on the HUD (nametable row 1, after "SCORE:") */
void update_score_display(void)
{
	word pos;
	int dig;
	byte cnt;
	byte digits[5];

	pos = 39; /* row 1, column 7 */

	cnt = 0;
	dig = score;
	if (dig == 0) {
		digits[0] = 0;
		cnt = 1;
	} else {
		while (dig > 0 && cnt < 5) {
			digits[cnt] = (byte)(dig % 10);
			dig = dig / 10;
			cnt++;
		}
	}

	/* Write up to 5 digits right-aligned */
	while (cnt < 5) {
		digits[cnt] = 0xFF; /* blank */
		cnt++;
	}

	/* Write digits MSB first at positions 39-43 */
	write_bg_tile(39, (digits[4] <= 9) ? (0x30 + digits[4]) : 0xFF);
	write_bg_tile(40, (digits[3] <= 9) ? (0x30 + digits[3]) : 0xFF);
	write_bg_tile(41, (digits[2] <= 9) ? (0x30 + digits[2]) : 0xFF);
	write_bg_tile(42, (digits[1] <= 9) ? (0x30 + digits[1]) : 0xFF);
	write_bg_tile(43, 0x30 + digits[0]);
}

/* Update the lives display on the HUD */
void update_lives_display(void)
{
	write_bg_tile(61, 0x30 + (byte)lives); /* row 1, column 29 */
}

/* ----------------------------------------------------------------
 * Game object management
 * ---------------------------------------------------------------- */

void init_asteroids(void)
{
	int i;

	for (i = 0; i < MAX_ASTEROIDS; i++) {
		asteroid_x[i] = 0;
		asteroid_y[i] = 0;
		asteroid_active[i] = 0;
		asteroid_type[i] = 0;
	}
}

void init_bullets(void)
{
	int i;

	for (i = 0; i < MAX_BULLETS; i++) {
		bullet_x[i] = 0;
		bullet_y[i] = 0;
		bullet_active[i] = 0;
	}
}

/* Spawn an asteroid at a random X position at the top of the screen */
void spawn_asteroid(void)
{
	int i;
	int x_pos;

	for (i = 0; i < MAX_ASTEROIDS; i++) {
		if (!asteroid_active[i]) {
			x_pos = master_prng(224);
			if (x_pos < PLAY_AREA_LEFT)
				x_pos = PLAY_AREA_LEFT;

			asteroid_x[i] = x_pos;
			asteroid_y[i] = PLAY_AREA_TOP;
			asteroid_active[i] = 1;
			asteroid_type[i] = (byte)(master_prng(2));
			return;
		}
	}
}

/* Fire a bullet from the ship's position */
void fire_bullet(void)
{
	int i;

	for (i = 0; i < MAX_BULLETS; i++) {
		if (!bullet_active[i]) {
			bullet_x[i] = ship_x;
			bullet_y[i] = ship_y - 8;
			bullet_active[i] = 1;
			nesi_sfx_shoot();
			return;
		}
	}
}

/* Move all active asteroids downward */
void update_asteroids(void)
{
	int i;

	for (i = 0; i < MAX_ASTEROIDS; i++) {
		if (asteroid_active[i]) {
			asteroid_y[i] += asteroid_speed;

			/* Deactivate if off bottom of screen */
			if (asteroid_y[i] > PLAY_AREA_BOTTOM) {
				asteroid_active[i] = 0;
			}
		}
	}
}

/* Move all active bullets upward */
void update_bullets(void)
{
	int i;

	for (i = 0; i < MAX_BULLETS; i++) {
		if (bullet_active[i]) {
			bullet_y[i] -= BULLET_SPEED;

			/* Deactivate if off top of screen */
			if (bullet_y[i] < PLAY_AREA_TOP) {
				bullet_active[i] = 0;
			}
		}
	}
}

/* ----------------------------------------------------------------
 * Collision detection
 * ---------------------------------------------------------------- */

/* Get pointer to explosion sprites area in OAM */
nesi_sprite *get_explosion_spr(void)
{
	/* After ship(6) + asteroids(6*4=24) + bullets(4) = 34 sprites */
	return oam_base + 34;
}

/* Check bullet-asteroid collisions */
void check_bullet_hits(void)
{
	int i, j;

	for (i = 0; i < MAX_BULLETS; i++) {
		if (!bullet_active[i])
			continue;

		for (j = 0; j < MAX_ASTEROIDS; j++) {
			if (!asteroid_active[j])
				continue;

			if (nesi_sprite_collide(
				(byte)bullet_x[i], (byte)bullet_y[i],
				BULLET_WIDTH, BULLET_HEIGHT,
				(byte)asteroid_x[j], (byte)asteroid_y[j],
				ASTEROID_WIDTH, ASTEROID_HEIGHT))
			{
				/* Hit! */
				bullet_active[i] = 0;
				asteroid_active[j] = 0;
				score += POINTS_PER_HIT;
				nesi_sfx_hit();

				/* Show brief explosion */
				draw_explosion(get_explosion_spr(),
					asteroid_x[j], asteroid_y[j]);
				nesi_sprite_dma(2, 0);
				delay(2);
				clear_explosion(get_explosion_spr());

				/* Speed up game at score thresholds */
				if ((score % SPEED_UP_SCORE) == 0 && asteroid_speed < 6) {
					asteroid_speed++;
					level++;
					if (spawn_rate > 15)
						spawn_rate -= 5;
				}
				break;
			}
		}
	}
}

/* Check ship-asteroid collisions */
void check_ship_hits(void)
{
	int i;

	if (invincible_timer > 0)
		return;

	for (i = 0; i < MAX_ASTEROIDS; i++) {
		if (!asteroid_active[i])
			continue;

		if (nesi_sprite_collide(
			(byte)ship_x, (byte)ship_y,
			SHIP_WIDTH, SHIP_HEIGHT,
			(byte)asteroid_x[i], (byte)asteroid_y[i],
			ASTEROID_WIDTH, ASTEROID_HEIGHT))
		{
			/* Ship hit! */
			asteroid_active[i] = 0;
			lives--;
			nesi_sfx_death();

			/* Brief invincibility after being hit */
			invincible_timer = 60;

			/* Show explosion at ship position */
			draw_explosion(get_explosion_spr(), ship_x, ship_y);
			nesi_sprite_dma(2, 0);
			delay(4);
			clear_explosion(get_explosion_spr());

			if (lives <= 0) {
				game_over = 1;
			}

			update_lives_display();
			return;
		}
	}
}

/* ----------------------------------------------------------------
 * Rendering
 * ---------------------------------------------------------------- */

void render_sprites(void)
{
	nesi_sprite *spr;
	int i;
	byte ship_visible;

	spr = oam_base;

	/* Ship: blink during invincibility */
	ship_visible = 1;
	if (invincible_timer > 0) {
		ship_visible = (invincible_timer & 0x04) ? 1 : 0;
	}
	draw_ship(spr, ship_x, ship_y, ship_visible);
	spr += 6;

	/* Asteroids */
	for (i = 0; i < MAX_ASTEROIDS; i++) {
		draw_asteroid(spr, asteroid_x[i], asteroid_y[i],
			asteroid_type[i], asteroid_active[i]);
		spr += 4;
	}

	/* Bullets */
	for (i = 0; i < MAX_BULLETS; i++) {
		draw_bullet(spr, bullet_x[i], bullet_y[i], bullet_active[i]);
		spr += 1;
	}

	nesi_sprite_dma(2, 0);
}

/* ----------------------------------------------------------------
 * Game screens
 * ---------------------------------------------------------------- */

/* Display title screen and wait for START */
void title_screen(void)
{
	int i;
	char ch;

	nesi_disable_graphics();
	nesi_put_background(nametable_title, i);
	nesi_enable_graphics();

	/* Wait for START button */
	while (1) {
		ch = nesi_get_joy(nesi_JOYPAD_1);
		if (nesi_START_Down(ch))
			return;
		waitvblank();
	}
}

/* Display game over screen */
void game_over_screen(void)
{
	char ch;

	/* Write GAME OVER text on the play field */
	write_bg_tile(nesi_rowcol(13, 10), 0x47); /* G */
	write_bg_tile(nesi_rowcol(13, 11), 0x41); /* A */
	write_bg_tile(nesi_rowcol(13, 12), 0x4D); /* M */
	write_bg_tile(nesi_rowcol(13, 13), 0x45); /* E */
	write_bg_tile(nesi_rowcol(13, 15), 0x4F); /* O */
	write_bg_tile(nesi_rowcol(13, 16), 0x56); /* V */
	write_bg_tile(nesi_rowcol(13, 17), 0x45); /* E */
	write_bg_tile(nesi_rowcol(13, 18), 0x52); /* R */

	/* "PRESS START" below */
	write_bg_tile(nesi_rowcol(16, 10), 0x50); /* P */
	write_bg_tile(nesi_rowcol(16, 11), 0x52); /* R */
	write_bg_tile(nesi_rowcol(16, 12), 0x45); /* E */
	write_bg_tile(nesi_rowcol(16, 13), 0x53); /* S */
	write_bg_tile(nesi_rowcol(16, 14), 0x53); /* S */
	write_bg_tile(nesi_rowcol(16, 16), 0x53); /* S */
	write_bg_tile(nesi_rowcol(16, 17), 0x54); /* T */
	write_bg_tile(nesi_rowcol(16, 18), 0x41); /* A */
	write_bg_tile(nesi_rowcol(16, 19), 0x52); /* R */
	write_bg_tile(nesi_rowcol(16, 20), 0x54); /* T */

	nesi_sfx_death();
	delay(8);

	/* Wait for START */
	while (1) {
		ch = nesi_get_joy(nesi_JOYPAD_1);
		if (nesi_START_Down(ch))
			return;
		waitvblank();
	}
}

/* ----------------------------------------------------------------
 * Main game loop
 * ---------------------------------------------------------------- */

void play_game(void)
{
	int i;
	char ch;
	byte shoot_cooldown;
	int frame_count;

	/* Init game state */
	ship_x = SHIP_START_X;
	ship_y = SHIP_START_Y;
	score = 0;
	lives = INITIAL_LIVES;
	level = 1;
	game_over = 0;
	asteroid_speed = 2;
	spawn_timer = 0;
	spawn_rate = 40;
	invincible_timer = 0;
	shoot_cooldown = 0;
	frame_count = 0;

	init_asteroids();
	init_bullets();

	/* Setup play field */
	nesi_disable_graphics();
	nesi_fill_ram(0x0200, 0x0200, 0);
	nesi_put_background(nametable_game, i);
	nesi_enable_graphics();

	/* Init sound */
	nesi_sound_init();
	nesi_sound_enable(0x0F); /* Enable pulse1, pulse2, triangle, noise */

	/* Init PRNG */
	init_prng(42, 17);

	/* Initial HUD */
	update_score_display();
	update_lives_display();

	oam_base = (nesi_sprite *)0x0200;

	/* Main loop */
	while (!game_over) {
		/* Read input */
		ch = nesi_get_joy(nesi_JOYPAD_1);

		/* Movement */
		if (nesi_LEFT_Down(ch)) {
			ship_x -= SHIP_SPEED;
			if (ship_x < PLAY_AREA_LEFT)
				ship_x = PLAY_AREA_LEFT;
		}
		if (nesi_RIGHT_Down(ch)) {
			ship_x += SHIP_SPEED;
			if (ship_x > PLAY_AREA_RIGHT - SHIP_WIDTH)
				ship_x = PLAY_AREA_RIGHT - SHIP_WIDTH;
		}
		if (nesi_UP_Down(ch)) {
			ship_y -= SHIP_SPEED;
			if (ship_y < PLAY_AREA_TOP + 16)
				ship_y = PLAY_AREA_TOP + 16;
		}
		if (nesi_DOWN_Down(ch)) {
			ship_y += SHIP_SPEED;
			if (ship_y > PLAY_AREA_BOTTOM - SHIP_HEIGHT)
				ship_y = PLAY_AREA_BOTTOM - SHIP_HEIGHT;
		}

		/* Shooting with cooldown */
		if (nesi_A_Down(ch) && shoot_cooldown == 0) {
			fire_bullet();
			shoot_cooldown = 8;
		}
		if (shoot_cooldown > 0)
			shoot_cooldown--;

		/* Spawn asteroids on timer */
		spawn_timer++;
		if (spawn_timer >= spawn_rate) {
			spawn_asteroid();
			spawn_timer = 0;
		}

		/* Update game objects */
		update_asteroids();
		update_bullets();

		/* Collision detection */
		check_bullet_hits();
		check_ship_hits();

		/* Update invincibility timer */
		if (invincible_timer > 0)
			invincible_timer--;

		/* Update HUD */
		update_score_display();

		/* Render */
		render_sprites();

		/* Frame pacing */
		waitvblank();

		frame_count++;
	}

	/* Game over */
	nesi_sound_disable();
}

/* ----------------------------------------------------------------
 * Entry point
 * ---------------------------------------------------------------- */

int main(void)
{
	int i;
	struct ppu_parameters p;

	/* Initialize PPU */
	nesi_init();
	nesi_disable_graphics();

	/* Configure PPU */
	count_freq = 15;

	p.sprite_address = 1;
	p.background_address = 0;
	p.turn_nmi = 1;
	p.sprite_size = 0;    /* 8x8 sprites */
	p.name_table_address = 0;

	nesi_ppu_parameter_address(&p);

	/* Load palettes */
	nesi_copy_vram(bkg_pal, 16, nesi_back_palette);
	nesi_copy_vram(spr_pal, 16, nesi_sprite_palette);

	nesi_enable_graphics();

	/* Clear sprite OAM */
	nesi_fill_ram(0x0200, 0x0200, 0);

	/* Main game loop: title -> play -> game over -> repeat */
	while (1) {
		title_screen();
		play_game();
		game_over_screen();
	}
}
