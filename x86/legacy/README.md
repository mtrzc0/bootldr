## Overview
This directory contain an x86 legacy BIOS bootloader that initializes the system in 16-bit Real Mode, transitions to 32-bit Protected Mode, and loads a simple kernel. The bootloader supports FAT16 and Multiboot specifications.

## Project Structure
- `src/stage1/`: Contains the assembly code for the bootloader.
- `src/stage2/`: Contains the C code for the kernel.
- `build/`: Directory where the compiled binaries are stored.
- `boot/`: Directory where the final bootable image is stored.

## Build Instructions
1. **Install Dependencies**:
    - `nasm`: Netwide Assembler for assembling the bootloader.
    - `clang`: C compiler for compiling the kernel.
    - `ld`: GNU linker for linking the object files.
    - `qemu`: Emulator for testing the bootloader.

2. **Build the Bootloader**:
   ```sh
   make all
   ```

3. **Clean the Build Directory**:
   ```sh
   make clean
   ```

## Running the Bootloader
To run the bootloader using QEMU:
```sh
make all
```
This command will build the bootloader and automatically run it in QEMU.

## Files
- `Makefile`: Contains the build rules and commands.
- `src/stage1/start.asm`: Assembly code for the bootloader.
- `src/stage2/main.c`: C code for the kernel.
- `src/stage1/linker.ld`: Linker script for the bootloader.
- `src/stage2/link.ld`: Linker script for the kernel.

## License
This project is licensed under the MIT License. See the `LICENSE` file for details.