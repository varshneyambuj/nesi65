# NESI65 - NES Game Development Library & Games
# Build system for cc65 (6502 C compiler)
#
# Targets:
#   make lib            - Build the nesi65 library object files
#   make number_muncher - Build Number Muncher game ROM
#   make hanuman_typing - Build Hanuman Typing Warrior game ROM
#   make space_dodger   - Build Space Dodger game ROM
#   make all            - Build everything
#   make clean          - Remove intermediate build artifacts
#   make distclean      - Remove all generated files including ROMs

CC      = cc65
AS      = ca65
LD      = ld65
CFLAGS  = -t nes
ASFLAGS = -t nes

# Directories
LIB_DIR     = lib
GAME_DIR    = games
CONFIG_DIR  = config
RUNTIME_DIR = runtime
ROM_DIR     = roms
BUILD_DIR   = build

# Linker config and runtime objects
LD_CFG     = $(CONFIG_DIR)/nes_chr-ram.cfg
CRT0       = $(RUNTIME_DIR)/crt0_chr-ram.o
WAITVBLANK = $(RUNTIME_DIR)/waitvblank.o
NES_LIB    = /usr/share/cc65/lib/none.lib

# Library source files
LIB_C_SRCS = $(LIB_DIR)/nesi65-init.c \
             $(LIB_DIR)/nesi65-joystick.c \
             $(LIB_DIR)/nesi65-keyboard.c \
             $(LIB_DIR)/nesi65-memory.c \
             $(LIB_DIR)/nesi65-misc.c \
             $(LIB_DIR)/nesi65-sound.c \
             $(LIB_DIR)/nesi65-collision.c \
             $(LIB_DIR)/nesi65-text.c \
             $(LIB_DIR)/nesi65-math.c

LIB_ASM_SRCS = $(LIB_DIR)/nesi65-joystickc.s \
               $(LIB_DIR)/nesi65-memoryc.s

# Library object files
LIB_C_OBJS   = $(patsubst $(LIB_DIR)/%.c,$(BUILD_DIR)/%.o,$(LIB_C_SRCS))
LIB_ASM_OBJS = $(patsubst $(LIB_DIR)/%.s,$(BUILD_DIR)/%.o,$(LIB_ASM_SRCS))
LIB_OBJS     = $(LIB_C_OBJS) $(LIB_ASM_OBJS)

# Include path for library headers
INCLUDES = -I $(LIB_DIR)

# ============================================================
# Default target
# ============================================================

.PHONY: all lib number_muncher hanuman_typing space_dodger clean distclean

all: lib number_muncher space_dodger

lib: $(LIB_OBJS)

# ============================================================
# Library: compile C -> assembly -> object
# ============================================================

$(BUILD_DIR)/%.o: $(LIB_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(BUILD_DIR)/$*.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/$*.s

# Library: assemble hand-written .s files
$(BUILD_DIR)/%.o: $(LIB_DIR)/%.s | $(BUILD_DIR)
	$(AS) $(ASFLAGS) -o $@ $<

# ============================================================
# Number Muncher game
# ============================================================

NM_DIR  = $(GAME_DIR)/number_muncher
NM_SRC  = $(NM_DIR)/number_muncher.c
NM_OBJ  = $(BUILD_DIR)/number_muncher.o
NM_ROM  = $(ROM_DIR)/Number_Muncher.nes

number_muncher: $(NM_ROM)

$(NM_OBJ): $(NM_SRC) $(NM_DIR)/number_muncher.h $(NM_DIR)/number_muncher_sprite.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I $(NM_DIR) -o $(BUILD_DIR)/number_muncher.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/number_muncher.s

$(NM_ROM): $(NM_OBJ) $(LIB_OBJS) | $(ROM_DIR)
	$(LD) -C $(LD_CFG) $(WAITVBLANK) $(CRT0) $(NM_OBJ) $(LIB_OBJS) $(NES_LIB) -o $@

# ============================================================
# Hanuman Typing Warrior game
# (Note: requires name_table0.h data file in the game directory)
# ============================================================

HT_DIR  = $(GAME_DIR)/hanuman_typing
HT_SRC  = $(HT_DIR)/hanuman_typing.c
HT_OBJ  = $(BUILD_DIR)/hanuman_typing.o
HT_ROM  = $(ROM_DIR)/Hanuman_Typing.nes

hanuman_typing: $(HT_ROM)

$(HT_OBJ): $(HT_SRC) $(HT_DIR)/hanuman_typing.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I $(HT_DIR) -o $(BUILD_DIR)/hanuman_typing.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/hanuman_typing.s

$(HT_ROM): $(HT_OBJ) $(LIB_OBJS) | $(ROM_DIR)
	$(LD) -C $(LD_CFG) $(WAITVBLANK) $(CRT0) $(HT_OBJ) $(LIB_OBJS) $(NES_LIB) -o $@

# ============================================================
# Space Dodger game
# ============================================================

SD_DIR  = $(GAME_DIR)/space_dodger
SD_SRC  = $(SD_DIR)/space_dodger.c
SD_OBJ  = $(BUILD_DIR)/space_dodger.o
SD_ROM  = $(ROM_DIR)/Space_Dodger.nes

space_dodger: $(SD_ROM)

$(SD_OBJ): $(SD_SRC) $(SD_DIR)/space_dodger.h | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INCLUDES) -I $(SD_DIR) -o $(BUILD_DIR)/space_dodger.s $<
	$(AS) $(ASFLAGS) -o $@ $(BUILD_DIR)/space_dodger.s

$(SD_ROM): $(SD_OBJ) $(LIB_OBJS) | $(ROM_DIR)
	$(LD) -C $(LD_CFG) $(WAITVBLANK) $(CRT0) $(SD_OBJ) $(LIB_OBJS) $(NES_LIB) -o $@

# ============================================================
# Directory creation
# ============================================================

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(ROM_DIR):
	mkdir -p $(ROM_DIR)

# ============================================================
# Cleanup
# ============================================================

clean:
	rm -rf $(BUILD_DIR)

distclean: clean
	rm -f $(ROM_DIR)/Number_Muncher.nes $(ROM_DIR)/Hanuman_Typing.nes $(ROM_DIR)/Space_Dodger.nes
