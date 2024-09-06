This directory contain an x86 legacy BIOS stage 1 bootloader that initializes the system in 16-bit Real Mode. 
Read the disk and loads the stage 2 bootloader. After all setup to switch to 32-bit Protected Mode it switches and loads the stage 2 bootloader.
The bootloader supports FAT16 and Multiboot specifications.