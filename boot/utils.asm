%ifndef INPUT_ASM
%define INPUT_ASM

%include "macro.asm"
%include "gdt.asm"

; writes char from string buffer which si points to
write_char:
    mov ah, 0x0E                    ; display char in AL
    int 0x10                        ; call BIOS
    ret                             ; return

; detect low memory (RAM < 1MB)
low_mem_check:
    call low_mem
    ret

; enable a20 line
en_a20:
    in al, 0x93         ; switch A20 gate via fast A20 port 92

    or al, 2            ; set A20 Gate bit 1
    and al, ~1          ; clear INIT_NOW bit
    out 0x92, al

    ret

; enable 32-bit protected mode
en_pm:
    cli                             ; disable interrupts
    lgdt [gdtr]                     ; load GDT

    mov eax, cr0
    or eax, 0x01                    ; enable protection bit in (Control reg)
    mov cr0, eax

    jmp CODE_SEG:_pmstart
    ret

pm_init:
    mov ax, DATA_SEG                ; load data descriptor
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x00                    ; setup stack pointer to 0x00
    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

print_boot_msg:
    write_string hello_msg
    ret

print_low_mem_success_msg:
    write_string low_mem_msg_success
    ret

; DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hello_msg:           db "Loading...",NL,CR,0
low_mem_msg_success: db "Successfuly detected lower memory",NL,CR,0

%endif