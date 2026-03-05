/*
 * NESI65 - NES Joystick/Controller Input (C stub)
 *
 * The actual nesi_get_joy() implementation is in nesi65-joystickc.s
 * (hand-optimized 6502 assembly for reading the controller port).
 * This file is kept as a compilation unit for the library build.
 *
 * Author:  Ambuj Varshney <ambuj_varshney(at)daiict.ac.in>
 * License: GNU GPL v3
 */

#include "nesi65-joystick.h"
