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
DEBUG_ELF_OUTPUT := $(BUILD_DIR)/$(TARGET).elf

# Compilation flags
ASM_FLAGS := -i $(SRC_DIR)
ASM_DEBUG_FLAGS := -f elf32 -g -F dwarf -i $(SRC_DIR)
LDFLAGS := -T $(SRC_DIR)/linker.ld -m elf_i386

# Targets
.PHONY: all debug clean

all: $(ASM_OUTPUT)

$(ASM_OUTPUT): $(ASM_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_FLAGS) -o $@ $<

debug: $(DEBUG_ELF_OUTPUT)

$(DEBUG_ELF_OUTPUT): $(ASM_INPUT)
	mkdir -p $(BUILD_DIR)
	$(ASM) $(ASM_DEBUG_FLAGS) -o $(ASM_DEBUG_OUTPUT) $<
	$(LINKER) $(LDFLAGS) -o $@ $(ASM_DEBUG_OUTPUT)

clean:
	rm -rf $(BUILD_DIR)
