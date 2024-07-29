BITS 16
ORG 0x7c00

global main

section .data
	boot_mess: db "Bootldr",0
	boot_mess_len: equ  $-boot_mess

section .bss

section .text

main: 
	times 512-(boot_mess_len+16) db 0		; fill with zeros
	dw 0xAA55				                ; boot signature required by MBR

