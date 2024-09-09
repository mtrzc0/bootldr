BITS 16                                     ; use 16-bit Real Mode

%include "bpb.asm"                          ; BIOS Parameter Block

%define START_STAGE1 0x7C00
%define START_STAGE2 0x7E00

section .text
global _stage1

; Disk Address Packet (DAP) structure
; FIXME: reading LBAs above 127
; TODO: read up to 1.44 MB (2879 sectors)
DAPACK:     db 0x10
            db 0
            dw 127                         ; sectors to read
.buf_addr:  dw START_STAGE2                ; buffer address
            dw 0                           ; segment address
.lba_addr:  dd 1                           ; lower 32-bits of LBA address
            dd 0                           ; upper 16-bits of LBA address

_stage1:
    ; boot sector initialization
    xor ax, ax                              ; ax = 0
    mov ds, ax                              ; setup data segments registers
    mov es, ax
    mov ss, ax                              ; setup stack
    mov sp, START_STAGE1                    ; setup stack pointer

    ; prepare for 32-bit Protected Mode
    call disk_init_lba                      ; read disk into memory using CHS
    call en_a20                             ; enable a20 line
    call en_pm                              ; enable 32-bit Protected Mode

BITS 32                                     ; use 32-bit Protected Mode
_stage2:
    call pm_init                            ; basic init of the Protected Mode
    jmp START_STAGE2                        ; jump to the next stage
    cli                                     ; disable interrupts
    hlt

; include other files after _start label
%include "utils16.asm"
%include "utils32.asm"

times 510-($ - $$) db 0		                ; fill rest of the code with 0
dw 0xAA55				                    ; boot signature required by MBR
