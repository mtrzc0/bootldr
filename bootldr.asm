BITS 16
ORG 0x7C00

mov ax, 0
mov ss, ax
mov sp, 0x7C00

call write_string
hlt                                         ; halt CPU and hang here

; WRITE CHAR PROC
write_char:
    mov al, [si]                            ; load character to al
    mov ah, 0x0E                            ; BIOS function - display character
    int 0x10                                ; call BIOS
    inc si                                  ; get next chr
    ret                                     ; Return
; END

; WRITE STRING PROC
write_string: 
xor cx, cx                                  ; cx = 0
mov si, boot_mess                           ; si = &boot_mess

print_next_char:
    call write_char                         ; call subprocedure 
    inc cx
    cmp cx, boot_mess_len                   ; if i < boot_mess_len
    jl print_next_char                      ; jump 
    ret
; END

; DATA
boot_mess: db "Bootldr",0
boot_mess_len: equ  $-boot_mess

times 510-($ - $$) db 0		                ; fill rest of the code
dw 0xAA55				                    ; boot signature required by MBR
