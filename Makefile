include conf.mk

ifeq ($(ARCH), x86)
	ifeq ($(TYPE), legacy)
		# Source directories
		SRC_DIR := $(ARCH)/$(TYPE)/src
		# Tools
		ASM := nasm
		CC := clang
		LINKER := ld
		VM := qemu-system-i386
		CC_ARCH := i386
		CC_TARGET := i386-unknown-none
		ELF_ARCH := elf_i386
		ASM_OUTPUT := elf32
  	else ifeq ($(TYPE), uefi)
		SRC_DIR := $(ARCH)/$(TYPE)/src
	endif
endif

# Stage-specific directories and source files
STAGE1_NAME := stage1
STAGE1_DIR := $(SRC_DIR)/stage1
STAGE1_SRC := $(STAGE1_DIR)/start.asm

STAGE2_NAME := stage2
STAGE2_DIR := $(SRC_DIR)/stage2
STAGE2_SRC := $(STAGE2_DIR)/main.c

# Output files
BOOT_DIR := boot
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin
TARGET_IMG := $(BUILD_DIR)/$(TARGET).img
STAGE1_OBJ := $(BUILD_DIR)/$(STAGE1_NAME).o
STAGE1_BIN := $(BUILD_DIR)/$(STAGE1_NAME).bin
STAGE1_ELF := $(BUILD_DIR)/$(STAGE1_NAME).elf
STAGE2_OBJ := $(BUILD_DIR)/$(STAGE2_NAME).o
STAGE2_BIN := $(BUILD_DIR)/$(STAGE2_NAME).bin

# Compilation flags
ASM_FLAGS := -f $(ASM_OUTPUT) -i $(STAGE1_DIR) -w-label-orphan -w-pp-trailing -w-number-overflow
ASM_DEBUG_FLAGS := -f $(ASM_OUTPUT) -i $(STAGE1_DIR) -g -F dwarf
ASM_LD_FLAGS := -Ttext 0x7c00 -m $(ELF_ARCH) --oformat binary
ASM_LD_DEBUG_FLAGS := -Ttext 0x7C00 -m $(ELF_ARCH)
C_LD_FLAGS := -T $(STAGE2_DIR)/link.ld -m $(ELF_ARCH)
CC_FLAGS := -Wno-unused-command-line-argument -ffreestanding -march=$(CC_ARCH) -target $(CC_TARGET) -fno-builtin -nostdlib -z execstack -m32
CC_DEBUG_FLAGS := -g $(CC_FLAGS)

# QEMU run flags
VM_FLAGS := -drive format=raw,file=$(TARGET_IMG)
VM_DEBUG_FLAGS := -s -S -drive format=raw,file=$(TARGET_IMG)

# Phony targets
.PHONY: all debug clean stage1 stage2

# Default target to build the bootloader binary
all: $(TARGET_BIN)

# Rule to produce the final binary
$(TARGET_BIN): $(STAGE1_BIN) $(STAGE2_BIN)
	mkdir -p $(BUILD_DIR)
	dd if=$(STAGE1_BIN) of=$(TARGET_IMG) bs=512 count=1 conv=notrunc
	dd if=$(STAGE2_BIN) of=$(TARGET_IMG) bs=512 seek=1 count=2880 conv=notrunc
	cp $(TARGET_IMG) $(BOOT_DIR)/$(TARGET).img
	$(VM) $(VM_FLAGS)

# Rule to build the Stage 1 object file
stage1: $(STAGE1_BIN)

# Rule to build the Stage 1 object file
$(STAGE1_BIN): $(STAGE1_SRC)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $(STAGE1_ELF) $<
	$(LINKER) $(ASM_LD_FLAGS) -o $@ $(STAGE1_ELF)

# Rule to build the Stage 2 object file
stage2: $(STAGE2_BIN)

$(STAGE2_BIN): $(STAGE2_SRC)
	mkdir -p $(BUILD_DIR)
	$(CC) $(CC_FLAGS) -c -o $(STAGE2_OBJ) $<
	$(LINKER) $(C_LD_FLAGS) -o $@ $(STAGE2_OBJ)

# Debug target to build the ELF file for debugging
debug: $(STAGE1_OBJ)

$(STAGE1_ELF): $(STAGE1_SRC)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_DEBUG_FLAGS) -o $@ $<

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)
