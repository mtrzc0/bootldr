%ifndef GDT_ASM
%define GDT_ASM

BITS 16                                 ; use 16-bit Real Mode

gdt_start:
gdt_null:
    dq 0x0                              ; null offset [63...0]  |
gdt_code:
    dw 0xFFFF                           ; limit       [15...0]  |
    dw 0x0                              ; base        [31..16]  |
    db 0x0                              ; base        [39..32]  |
    db 0b10011010                       ; access byte [47..40]  |
    db 0b11001111                       ; flags       [55..52]  | limit [51..48]
    db 0x0                              ; base        [63..56]  |
gdt_data:
    dw 0xFFFF                           ; limit       [15...0]  |
    dw 0x0                              ; base        [31..16]  |
    db 0x0                              ; base        [39..32]  |
    db 0b10010010                       ; access byte [47..40]  |
    db 0b11001111                       ; flags       [55..52]  | limit [51..48]
    db 0x0                              ; base        [63..56]  |
gdt_end:
gdtr:
    dw gdt_end - gdt_start - 1          ; calculate lengh of gdt struct
    dd gdt_start                        ; pointer to start

CODE_SEG equ gdt_code - gdt_start       ; address of code segment relative to start
DATA_SEG equ gdt_data - gdt_start       ; address of data segment relative to start

%endif ; GDT_ASM