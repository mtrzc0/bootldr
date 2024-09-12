%ifndef UTILS32_ASM
%define UTILS32_ASM

BITS 32                             ; use 32-bit Protected Mode

%define MULTIBOOT_MAGIC 0x1BADB002  ; Multiboot magic number

initpm:
    mov ax, DATA_SEG                ; load data descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

multiboot_init:
    mov eax, MULTIBOOT_MAGIC
    ; TODO: mov ebx, multiboot_info
    ; TODO: Must be a 32-bit read/execute code segment with an offset of ‘0’ and a limit of ‘0xFFFFFFFF’. The exact value is undefined.
    ;       mov cs, code_segment
    ; TODO: Must be a 32-bit read/write data segment with an offset of ‘0’ and a limit of ‘0xFFFFFFFF’. The exact values are all undefined.
    ;       mov ds, data_segment
    ;       mov es, data_segment
    ;       mov fs, data_segment
    ;       mov gs, data_segment
    ;       mov ss, data_segment
    ; TODO: Bit 31 (PG) must be cleared. Bit 0 (PE) must be set. Other bits are all undefined.
    ; TODO: Bit 17 (VM) must be cleared. Bit 9 (IF) must be cleared. Other bits are all undefined.
    ret

%endif ; UTILS32_ASM