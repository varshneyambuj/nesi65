/*
 * NESI65 - APU Sound Control
 *
 * Functions to control the NES Audio Processing Unit (APU).
 * Supports Pulse 1, Pulse 2, Triangle, and Noise channels.
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_SOUND_H
#define NESI65_SOUND_H

#include "nesi65.h"

/* APU register addresses */
#define APU_PULSE1_CTRL    0x4000  /* Duty, envelope, volume */
#define APU_PULSE1_SWEEP   0x4001  /* Sweep unit */
#define APU_PULSE1_TIMER_L 0x4002  /* Timer low 8 bits */
#define APU_PULSE1_TIMER_H 0x4003  /* Length counter + timer high 3 bits */

#define APU_PULSE2_CTRL    0x4004
#define APU_PULSE2_SWEEP   0x4005
#define APU_PULSE2_TIMER_L 0x4006
#define APU_PULSE2_TIMER_H 0x4007

#define APU_TRI_CTRL       0x4008  /* Linear counter */
#define APU_TRI_TIMER_L    0x400A  /* Timer low 8 bits */
#define APU_TRI_TIMER_H    0x400B  /* Length counter + timer high 3 bits */

#define APU_NOISE_CTRL     0x400C  /* Envelope, volume */
#define APU_NOISE_PERIOD   0x400E  /* Mode + period index */
#define APU_NOISE_LENGTH   0x400F  /* Length counter load */

#define APU_STATUS         0x4015  /* Channel enable/disable */
#define APU_FRAME_CTR      0x4017  /* Frame counter mode */

/* Duty cycle values for pulse channels */
#define DUTY_12_5   0x00  /* 12.5% duty (thin, sharp) */
#define DUTY_25     0x40  /* 25% duty (standard square) */
#define DUTY_50     0x80  /* 50% duty (full square, hollow) */
#define DUTY_75     0xC0  /* 75% duty (inverted 25%) */

/* Length counter halt + constant volume flag */
#define SND_LOOP    0x20  /* Halt length counter (loop envelope) */
#define SND_CONST   0x10  /* Use constant volume (not envelope) */

/* Initialize APU to a known state */
void nesi_sound_init(void);

/* Enable/disable individual channels via APU status register */
void nesi_sound_enable(byte channels);
void nesi_sound_disable(void);

/*
 * Play a tone on pulse channel 1.
 *   duty:   DUTY_12_5, DUTY_25, DUTY_50, or DUTY_75
 *   volume: 0-15
 *   timer:  11-bit timer period (lower = higher pitch)
 *   length: length counter load value (0-31, upper 5 bits)
 */
void nesi_pulse1_play(byte duty, byte volume, word timer, byte length);

/* Play a tone on pulse channel 2 */
void nesi_pulse2_play(byte duty, byte volume, word timer, byte length);

/* Play a tone on the triangle channel (no volume control) */
void nesi_triangle_play(word timer, byte length, byte linear);

/* Play noise channel */
void nesi_noise_play(byte volume, byte period, byte mode, byte length);

/* Silence individual channels */
void nesi_pulse1_stop(void);
void nesi_pulse2_stop(void);
void nesi_triangle_stop(void);
void nesi_noise_stop(void);

/*
 * Predefined sound effects.
 * These play short sequences useful for games.
 */
void nesi_sfx_shoot(void);
void nesi_sfx_hit(void);
void nesi_sfx_pickup(void);
void nesi_sfx_death(void);

#endif /* NESI65_SOUND_H */
