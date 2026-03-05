# NESI65

A C library for developing NES games using the [cc65](https://cc65.github.io/cc65/) 6502 compiler toolchain.

## Project Structure

```
nesi65/
├── lib/                    # NESI65 library
│   ├── nesi65.h            # Core types, macros, PPU constants
│   ├── nesi65-init.*       # PPU initialization and graphics control
│   ├── nesi65-joystick.*   # NES controller input
│   ├── nesi65-joystickc.s  # Controller read (hand-optimized 6502 asm)
│   ├── nesi65-keyboard.*   # Family Basic / SUBOR keyboard input
│   ├── nesi65-memory.*     # VRAM and memory manipulation
│   ├── nesi65-memoryc.s    # Memory routines (hand-optimized 6502 asm)
│   └── nesi65-misc.*       # PRNG and delay utilities
├── games/
│   ├── number_muncher/     # Math education game (multiples/factors)
│   └── hanuman_typing/     # Typing game with keyboard input
├── config/
│   └── nes_chr-ram.cfg     # NES memory layout for ld65 linker
├── runtime/                # Pre-compiled runtime files
│   ├── crt0_chr-ram.o      # C runtime startup for CHR-RAM config
│   ├── waitvblank.o        # VBlank wait routine
│   └── nes.lib             # cc65 NES runtime library
├── roms/                   # Compiled game ROMs
│   ├── Number_Muncher.nes
│   └── Hanuman_Typing.nes
├── data/                   # Graphics data
│   ├── output.chr          # CHR tile data (8KB)
│   └── pic0.nam            # NES nametable binary
└── Makefile
```

## Building

Requires the [cc65](https://cc65.github.io/cc65/) toolchain (`cc65`, `ca65`, `ld65`) installed and in your PATH.

```bash
# Build the library and Number Muncher game
make all

# Build a specific game
make number_muncher
make hanuman_typing

# Clean build artifacts
make clean
```

## Library Modules

| Module | Description |
|--------|-------------|
| `nesi65.h` | Core types (`byte`, `word`), binary literal macros, PPU address constants, `nesi_sprite` struct |
| `nesi65-init` | PPU initialization, VBlank wait, enable/disable sprite and background rendering |
| `nesi65-joystick` | Read NES controller buttons (A, B, Select, Start, D-pad) |
| `nesi65-keyboard` | Read Family Basic and SUBOR keyboards via expansion port |
| `nesi65-memory` | VRAM write, tile copy, sprite DMA, RAM fill operations |
| `nesi65-misc` | Pseudo-random number generator, busy-wait delay |

## Games

### Number Muncher
A math education game. The player moves a muncher on a 5x6 grid and eats numbers that match the current rule (multiples or factors of a target number). Troggle enemies spawn and move across the grid - avoid them or lose a life.

### Hanuman Typing Warrior
A typing game using the Family Basic keyboard. Words appear on screen and the player types them character by character. Each correct keystroke advances Hanuman forward. After completing a word, Hanuman breaks a piece of the rock tower.

## Author

Ambuj Varshney - ambuj123@gmail.com

## License

GNU GPL v3 - see [LICENSE](LICENSE) for details.
