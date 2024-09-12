BITS 16                                     ; use 16-bit Real Mode

%include "bpb.asm"                          ; BIOS Parameter Block

%define START_STAGE1 0x7C00
%define START_STAGE2 0x7E00

global _start
extern bmain

section .text

_start:
    ; boot sector initialization
    xor ax, ax                              ; ax = 0
    mov ds, ax                              ; setup data segments registers
    mov es, ax
    mov ss, ax                              ; setup stack
    mov sp, START_STAGE1                    ; setup stack pointer

    ; prepare for 32-bit Protected Mode
    call disk_read_64K                      ; read 64K from disk
    call ena20                              ; enable a20 line
    call enpm                               ; enable 32-bit Protected Mode

BITS 32                                     ; use 32-bit Protected Mode
_pstart:
    call initpm                             ; basic init of the Protected Mode
    ;call bmain
    jmp START_STAGE2                        ; jump to the next stage
    cli                                     ; disable interrupts
    hlt

; include other files after _start label
%include "utils16.asm"
%include "utils32.asm"

times 510-($ - $$) db 0		                ; fill rest of the code with 0
dw 0xAA55				                    ; boot signature required by MBR
