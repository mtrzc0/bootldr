gdt:
    dq 0x0                      ; null offset [63...0]  | 
    dw 0xFFFF                   ; limit       [15...0]  |
    dw 0x0                      ; base        [31..16]  |
    db 0x0                      ; base        [39..32]  |
    db 0b10010010               ; access byte [47..40]  |
    db 0b11001111               ; flags       [55..52]  | limit [51..48]
    db 0x0                      ; base        [63..56]  |
gdt_meta:
    dd gdt_meta - gdt - 1       ; calculate lengh of gdt struct
