%ifndef UTILS32_ASM
%define UTILS32_ASM

BITS 32                             ; use 32-bit Protected Mode

pm_init:
    mov ax, DATA_SEG                ; load data descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

putchar:
    mov eax, 0x07690748             ; loads strint into eax
    mov [0xb8000], eax              ; prints "Hi"
    ret

%endif