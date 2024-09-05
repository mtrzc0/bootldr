# Include configuration file
include conf.mk

# Define target-specific directories and files
TARGET := bootloader
TARGET_DIR := boot
BUILD_DIR := build
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin
TARGET_ELF := $(BUILD_DIR)/$(TARGET).elf
TARGET_IMG := $(BUILD_DIR)/$(TARGET).img

# Architecture-specific settings
ifeq ($(ARCH), x86)
    ifeq ($(TYPE), legacy)
        # Source directories
        SRC_DIR := $(ARCH)/$(TYPE)/src
        # Tools
        ASM := nasm
        CC := clang
        LINKER := ld
        VM := qemu-system-i386
        # I/O Formats
        CC_ARCH := i386
        CC_TARGET := i386-unknown-none
        ELF_ARCH := elf_i386
        ASM_OUTPUT := elf32
        ENTRY_POINT := 0x7c00
    else ifeq ($(TYPE), uefi)
       SRC_DIR := $(ARCH)/$(TYPE)/src
    endif
endif

# Stage-specific directories and source files
STAGE1_NAME := stage1
STAGE1_DIR := $(SRC_DIR)/stage1
STAGE1_SRC := $(STAGE1_DIR)/start.asm
STAGE1_BIN := $(BUILD_DIR)/$(STAGE1_NAME).bin
STAGE1_DEBUG_OBJ := $(STAGE1_DIR)/$(BUILD_DIR)/start.debug.o

STAGE2_NAME := stage2
STAGE2_DIR := $(SRC_DIR)/stage2
STAGE2_C_SRCS := $(wildcard $(STAGE2_DIR)/*.c)
STAGE2_ASM_SRCS := $(wildcard $(STAGE2_DIR)/*.asm)
STAGE2_BIN := $(BUILD_DIR)/$(STAGE2_NAME).bin
STAGE2_DEBUG_C_OBJ := $(patsubst $(STAGE2_DIR)/%.c, $(STAGE2_DIR)/$(BUILD_DIR)/%.debug.o, $(STAGE2_C_SRCS))
STAGE2_DEBUG_ASM_OBJ := $(patsubst $(STAGE2_DIR)/%.asm, $(STAGE2_DIR)/$(BUILD_DIR)/%.debug.o, $(STAGE2_ASM_SRCS))

# Compilation flags
LD_DEBUG_FLAGS := -Ttext $(ENTRY_POINT) -m $(ELF_ARCH)

# QEMU run flags
VM_FLAGS := -drive format=raw,file=$(TARGET_IMG)
VM_DEBUG_FLAGS := -s -S -drive format=raw,file=$(TARGET_IMG)

# Phony targets
.PHONY: all debug clean

# Silent
.SILENT: all

# Default target to build the bootloader binary
all: stages $(TARGET_BIN)

# Rule to produce the final file (.img)
$(TARGET_BIN): $(STAGE1_BIN) $(STAGE2_BIN)
	# Create build directory
	mkdir -p $(BUILD_DIR)
	# Create target image file of 1.44MB
	dd if=/dev/zero of=$(TARGET_IMG) bs=512 count=2880
	# Make fs
	# mkfs.fat -F 16 -n "os" $(TARGET_IMG)
	# Write Stage 1 binary to image
	dd if=$(STAGE1_BIN) of=$(TARGET_IMG) bs=512 count=1 conv=notrunc
	# Write Stage 2 binary to image
	dd if=$(STAGE2_BIN) of=$(TARGET_IMG) bs=512 seek=1 conv=notrunc
	# Copy image to target directory
	cp $(TARGET_IMG) $(TARGET_DIR)/$(TARGET).img

# Debug target to build the ELF file for debugging
debug: stages_debug $(TARGET_ELF)
	objdump -x $(TARGET_ELF)

# Run the bootloader in QEMU
run: $(TARGET_BIN)
	if [ $(DEBUG) = true ]; then \
		$(VM) $(VM_DEBUG_FLAGS); \
	elif [ $(DEBUG) = false ]; then \
		$(VM) $(VM_FLAGS); \
	else \
		echo "Invalid DEBUG value: $(DEBUG)"; \
		echo "Try DEBUG=true or DEBUG=false"; \
	fi

# Build the bootloader stages
stages:
	@$(MAKE) -j 8 -C $(STAGE1_DIR)
	@$(MAKE) -j 8 -C $(STAGE2_DIR)

# Build the bootloader stages in debug mode
stages_debug:
	@$(MAKE) -C $(STAGE1_DIR) debug
	@$(MAKE) -C $(STAGE2_DIR) debug

# Rule to build the ELF file for debugging
$(TARGET_ELF): $(STAGE1_DEBUG_OBJ) $(STAGE2_DEBUG_C_OBJ) $(STAGE2_DEBUG_ASM_OBJ)
	# Create build directory
	mkdir -p $(BUILD_DIR)
	# Link object files to create ELF
	$(LINKER) $(LD_DEBUG_FLAGS) -o $(TARGET_ELF) $^

# Clean up the build directory
clean:
	# Remove build directory
	rm -rf $(BUILD_DIR)
	# Clean up the build sub-directories
	@$(MAKE) -C $(STAGE1_DIR) clean
	@$(MAKE) -C $(STAGE2_DIR) clean