%ifndef BPB_ASM
%define BPB_ASM

BITS 16                             ; use 16-bit Real Mode

BS_JmpBoot:                             ; Jump instruction to boot code
    jmp short _start
    nop

; BIOS Parameter Block
BS_OEMName:                 db "MSWIN4.1"               ; OEM Name
BPB_BytsPerSec:             dw 512                      ; Bytes per sector
BPB_SecPerClus:             db 1                        ; Sectors per cluster
BPB_RsvdSecCnt:             dw 1                        ; Reserved sectors (boot sector => kernel will be loaded from disk)
BPB_NumFATs:                db 2                        ; Number of FATs
BPB_RootEntCnt:             dw 512                      ; Number of root directory entries
BPB_TotSec16:               dw 2880                     ; Total sectors in logical volume
BPB_Media:                  db 0xF0                     ; Media descriptor type
BPB_FATSz16:                dw 9                        ; Sectors per FAT
BPB_SecPerTrk:              dw 18                       ; Sectors per track
BPB_NumHeads:               dw 2                        ; Number of heads or sides
BPB_HiddSec:                dd 0                        ; Number of hidden sectors
BPB_TotSec32:               dd 0                        ; Large sector count

; EBPB FAT16
BS_DrvNum:                  db 0                        ; 0x00 for floppy, 0x80 for hard drive
BS_Reserved:                db 0                        ; Windows NT flags (reserved)
BS_BootSig:                 db 0x29                     ; Extended boot signature
BS_VolID:                   dd 0                        ; 4-byte serial number
BS_VolLab:                  db "BOOTLDR    "            ; Volume label
BS_FilSysType:              db "FAT16   "               ; File system type

%endif ; BPB_ASM
