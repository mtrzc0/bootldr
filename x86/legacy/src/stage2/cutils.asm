BITS 32                 ; Protected mode 32-bit code

global outb             ; make the label outb visible outside this file
global inb              ; make the label inb visible outside this file
global outw             ; make the label outw visible outside this file
global inw              ; make the label inw visible outside this file
global outdw            ; make the label outdw visible outside this file
global rep_insw         ; make the label rep_insw visible outside this file
global indw             ; make the label indw visible outside this file
global multiboot_entry  ; make the label load_kernel visible outside this file


;==============================================================================
; multiboot header
;==============================================================================
%define MULTIBOOT_MAGIC     0x1BADB002
%define MULTIBOOT_FLAGS     0x00000000

multiboot_h:
.magic:         dd MULTIBOOT_MAGIC
.flags:         dd MULTIBOOT_FLAGS
.checksum:      dd -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)
.header_addr:   dd multiboot_h
.load_addr:     dd $
.load_end_addr: dd 0x0000
.bss_end_addr:  dd 0x0000
.entry_addr:    dd 0x100000 ; default kernel entry point (HMA)

;==============================================================================
; functions
;==============================================================================

; multiboot_entry - load the kernel
; stack: [esp + 4] kernel entry address
;        [esp    ] return address
multiboot_entry:
    mov eax, [esp + 4]                  ; get the kernel entry address
    mov [multiboot_h.entry_addr], eax   ; save the kernel entry address
    ;TODO: stack pointer init
    ;TODO: reset eflags
    mov ebx, [multiboot_h]              ; get the address of the multiboot header
    mov eax, [multiboot_h.magic]        ; get the magic number
    push ebx                            ; save the address of the multiboot header
    push eax                            ; save the magic number
    jmp [multiboot_h.entry_addr]        ; jump to the kernel entry point
    ret

; outb - send a byte to an I/O port
; stack: [esp + 8] the data byte
;        [esp + 4] the I/O port
;        [esp    ] return address
outb:
    mov al, [esp + 8]    ; move the data to be sent into the al register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, al           ; send the data to the I/O port
    ret                  ; return to the calling function

; outw - send a word to an I/O port
; stack: [esp + 8] the data word
;        [esp + 4] the I/O port
;        [esp    ] return address
outw:
    mov ax, [esp + 8]    ; move the data to be sent into the ax register
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    out dx, ax           ; send the data to the I/O port
    ret                  ; return to the calling function

; outdw - send a double word to an I/O port
; stack: [esp + 8] the data double word
;        [esp + 4] the I/O port
;        [esp    ] return address
outdw:
    mov eax, [esp + 8]   ; move the data to be sent into the eax register
    mov dx, [esp + 4]    ; move the address of the I/O port into the edx register
    out dx, eax          ; send the data to the I/O port
    ret                  ; return to the calling function

; inb - read a byte from an I/O port
; stack: [esp + 4] the I/O port
;        [esp    ] return address
inb:
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    in al, dx            ; read a byte from the I/O port
    ret                  ; return to the calling function

; inw - read a word from an I/O port
; stack: [esp + 4] the I/O port
;        [esp    ] return address
inw:
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    in ax, dx            ; read a word from the I/O port
    ret                  ; return to the calling function

; indw - read a word from an I/O port
; stack: [esp + 4] the I/O port
;        [esp    ] return address
indw:
    mov dx, [esp + 4]    ; move the address of the I/O port into the edx register
    in eax, dx           ; read a double word from the I/O port
    ret                  ; return to the calling function

; rep_insw - read multiple words from an I/O port
; stack: [esp + 12] the number of words to read
;        [esp + 8] address of the buffer to store the data
;        [esp + 4] the I/O port
;        [esp    ] return address
rep_insw:
    mov dx, [esp + 4]    ; move the address of the I/O port into the dx register
    mov edi, [esp + 8]   ; move the address of the buffer into the edi register
    mov ecx, [esp + 12]  ; move the number of words to read into the ecx register
    rep insw             ; read multiple words from the I/O port
    ret