%ifndef BPB_ASM
%define BPB_ASM

BITS 16                             ; use 16-bit Real Mode

jmp short _start
nop

; BIOS Parameter Block
oem:                                        db "MSWIN4.1"               ; OEM Name
bytes_per_sector:                           dw 512                      ; Bytes per sector
num_of_sectors_per_cluster:                 db 1                        ; Sectors per cluster
reserved_sectors:                           dw 1                        ; Reserved sectors
num_of_FATs:                                db 2                        ; Number of FATs
num_of_root_dir_entries:                    dw 224                      ; Number of root directory entries
total_sectors_in_logical_volume:            dw 2880                     ; Total sectors in logical volume
media_descriptor_type:                      db 0xF0                     ; Media descriptor type
sectors_per_FAT:                            dw 9                        ; Sectors per FAT
sectors_per_track:                          dw 18                       ; Sectors per track
heads_or_sides_on_media:                    dw 2                        ; Number of heads or sides
hidden_sectors:                             dd 0                        ; Number of hidden sectors
large_sector_count:                         dd 0                        ; Large sector count

; EBPB FAT16
drive_number:                               db 0                        ; 0x00 for floppy, 0x80 for hard drive
windows_nt_flags:                           db 0                        ; Windows NT flags (reserved)
signature:                                  db 0x29                     ; Extended boot signature
volume_id:                                  dd 0                        ; 4-byte serial number
volume_label:                               db "  BOOTLDR  "            ; Volume label
file_system_type:                           db "  FAT16 "               ; File system type

%endif ; BPB_ASM
