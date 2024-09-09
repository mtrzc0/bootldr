# Define the build directory
BUILD_DIR := build

# Define the assembler and linker
ASM := nasm
LINKER := ld

# Define the ELF architecture and output format
ELF_ARCH := elf_i386
ASM_OUTPUT := elf32

# Define the entry point for the linker
ENTRY_POINT := 0x7c00

# Define the assembler flags
ASM_FLAGS := -f $(ASM_OUTPUT) -w-label-orphan -w-pp-trailing -w-number-overflow
ASM_DEBUG_FLAGS := -f $(ASM_OUTPUT) -g -F dwarf

# Define the linker flags
ASM_LD_FLAGS := -Ttext $(ENTRY_POINT) -m $(ELF_ARCH) --oformat binary

# Find all .asm files in the current directory
SRC_FILES := $(wildcard boot.asm)

# Generate object file names from source files
OBJ_FILES := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(SRC_FILES))
OBJ_DEBUG_FILES := $(patsubst %.asm, $(BUILD_DIR)/%.debug.o, $(SRC_FILES))

# Define the target directory and name
TARGET_DIR := ../../../../build
TARGET_NAME := stage1

# Define the target binary file
TARGET_BIN := $(TARGET_DIR)/$(TARGET_NAME).bin

# Define phony targets
.PHONY: all debug

# Default target to build all object files and the binary
all: $(OBJ_FILES) $(TARGET_BIN)

# Rule to compile .asm files into .o files
$(BUILD_DIR)/%.o: %.asm
	# Create the build directory if it doesn't exist
	mkdir -p $(BUILD_DIR)
	# Assemble the source file into an object file
	$(ASM) $(ASM_FLAGS) -o $@ $<

# Rule to build the Stage 1 binary
$(TARGET_BIN): $(OBJ_FILES)
	# Create the target directory if it doesn't exist
	mkdir -p $(TARGET_DIR)
	# Link the object files into a binary
	$(LINKER) $(ASM_LD_FLAGS) -o $@ $<

# Debug target to build debug object files
debug: $(OBJ_DEBUG_FILES)

# Rule to compile .asm files into debug .o files
$(BUILD_DIR)/%.debug.o: %.asm
	# Create the build directory if it doesn't exist
	mkdir -p $(BUILD_DIR)
	# Assemble the source file into a debug object file
	$(ASM) $(ASM_DEBUG_FLAGS) -o $@ $<

# Clean up the build directory
clean:
	# Remove the build directory and its contents
	rm -rf $(BUILD_DIR)