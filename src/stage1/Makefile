# Variables
TARGET := boot_sector
BUILD_DIR := build
SRC_DIR := .
SRC := main
ASM := nasm
LINKER := ld

# Source files and output paths
ASM_FILES := $(SRC).asm
ASM_INPUT := $(patsubst %, $(SRC_DIR)/%, $(ASM_FILES))
ASM_OUTPUT_BIN := $(BUILD_DIR)/$(TARGET).bin
ASM_OUTPUT_O := $(BUILD_DIR)/$(TARGET).o
ASM_OUTPUT_ELF := $(BUILD_DIR)/$(TARGET).elf

# Compilation flags
ASM_FLAGS := -f bin -i $(SRC_DIR) -w+label-orphan
ASM_DEBUG_FLAGS := -f elf32 -g -F dwarf -i $(SRC_DIR)
LD_DEBUG_FLAGS := -Ttext 0x7C00 -m elf_i386

# Targets
.PHONY: stage1 debug clean

# Default target to build the bootloader binary
stage1: $(ASM_OUTPUT_O)

# Rule to produce the final binary
$(ASM_OUTPUT_O): $(ASM_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $(ASM_OUTPUT_O) $<

# Debug target to build the ELF file for debugging
debug: $(ASM_OUTPUT_ELF)

# Rule to produce the ELF file for debugging
$(ASM_OUTPUT_ELF): $(ASM_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_DEBUG_FLAGS) -o $(ASM_OUTPUT_O) $<
	$(LINKER) $(LD_DEBUG_FLAGS) -o $@ $(ASM_OUTPUT_O)

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)
