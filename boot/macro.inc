%ifndef MACRO_ASM
%define MACRO_ASM

; loads address of string to write into si register
; loop used for writing multiple characters to the display
%macro write_string 2
    push si
    push bx
    push ax

    xor cx, cx                      ; cx = 0
    mov si, %1                      ; si = address of string aka %1
    %%write_next_char:
        call write_char             ; call sub procedure
        inc cx                      ; cx += 1
        cmp cx, %2                  ; if cx < length of string
        jl %%write_next_char        ; jump

    pop ax
    pop bx
    pop si
%endmacro

%endif