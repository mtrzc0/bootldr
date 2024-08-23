BITS 16                                     ; tell assembler that we use Real mode
ORG 0x7C00                                  ; put code in to first sector of the disk

%include "input.asm"

mov ax, 0
mov ss, ax
mov sp, 0x7C00

call print_boot_msg
hlt                                         ; halt CPU and hang here

print_boot_msg:
    load_string boot_msg
    write_char_loop boot_msg_len
    ret                                     ; return

; DATA
boot_msg: db "Bootldr",0
boot_msg_len: equ  $-boot_msg

times 510-($ - $$) db 0		                ; fill rest of the code
dw 0xAA55				                    ; boot signature required by MBR
