%ifndef INPUT_ASM
%define INPUT_ASM

%include "macro.asm"

; writes char from string buffer which si points to
write_char:
    mov ah, 0x0E                    ; load BIOS function - display char
    int 0x10                        ; call BIOS
    ret                             ; return

; print string to the display
print_boot_msg:
    write_string boot_msg
    ret

; DATA
boot_msg: db "Bootldr",0

%endif