BITS 32                 ; Protected mode 32-bit code

global outb             ; make the label outb visible outside this file
global inb              ; make the label inb visible outside this file
global outw             ; make the label outw visible outside this file
global inw              ; make the label inw visible outside this file

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