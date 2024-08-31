# Bootloader Project

This project focuses on understanding how bootloaders work. It includes various stages and components to load an operating system.

## Core Components

- **OS Loader**: Loads the operating system (Unix family).
- **GUI**: Graphical user interface.
- **Config**: Configuration files and settings.

## Project Structure

```
.
├── boot
│   └── img
│       └── bootloader.img
├── src
│   ├── stage1
│   │   ├── start.asm
│   │   ├── gdt.asm
│   │   ├── bpb.asm
│   │   ├── log.asm
│   │   ├── mem.asm
│   │   ├── macro16.asm
│   │   ├── utils16.asm
│   │   ├── utils32.asm
│   ├── stage2
│   │   ├── main.c
│   │   └── link.ld
├── build
├── .gitignore
├── Makefile
└── README.md
```

## Installation

Currently, there is no installation process available.

## To Do

<details>
<summary>Click to expand</summary>

- [x] Place the code in MBR (first sector of the disk)
- [x] Setup 16-bit segment registers and stack
- [x] Print startup message
- [x] Enable and confirm enabled A20 line
- [x] Load GDTR
- [x] Setup GDT
- [x] Enable 32-bit Protected Mode
- [x] Call second stage C code
- [ ] Support GNU Multiboot
- [ ] Check presence of PCI, CPUID, MSRs
- [ ] Inform BIOS of target processor mode
- [ ] Get memory map from BIOS
- [ ] Locate kernel in filesystem
- [ ] Allocate memory to load kernel image
- [ ] Load kernel image into buffer
- [ ] Enable graphics mode
- [ ] Check kernel image ELF headers
- [ ] Allocate and map memory for kernel segments
- [ ] Setup COM serial output port
- [ ] Setup IDT
- [ ] Disable PIC
- [ ] Check presence of CPU features (NX, SMEP, x87, PCID, global pages, TCE, WP, MMX, SSE, SYSCALL), and enable them
- [ ] Assign a PAT to write combining
- [ ] Setup FS/GS base
- [ ] Load IDTR
- [ ] Enable APIC and setup using information in ACPI tables
- [ ] Setup TSS

</details>

## Build Instructions

### Prerequisites

- `nasm` (Netwide Assembler)
- `ld` (GNU Linker)
- `gcc` (GNU Compiler Collection)
- `make` (GNU Make)
- `clang` (LLVM Compiler)
- `qemu` (for running the bootloader)

### Building the Bootloader and running it in QEMU

To build the bootloader, run the following command:

```sh
make
```

This will generate the bootloader binary in the `build` directory.

### Cleaning the Build

To clean the build directory, run:

```sh
make clean
```

### Debugging and running the Bootloader in QEMU in suspended mode

To build the ELF file for debugging, run:

```sh
make debug
```

## Contribution

Contributions are welcome! Please fork the repository and submit a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
