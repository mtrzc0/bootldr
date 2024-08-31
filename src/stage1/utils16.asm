%ifndef UTILS16_ASM
%define UTILS16_ASM

BITS 16                             ; use 16-bit Real Mode

%include "macro16.asm"
%include "gdt.asm"

; read disk into memory using CHS
disk_init:
    pusha
    read_disk
    popa
    ret

; writes char from string buffer which si points to
write_char:
    pusha
    mov ah, 0x0E                    ; display char in AL
    int 0x10                        ; call BIOS
    popa
    ret

; clear screen
clear16:
    pusha
    mov ah, 0x00
    mov al, 0x03
    int 0x10                        ; call BIOS
    popa
    ret

; detect low memory (RAM < 1MB)
low_mem_check:
    pusha
    low_mem
    popa
    ret

; enable a20 line
en_a20:
    push ax
    in al, 0x93         ; switch A20 gate via fast A20 port 92
    or al, 2            ; set A20 Gate bit 1
    and al, ~1          ; clear INIT_NOW bit
    out 0x92, al
    pop ax
    ret

; enable 32-bit Protected Mode
en_pm:
    push eax
    cli                             ; disable interrupts
    lgdt [gdtr]                     ; load GDT

    mov eax, cr0
    or eax, 0x01                    ; enable protection bit in (Control reg)
    mov cr0, eax

    pop eax
    jmp CODE_SEG:_pmstart

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

print_boot_msg:
    pusha
    write_string hello_msg
    popa
    ret

print_low_mem_success_msg:
    pusha
    write_string low_mem_msg_success
    popa
    ret

print_low_mem_error_msg:
    pusha
    write_string low_mem_msg_error
    popa
    ret

print_disk_read_success_msg:
    pusha
    write_string disk_read_success
    popa
    ret

print_disk_read_error_msg:
    pusha
    write_string disk_read_error
    popa
    ret

; DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hello_msg:           db "[ OK  ] Basic init",NL,CR,0
low_mem_msg_success: db "[ OK  ] Detecting lower memory",NL,CR,0
low_mem_msg_error:   db "[ ERR ] Detecting lower memory",NL,CR,0
disk_read_success:   db "[ OK  ] Reading disk",NL,CR,0
disk_read_error:     db "[ ERR ] Reading disk",NL,CR,0
drive_num:           db 0x00

%endif ; UTILS16_ASM