include conf.mk

# Target specific files
TARGET_DIR := boot
BUILD_DIR := build
TARGET_BIN := $(BUILD_DIR)/$(TARGET).bin
TARGET_ELF := $(BUILD_DIR)/$(TARGET).elf
TARGET_OBJ := $(BUILD_DIR)/$(TARGET).o
TARGET_IMG := $(BUILD_DIR)/$(TARGET).img

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
STAGE2_SRCS := $(wildcard $(STAGE2_DIR)/*.c)
STAGE2_BIN := $(BUILD_DIR)/$(STAGE2_NAME).bin
STAGE2_DEBUG_OBJ := $(patsubst $(STAGE2_DIR)/%.c, $(STAGE2_DIR)/$(BUILD_DIR)/%.debug.o, $(STAGE2_SRCS))

# Compilation flags
LD_DEBUG_FLAGS := -Ttext $(ENTRY_POINT) -m $(ELF_ARCH)

# QEMU run flags
VM_FLAGS := -drive format=raw,file=$(TARGET_IMG)
VM_DEBUG_FLAGS := -s -S -drive format=raw,file=$(TARGET_IMG)

# Phony targets
.PHONY: all debug clean

# Silent
.SILENT: all run

# Default target to build the bootloader binary
all: $(TARGET_BIN)

# Rule to produce the final file (.img)
# TODO: produce .iso file (kernel + bootloader)
$(TARGET_BIN): $(STAGE1_BIN) $(STAGE2_BIN)
	mkdir -p $(BUILD_DIR)
	dd if=$(STAGE1_BIN) of=$(TARGET_IMG) bs=512 count=1 conv=notrunc
	dd if=$(STAGE2_BIN) of=$(TARGET_IMG) bs=512 seek=1 count=2880 conv=notrunc
	# TODO: mkfs
	cp $(TARGET_IMG) $(TARGET_DIR)/$(TARGET).img

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

# Debug target to build the ELF file for debugging
debug: $(TARGET_ELF)

# Rule to build the ELF file for debugging
# TODO: Check if required objs were build
$(TARGET_ELF): $(STAGE1_DEBUG_OBJ) $(STAGE2_DEBUG_OBJS)
	mkdir -p $(BUILD_DIR)
	$(LINKER) $(LD_DEBUG_FLAGS) -o $(TARGET_ELF) $(STAGE1_DEBUG_OBJ) $(STAGE2_DEBUG_OBJ)

# Clean up the build directory
clean:
	rm -rf $(BUILD_DIR)