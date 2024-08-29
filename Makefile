# Target names
TARGET := bootloader
BUILD_DIR := build
SRC_DIR := src

# Stage-specific directories and source files
STAGE1_NAME := boot
STAGE1_DIR := $(SRC_DIR)/stage1
STAGE1_SRC := $(STAGE1_DIR)/main.asm

STAGE2_NAME := loader
STAGE2_DIR := $(SRC_DIR)/stage2
STAGE2_SRC := $(STAGE2_DIR)/main.c

# Tools
ASM := nasm
CC := gcc
LINKER := ld
VM := qemu-system-i386

# Output files
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin
STAGE1_OBJ := $(BUILD_DIR)/$(STAGE1_NAME).o
STAGE1_ELF := $(BUILD_DIR)/$(STAGE1_NAME).elf
STAGE2_OBJ := $(BUILD_DIR)/$(STAGE2_NAME).o

# Compilation flags
ASM_FLAGS := -f elf32 -i $(STAGE1_DIR) -w+label-orphan -w+pp-trailing
ASM_DEBUG_FLAGS := -f elf32 -i $(STAGE1_DIR) -g -F dwarf
CC_FLAGS := -m32 -ffreestanding
CC_DEBUG_FLAGS := -m32 -g -ffreestanding
LD_FLAGS := -T $(STAGE1_DIR)/linker.ld -m elf_i386
LD_DEBUG_FLAGS := -Ttext 0x7C00 -m elf_i386

# QEMU run flags
VM_FLAGS := -drive format=raw,file=$(TARGET_BIN)
VM_DEBUG_FLAGS := -s -S -drive format=raw,file=$(TARGET_BIN)

# Phony targets
.PHONY: all debug clean stage1 stage2

# Default target to build the bootloader binary
all: $(TARGET_BIN)

# Rule to produce the final binary
$(TARGET_BIN): $(STAGE1_OBJ)
	mkdir -p $(BUILD_DIR)
	$(LINKER) $(LD_FLAGS) -o $@ $<
	$(VM) $(VM_FLAGS)

# Rule to build the Stage 1 object file
$(STAGE1_OBJ): $(STAGE1_SRC)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $@ $<

# Rule to build the Stage 2 object file
stage2: $(STAGE2_OBJ)

$(STAGE2_OBJ): $(STAGE2_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) -o $@ $<

# Debug target to build the ELF file for debugging
debug: $(STAGE1_ELF)

$(STAGE1_ELF): $(STAGE1_OBJ)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_DEBUG_FLAGS) -o $(STAGE1_OBJ) $(STAGE1_SRC)
	$(LINKER) $(LD_DEBUG_FLAGS) -o $@ $(STAGE1_OBJ)

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)
