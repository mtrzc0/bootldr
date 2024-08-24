%include "macro.asm"
%include "input.asm"

BITS 16                                     ; tell assembler that we use Real mode
;ORG 0x7C00                                  ; put code in to first sector of the disk

section .data
boot_msg: db "Bootldr",0
boot_msg_len: equ  $-boot_msg

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

call print_boot_msg
hlt                                         ; halt CPU and hang here

print_boot_msg:
    write_string boot_msg, boot_msg_len
    ret                                     ; return

times 510-($ - $$) db 0		                ; fill rest of the code
dw 0xAA55				                    ; boot signature required by MBR
