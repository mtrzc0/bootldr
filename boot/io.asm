%ifndef INPUT_ASM
%define INPUT_ASM

%include "macro.asm"

; writes char from string buffer which si points to
write_char:
    mov ah, 0x0E                    ; display char in AL
    int 0x10                        ; call BIOS
    ret                             ; return

; detect low memory (RAM < 1MB)
check_low_mem:
    call low_mem
    ret
; print string to the display
print_boot_msg:
    write_string boot_msg
    ret

print_low_mem_msg_success:
    write_string low_mem_msg_success
    ret

; DATA
boot_msg: db "Bootldr Hello!",ENDL,0
low_mem_msg_success: db "Successfuly detected lower memory",ENDL,0

%endif
