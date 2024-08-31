%include "mem.asm"                          ; memory related macros
%include "bpb.asm"                          ; BIOS Parameter Block

BITS 16                                     ; use 16-bit Real Mode

section .text

global _start
_start:
    boot_sector_init
    call print_boot_msg                     ; print hello
    call low_mem_check                      ; detect lower memory (< 1MB)
    call disk_init                          ; read disk into memory
    call clear16                            ; clear the screen
    call en_a20                             ; enable a20 line
    call en_pm                              ; enable 32-bit Protected Mode

BITS 32                                     ; use 32-bit Protected Mode
_pmstart:
    call pm_init                            ; basic init of the Protected Mode
    ;call putchar                            ; Protected Mode test
    jmp START_STAGE2                        ; jump to the next stage
    hlt

%include "utils16.asm"
%include "utils32.asm"

times 510-($ - $$) db 0		                ; fill rest of the code with 0
dw 0xAA55				                    ; boot signature required by MBR
