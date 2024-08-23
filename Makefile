TARGET := bootloader
BUILD_DIR := build
SRC_DIR := boot
ASM := nasm
FILES := core.asm
OUTPUT_PATH := $(BUILD_DIR)/$(TARGET)
INPUT_PATH := $(SRC_DIR)/$(FILES)

all: $(TARGET) 

$(TARGET): $(INPUT_PATH)
	mkdir -p $(BUILD_DIR)
	$(ASM) -i $(SRC_DIR) -o $(OUTPUT_PATH) $(INPUT_PATH)

debug: $(INPUT_PATH)
	mkdir -p $(BUILD_DIR)
	$(ASM) -g -i $(SRC_DIR) -o $(OUTPUT_PATH) $(INPUT_PATH)
clean:
	rm -rf $(BUILD_DIR)
