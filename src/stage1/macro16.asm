%ifndef MACRO_ASM
%define MACRO_ASM

%define NL 0x0A                                 ; '\n'
%define CR 0x0D                                 ; CR
%define ENDL NL,CR                              ; '\n' + CR

BITS 16                                         ; use 16-bit Real Mode

; read disk into memory using CHS
%macro read_disk 0
    mov ah, 0x02                                ; read disk
    mov al, 1                                   ; read 1 sector
    mov ch, 0                                   ; cylinder 0
    mov dh, 0                                   ; head 0
    mov cl, 2                                   ; sector 2
    mov bx, 0x7E00                              ; buffer address
    int 0x13                                    ; call BIOS
    jc %%error                                  ; if carry flag is set handle error
    call print_disk_read_success_msg            ; if success print msg
    %%error:
        ret
%endmacro

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
