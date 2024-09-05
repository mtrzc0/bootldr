BITS 32                             ; Protected mode 32-bit code

global outb
outb:
    ; C function prologue
    push ax
    push dx
    push ebp
    mov ebp, esp
    sub esp, 8                      ; make room for two 16-bit arguments
    mov al, [ebp-4]
    mov dx, [ebp-8]
    out dx, ax                      ; output byte in AL to port in DX
    ; C function epilogue
    mov esp, ebp
    pop ebp
    pop dx
    pop ax
    ret
