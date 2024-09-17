%include "bpb.asm"                          ; BIOS Parameter Block

%define START_STAGE1 0x7C00
%define START_STAGE2 0x7E00

extern bmain

section .text
global _start

BITS 16                                     ; use 16-bit Real Mode
_start:
    ; boot sector initialization
    xor ax, ax                              ; ax = 0
    mov ds, ax                              ; setup data segments registers
    mov es, ax
    mov ss, ax                              ; setup stack
    mov sp, START_STAGE1                    ; setup stack pointer

    ; prepare for 32-bit Protected Mode
    call disk_read_8K                      ; read 8K from disk
    call ena20                              ; enable a20 line
    call enpm                               ; enable 32-bit Protected Mode

BITS 32                                     ; use 32-bit Protected Mode
_pstart:
    ; prepare for 32-bit Protected Mode
    call initpm                             ; basic init of the Protected Mode

    ; call the C main function
    call bmain
    cli                                     ; disable interrupts
    ; TODO: add code to restart the system
    hlt

; include other files after _start label
%include "utils.asm"

times 510-($ - $$) db 0		                ; fill rest of the code with 0
dw 0xAA55				                    ; boot signature required by MBR
