# bootldr
This project focuses on understanding how bootloaders work

## Core

- [ ] OS loader (Unix family) 
- [ ] chsboot (aka. boot manager)
- [ ] GUI
- [ ] config
- [ ] OTA

### Installation
No instalation for now :<

### To do
- [x] Place the code in MBR (first sector of the disk) 
- [x] Setup 16-bit segment registers and stack
- [x] Print startup message
- [ ] Check presence of PCI, CPUID, MSRs
- [x] Enable and confirm enabled A20 line
- [x] Load GDTR
- [ ] Inform BIOS of target processor mode
- [ ] Get memory map from BIOS
- [ ] Locate kernel in filesystem
- [ ] Allocate memory to load kernel image
- [ ] Load kernel image into buffer
- [ ] Enable graphics mode
- [ ] Check kernel image ELF headers
- [x] Enable Protected Mode
- [ ] Allocate and map memory for kernel segments
- [ ] Setup COM serial output port
- [ ] Setup IDT
- [ ] Disable PIC
- [ ] Check presence of CPU features (NX, SMEP, x87, PCID, global pages, TCE, WP, MMX, SSE, SYSCALL), and enable them
- [ ] Assign a PAT to write combining
- [ ] Setup FS/GS base
- [ ] Load IDTR
- [ ] Enable APIC and setup using information in ACPI tables
- [ ] Setup GDT and TSS
