# Variables
TARGET := bootloader
BUILD_DIR := build
SRC_DIR := boot
ASM := nasm
LINKER := ld

# Source files and output paths
ASM_FILES := core.asm
ASM_INPUT := $(patsubst %, $(SRC_DIR)/%, $(ASM_FILES))
ASM_OUTPUT := $(BUILD_DIR)/$(TARGET).bin
ASM_DEBUG_OUTPUT := $(BUILD_DIR)/$(TARGET).o

# Compilation flags
ASM_FLAGS := -i $(SRC_DIR)
ASM_DEBUG_FLAGS := -f elf32 -g -F dwarf -i $(SRC_DIR)
LDFLAGS := -Ttext 0x7c00 -m elf_i386 --oformat binary

# Targets
.PHONY: all debug clean

all: $(ASM_OUTPUT)

$(ASM_OUTPUT): $(ASM_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $@ $<

debug: $(BUILD_DIR)/$(TARGET).dbg

$(BUILD_DIR)/$(TARGET).dbg: $(ASM_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_DEBUG_FLAGS) -o $(ASM_DEBUG_OUTPUT) $<
	$(LINKER) $(LDFLAGS) -o $(BUILD_DIR)/$(TARGET).bin $(ASM_DEBUG_OUTPUT)

clean:
	rm -rf $(BUILD_DIR)

