%ifndef INPUT_ASM
%define INPUT_ASM

; writes char from string buffer which si points to
write_char:
    mov al, [si]                    ; load char to al
    mov ah, 0x0E                    ; load BIOS function - display char
    int 0x10                        ; call BIOS
    inc si                          ; get next char
    ret                             ; return

%endif