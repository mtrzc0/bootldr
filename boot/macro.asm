%ifndef MACRO_ASM
%define MACRO_ASM

%define ENDL 0xA,0xD
%define NL 0xA                      ; NL = '\n'
%define CR 0xD                      ; CR = "enter"

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
        jmp %%repeat                ; jump to repeat

%%done:
    pop ax
    pop si
%endmacro

; detecting low mem size (stored in ax) via BIOS
%macro low_mem 0
    clc                             ; clear carry flag
    int 0x12                        ; call BIOS for low memory size
    jc %%error                      ; if carry flag is set handle error
    call print_low_mem_msg_success  ; if success print msg
    %%error:
        ret
%endmacro

%macro high_mem 0

    %%error:
        ret
%endmacro

%endif
