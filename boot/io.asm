%ifndef INPUT_ASM
%define INPUT_ASM

%include "macro.asm"

; writes char from string buffer which si points to
write_char:
    mov ah, 0x0E                    ; display char in AL
    int 0x10                        ; call BIOS
    ret                             ; return

; setup cursor column coordinate to zero 
endl:
    write_string new_line
    ret

; print string to the display
print_boot_msg:
    write_string boot_msg
    ret

print_mem_detect_msg:
    write_string mem_detect_msg
    ret

; DATA
boot_msg: db "Welcome in Bootldr",0
mem_detect_msg: db "Detecting memory...",0
new_line: db "",NL,CR,0

%endif
