%ifndef MACRO_ASM
%define MACRO_ASM

%define NL 0x0A                                 ; '\n'
%define CR 0x0D                                 ; CR
%define ENDL NL,CR                              ; '\n' + CR

; initialize boot sector
%macro boot_sec_init 0
    xor ax, ax                                  ; ax = 0

    ; setup data segments registers
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ss, ax
    mov sp, 0x7C00
%endmacro

; writing string to the display
%macro write_string 1
    push si
    push ax

    mov si, %1                                  ; si = address of string aka %1
    %%repeat:
        lodsb                                   ; load a byte pointed by si
        cmp al, 0                               ; check for null terminated string
        je %%done                               ; if true end procedure
        call write_char                         ; call sub procedure
        jmp %%repeat                            ; jump to repeat

%%done:
    pop ax
    pop si
%endmacro

; detecting low mem size (stored in ax) via BIOS
%macro low_mem 0
    clc                                         ; clear carry flag
    int 0x12                                    ; call BIOS for low memory size
    jc %%error                                  ; if carry flag is set handle error
    call print_low_mem_success_msg              ; if success print msg
    %%error:
        ret
%endmacro

%endif
