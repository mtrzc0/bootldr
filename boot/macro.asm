%ifndef MACRO_ASM
%define MACRO_ASM

; loads address of string to write into si register
%macro load_string 1
    mov si, %1                      ; si = address of string aka %1
%endmacro

; loop used for writing multiple characters to the display
%macro write_char_loop 1
xor cx, cx                          ; cx = 0
    %%write_next_char:
        call write_char             ; call sub procedure
        inc cx                      ; cx += 1
        cmp cx, %1                  ; if cx < length of string
        jl %%write_next_char        ; jump
%endmacro

%endif