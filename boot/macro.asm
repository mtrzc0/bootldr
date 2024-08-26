%ifndef MACRO_ASM
%define MACRO_ASM

; writing string to the display
%macro write_string 1
    push si
    push ax

    mov si, %1                      ; si = address of string aka %1
    %%repeat:
        lodsb                       ; load a byte pointed by si
        cmp al, 0                   ; check for null terminated string
        je %%done                   ; if true end procedure
        call write_char             ; call sub procedure
        jmp %%repeat                 ; jump to repeat

%%done:
    pop ax
    pop si
%endmacro

%endif