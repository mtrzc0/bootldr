BITS 16                                     ; tell assembler that we use Real mode
;ORG 0x7C00                                 commented out because of compatibility with GNU ld

section .text
global _start

_start:
xor ax, ax                                  ; ax = 0

; setup data segments registers
mov ds, ax
mov es, ax

; setup stack
mov ss, ax
mov sp, 0x7C00

call print_boot_msg                         ; print hello
call check_low_mem                          ; detect lower memory (< 1MB)
hlt                                         ; halt CPU and hang here

%include "io.asm"

times 510-($ - $$) db 0		                ; fill rest of the code
dw 0xAA55				                    ; boot signature required by MBR
