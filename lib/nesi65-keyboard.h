/*
 * NESI65 - NES Keyboard Input (Family Basic / SUBOR)
 *
 * Support for the Family Basic keyboard and SUBOR-compatible keyboards
 * connected to the NES expansion port ($4016/$4017).
 *
 * Author:  Ambuj Varshney <ambuj123@gmail.com>
 * License: GNU GPL v3
 */

#ifndef NESI65_KEYBOARD_H
#define NESI65_KEYBOARD_H

#include "nesi65.h"

/* Family Basic keyboard layout map (128 bytes) - defined in nesi65-keyboard.c */
extern byte keyboard[];

/* SUBOR-compatible keyboard layout map - defined in nesi65-keyboard.c */
extern char keyboard_subor[];

void nesi_initkeyb(void);
byte nesi_readkeyb(void);
char nesi_readkeyb_family(void);
char nesi_readkeyb_subor(void);

#endif /* NESI65_KEYBOARD_H */
