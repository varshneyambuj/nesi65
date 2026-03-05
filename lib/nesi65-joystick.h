/*
 * NESI65 - NES Joystick/Controller Input
 *
 * Read NES controller state and test individual button presses.
 * The actual read routine is in nesi65-joystickc.s (hand-optimized 6502 asm).
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_JOYSTICK_H
#define NESI65_JOYSTICK_H

#include "nesi65.h"

/* Controller port addresses */
#define nesi_JOYPAD_1  0x4016
#define nesi_JOYPAD_2  0x4017

/* Button bitmasks (after reading via nesi_get_joy) */
#define nesi_A       0x80
#define nesi_B       0x40
#define nesi_SELECT  0x20
#define nesi_START   0x10
#define nesi_UP      0x08
#define nesi_DOWN    0x04
#define nesi_LEFT    0x02
#define nesi_RIGHT   0x01

/* Button state test macros */
#define nesi_A_Down(joy_state)      (joy_state & nesi_A)
#define nesi_B_Down(joy_state)      (joy_state & nesi_B)
#define nesi_SELECT_Down(joy_state) (joy_state & nesi_SELECT)
#define nesi_START_Down(joy_state)  (joy_state & nesi_START)
#define nesi_UP_Down(joy_state)     (joy_state & nesi_UP)
#define nesi_DOWN_Down(joy_state)   (joy_state & nesi_DOWN)
#define nesi_LEFT_Down(joy_state)   (joy_state & nesi_LEFT)
#define nesi_RIGHT_Down(joy_state)  (joy_state & nesi_RIGHT)

/* Read controller state (implemented in nesi65-joystickc.s) */
byte nesi_get_joy(word joypad);

#endif /* NESI65_JOYSTICK_H */
