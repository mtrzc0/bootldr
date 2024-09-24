%ifndef UTILS_ASM
%define UTILS_ASM

; ======================= 16-bit =======================
BITS 16                             ; use 16-bit Real Mode

%include "gdt.asm"
%include "bpb.asm"

; MACROS
; ======

; memory
%define START_STAGE2 0x7E00

; ascii
%define NL 0x0A                      ; '\n'
%define CR 0x0D                      ; CR

; writing string to the display
%macro write_string 1
    mov si, %1                       ; si = address of string aka %1
%%repeat:
    lodsb                            ; load a byte pointed by si
    cmp al, 0                        ; check for null terminated string
    je %%exit                        ; if true end procedure
    call write_char                  ; call sub procedure
    jmp %%repeat                     ; jump to repeat
%%exit:
%endmacro

; FUNCTIONS
; =========

; read from disk for the next stage
disk_read:
    pusha
    xor di, di                      ; reset retry counter
    mov bx, START_STAGE2            ; set buffer for sector
    mov ax, 0x027F                  ;
    mov cx, 0x0002                  ; Set up registers for reading 64K from disk (128 sectors)
    mov dx, 0x0080                  ;
.retry:
    int 0x13                        ; call BIOS interrupt
    jnc .ok                         ; if no carry (success), jump to success
    inc di                          ; increment retry counter
    cmp di, 3                       ; check if retried 3 times
    jne .retry                      ; if not, retry
    call print_disk_read_fail
    jmp .exit                       ; exit on failure after 3 retries
.ok:
    call print_disk_read_ok
.exit:
    popa
    ret

; enable a20 line
ena20:
    push ax
    in al, 0x93                     ; switch A20 gate via fast A20 port 92
    or al, 2                        ; set A20 Gate bit 1
    and al, ~1                      ; clear INIT_NOW bit
    out 0x92, al
    pop ax
    ret

; enable 32-bit Protected Mode
enpm:
    pusha
    cli                             ; disable interrupts
    lgdt [gdtr]                     ; load GDT

    mov eax, cr0
    or eax, 0x01                    ; enable protection bit in (Control reg)
    mov cr0, eax

    popa
    jmp CODE_SEG:_pstart            ; jump to the next stage

; writes char from string buffer which si points to
write_char:
    pusha
    mov ah, 0x0E                    ; display char in AL
    int 0x10                        ; call BIOS
    popa
    ret

; PRINT MESSAGES
; ==============

print_disk_read_ok:
    pusha
    write_string disk_read_ok_str
    popa
    ret

print_disk_read_fail:
    pusha
    write_string disk_read_fail_str
    popa
    ret

print_disk_lba_sup_fail:
    pusha
    write_string disk_lba_sup_fail_str
    popa
    ret

; DATA
disk_read_ok_str:       db "Reading disk...OK",NL,CR,0
disk_read_fail_str:     db "Reading disk...FAIL",NL,CR,0
disk_lba_sup_fail_str:  db "LBA extensions not supported",NL,CR,0


; ======================= 32-bit =======================
BITS 32                             ; use 32-bit Protected Mode

%define MULTIBOOT_MAGIC 0x1BADB002  ; Multiboot magic number

; FUNCTIONS
; =========

; basic init of the Protected Mode
initpm:
    mov ax, DATA_SEG                ; load data descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

; MULTIBOOT header
multiboot_init:
    mov eax, MULTIBOOT_MAGIC
    ; TODO: mov ebx, multiboot_info
    ; TODO: Must be a 32-bit read/execute code segment with an offset of ‘0’ and a limit of ‘0xFFFFFFFF’. The exact value is undefined.
    ;       mov cs, code_segment
    ; TODO: Must be a 32-bit read/write data segment with an offset of ‘0’ and a limit of ‘0xFFFFFFFF’. The exact values are all undefined.
    ;       mov ds, data_segment
    ;       mov es, data_segment
    ;       mov fs, data_segment
    ;       mov gs, data_segment
    ;       mov ss, data_segment
    ; TODO: Bit 31 (PG) must be cleared. Bit 0 (PE) must be set. Other bits are all undefined.
    ; TODO: Bit 17 (VM) must be cleared. Bit 9 (IF) must be cleared. Other bits are all undefined.
    ret

%endif ; UTILS_ASM