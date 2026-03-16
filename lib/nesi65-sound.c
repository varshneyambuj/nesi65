/*
 * NESI65 - APU Sound Control
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#include "nesi65-sound.h"

/*
 * Initialize the APU to a silent, known state.
 * Disables all channels, silences registers, and sets 4-step frame counter.
 */
void nesi_sound_init(void)
{
	/* Disable all channels */
	a_addr(APU_STATUS) = 0x00;

	/* Silence pulse 1 */
	a_addr(APU_PULSE1_CTRL) = 0x30;
	a_addr(APU_PULSE1_SWEEP) = 0x08;
	a_addr(APU_PULSE1_TIMER_L) = 0x00;
	a_addr(APU_PULSE1_TIMER_H) = 0x00;

	/* Silence pulse 2 */
	a_addr(APU_PULSE2_CTRL) = 0x30;
	a_addr(APU_PULSE2_SWEEP) = 0x08;
	a_addr(APU_PULSE2_TIMER_L) = 0x00;
	a_addr(APU_PULSE2_TIMER_H) = 0x00;

	/* Silence triangle */
	a_addr(APU_TRI_CTRL) = 0x80;
	a_addr(APU_TRI_TIMER_L) = 0x00;
	a_addr(APU_TRI_TIMER_H) = 0x00;

	/* Silence noise */
	a_addr(APU_NOISE_CTRL) = 0x30;
	a_addr(APU_NOISE_PERIOD) = 0x00;
	a_addr(APU_NOISE_LENGTH) = 0x00;

	/* 4-step frame counter, disable IRQ */
	a_addr(APU_FRAME_CTR) = 0x40;
}

/*
 * Enable channels via status register.
 * Bits: 0=Pulse1, 1=Pulse2, 2=Triangle, 3=Noise, 4=DMC
 */
void nesi_sound_enable(byte channels)
{
	a_addr(APU_STATUS) = channels;
}

/* Disable all sound channels */
void nesi_sound_disable(void)
{
	a_addr(APU_STATUS) = 0x00;
}

void nesi_pulse1_play(byte duty, byte volume, word timer, byte length)
{
	a_addr(APU_PULSE1_CTRL) = duty | SND_LOOP | SND_CONST | (volume & 0x0F);
	a_addr(APU_PULSE1_SWEEP) = 0x08;
	a_addr(APU_PULSE1_TIMER_L) = (byte)(timer & 0xFF);
	a_addr(APU_PULSE1_TIMER_H) = (length << 3) | (byte)((timer >> 8) & 0x07);
}

void nesi_pulse2_play(byte duty, byte volume, word timer, byte length)
{
	a_addr(APU_PULSE2_CTRL) = duty | SND_LOOP | SND_CONST | (volume & 0x0F);
	a_addr(APU_PULSE2_SWEEP) = 0x08;
	a_addr(APU_PULSE2_TIMER_L) = (byte)(timer & 0xFF);
	a_addr(APU_PULSE2_TIMER_H) = (length << 3) | (byte)((timer >> 8) & 0x07);
}

void nesi_triangle_play(word timer, byte length, byte linear)
{
	a_addr(APU_TRI_CTRL) = 0x80 | (linear & 0x7F);
	a_addr(APU_TRI_TIMER_L) = (byte)(timer & 0xFF);
	a_addr(APU_TRI_TIMER_H) = (length << 3) | (byte)((timer >> 8) & 0x07);
}

void nesi_noise_play(byte volume, byte period, byte mode, byte length)
{
	a_addr(APU_NOISE_CTRL) = SND_LOOP | SND_CONST | (volume & 0x0F);
	a_addr(APU_NOISE_PERIOD) = (mode << 7) | (period & 0x0F);
	a_addr(APU_NOISE_LENGTH) = (length << 3);
}

void nesi_pulse1_stop(void)
{
	a_addr(APU_PULSE1_CTRL) = 0x30;
}

void nesi_pulse2_stop(void)
{
	a_addr(APU_PULSE2_CTRL) = 0x30;
}

void nesi_triangle_stop(void)
{
	a_addr(APU_TRI_CTRL) = 0x80;
}

void nesi_noise_stop(void)
{
	a_addr(APU_NOISE_CTRL) = 0x30;
}

/* Short laser/shoot sound: high-pitched pulse sweep down */
void nesi_sfx_shoot(void)
{
	a_addr(APU_STATUS) = a_addr(APU_STATUS) | 0x02;
	nesi_pulse2_play(DUTY_25, 8, 0x040, 0x01);
}

/* Hit/explosion: noise burst */
void nesi_sfx_hit(void)
{
	a_addr(APU_STATUS) = a_addr(APU_STATUS) | 0x08;
	nesi_noise_play(12, 6, 0, 0x02);
}

/* Pickup/collect: ascending two-note pulse */
void nesi_sfx_pickup(void)
{
	a_addr(APU_STATUS) = a_addr(APU_STATUS) | 0x01;
	nesi_pulse1_play(DUTY_50, 10, 0x0D0, 0x02);
}

/* Death: low descending noise */
void nesi_sfx_death(void)
{
	a_addr(APU_STATUS) = a_addr(APU_STATUS) | 0x08;
	nesi_noise_play(15, 8, 0, 0x06);
}
