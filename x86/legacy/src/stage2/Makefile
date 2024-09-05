# Define the build directory
BUILD_DIR := build

# Define the C compiler and linker
ASM := nasm
CC := clang
LINKER := ld

# Define the ELF architecture and target architecture
ELF_ARCH := elf_i386
CC_ARCH := i386
CC_TARGET := i386-unknown-none

# Define the compiler flags
CC_FLAGS := -Wno-unused-command-line-argument -ffreestanding -march=$(CC_ARCH) -target $(CC_TARGET) -fno-builtin -nostdlib -z execstack -m32
CC_DEBUG_FLAGS := -g $(CC_FLAGS)

# Define the assembler flags
ASM_FLAGS := -f elf32
ASM_DEBUG_FLAGS := -g $(ASM_FLAGS) -F dwarf

# Define the linker flags
C_LD_FLAGS := -T link.ld -m $(ELF_ARCH)

# Find all .c files in the current directory
C_SRC_FILES := $(wildcard *.c)
ASM_SRC_FILES += $(wildcard *.asm)

# Generate object file names from source files
C_OBJ_FILES := $(patsubst %.c, $(BUILD_DIR)/%.o, $(C_SRC_FILES))
C_OBJ_DEBUG_FILES := $(patsubst %.c, $(BUILD_DIR)/%.debug.o, $(C_SRC_FILES))
ASM_OBJ_FILES := $(patsubst %.asm, $(BUILD_DIR)/%.o, $(ASM_SRC_FILES))
ASM_OBJ_DEBUG_FILES := $(patsubst %.asm, $(BUILD_DIR)/%.debug.o, $(ASM_SRC_FILES))

# Define the target directory and name
TARGET_DIR := ../../../../build
TARGET_NAME := stage2

# Define the target binary file
TARGET_BIN := $(TARGET_DIR)/$(TARGET_NAME).bin

# Define phony targets
.PHONY: all debug

# Default target to build all object files and the binary
all: $(C_OBJ_FILES) $(ASM_OBJ_FILES) $(TARGET_BIN)

# Rule to compile .asm files into .o files
$(BUILD_DIR)/%.o: %.asm
	# Create the build directory if it doesn't exist
	mkdir -p $(BUILD_DIR)
	# Compile the source file into an object file
	$(ASM) $(ASM_FLAGS) $< -o $@

# Rule to compile .c files into .o files
$(BUILD_DIR)/%.o: %.c
	# Create the build directory if it doesn't exist
	mkdir -p $(BUILD_DIR)
	# Compile the source file into an object file
	$(CC) $(CC_FLAGS) -c $< -o $@

# Rule to build the Stage 2 binary
$(TARGET_BIN): $(C_OBJ_FILES) $(ASM_OBJ_FILES)
	# Create the target directory if it doesn't exist
	mkdir -p $(TARGET_DIR)
	# Link the object files into a binary
	$(LINKER) $(C_LD_FLAGS) -o $@ $^

# Debug target to build debug object files
debug: $(C_OBJ_DEBUG_FILES) $(ASM_OBJ_DEBUG_FILES)

# Rule to compile .asm files into .o files
$(BUILD_DIR)/%.debug.o: %.asm
	# Create the build directory if it doesn't exist
	mkdir -p $(BUILD_DIR)
	# Compile the source file into an object file
	$(ASM) $(ASM_DEBUG_FLAGS) $< -o $@

# Rule to compile .c files into debug .o files
$(BUILD_DIR)/%.debug.o: %.c
	# Create the build directory if it doesn't exist
	mkdir -p $(BUILD_DIR)
	# Compile the source file into a debug object file
	$(CC) $(CC_DEBUG_FLAGS) -c $< -o $@

# Clean up the build directory
clean:
	# Remove the build directory and its contents
	rm -rf $(BUILD_DIR)