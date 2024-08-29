# Variables
TARGET := bootloader
TARGET_STAGE1 := boot
TARGET_STAGE2 := loader
BUILD_DIR := build
SRC_DIR := src
STAGE1_DIR_NAME := stage1
STAGE1_SRC := main
STAGE2_DIR_NAME := stage2
STAGE2_SRC := main
ASM := nasm
CC := gcc
LINKER := ld

# Source and output files
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin
STAGE1_INPUT_O := $(BUILD_DIR)/$(TARGET_STAGE1).o
STAGE1_SRC_INPUT := $(SRC_DIR)/$(STAGE1_DIR_NAME)/$(STAGE1_SRC).asm
STAGE1_DEBUG_ELF := $(BUILD_DIR)/$(TARGET_STAGE1).elf
STAGE2_INPUT_O := $(BUILD_DIR)/$(TARGET_STAGE2).o
STAGE2_SRC_INPUT := $(SRC_DIR)/$(STAGE2_DIR_NAME)/$(STAGE2_SRC).c

# Compilation flags
ASM_FLAGS := -f elf32 -i $(SRC_DIR)/$(STAGE1_DIR_NAME) -w+label-orphan -w+pp-trailing
ASM_DEBUG_FLAGS := -f elf32 -i $(SRC_DIR)/$(STAGE1_DIR_NAME) -g -F dwarf
LD_FLAGS := -T $(SRC_DIR)/$(STAGE1_DIR_NAME)/linker.ld -m elf_i386
LD_DEBUG_FLAGS := -Ttext 0x7C00 -m elf_i386

# Run flags
VM := qemu-system-i386
VM_FLAGS := -drive format=raw,file=$(TARGET_BIN)
VM_DEBUG_FLAGS := -s -S -drive format=raw,file=$(TARGET_BIN)

# Targets
.PHONY: all debug clean stage1

# Default target to build the bootloader binary
all: $(TARGET_BIN)

# Rule to produce the final binary
$(TARGET_BIN): stage1
	mkdir -p $(BUILD_DIR)
	$(LINKER) $(LD_FLAGS) -o $@ $(STAGE1_INPUT_O)
	$(VM) $(VM_FLAGS)

# Default target to build the stage1
stage1: $(STAGE1_INPUT_O)

# Rule to produce assembly object file
$(STAGE1_INPUT_O): $(STAGE1_SRC_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $@ $<

# Default target to build the stage2
stage2: $(STAGE2_INPUT_O)

# Rule to produce C object file
$(STAGE2_INPUT_O): $(STAGE2_SRC_INPUT)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) -o $@ $<

# Debug target to build the ELF file for debugging
debug: $(STAGE1_DEBUG_ELF)

# Rule to produce the ELF file for debugging
$(STAGE1_DEBUG_ELF): $(STAGE1_INPUT_O)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_DEBUG_FLAGS) -o $(ASM_OUTPUT_O) $<
	$(LINKER) $(LD_DEBUG_FLAGS) -o $@ $(ASM_OUTPUT_O)

# Rule to produce the ELF file for debugging
$(STAGE2_DEBUG_ELF): $(STAGE2_INPUT_O)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_DEBUG_FLAGS) -o $@ $<

# Clean up the build directory
clean:
	rm -r $(BUILD_DIR)
