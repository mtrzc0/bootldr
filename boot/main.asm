BITS 16                                     ; tell assembler that we use Real mode

section .text

global _start
_start:
    boot_sector_init
    call print_boot_msg                     ; print hello
    call low_mem_check                      ; detect lower memory (< 1MB)
    call en_a20                             ; enable a20 line
    call en_pm                              ; enable 32bit Protected mode

BITS 32
_pmstart:
    call pm_init                            ; basic init of the Protected mode
    hlt                                     ; halt CPU and hang here

%include "utils.asm"

times 510-($ - $$) db 0		                ; fill rest of the code with 0
dw 0xAA55				                    ; boot signature required by MBR
