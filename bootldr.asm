BITS 16
ORG 0x7C00

global _main

section .data
	boot_mess: db "Bootldr",0
	boot_mess_len: equ  $-boot_mess

;section .bss

section .text

write_chr:
    mov al, [si]                            ; load character to al
    mov ah, 0x0E                            ; BIOS function - display character
    int 0x10                                ; call BIOS
    inc si                                  ; get next chr
    ret

_main: 
    mov si, boot_mess                       ; si = &boot_mess

print_next_chr: 
    xor cx, cx                              ; cx as i = 0
    call write_chr                          ; call subprocedure 
    inc cx                                  ; i++
    cmp cx, boot_mess_len                   ; if i < boot_mess_len
    jl print_next_chr                       ; jump to @@

    cli                                     ; disable interrupts
    hlt                                     ; CPU halt

	times 510-($ - $$) db 0		            ; fill rest of the code
	dw 0xAA55				                ; boot signature required by MBR
