%ifndef UTILS16_ASM
%define UTILS16_ASM

BITS 16                             ; use 16-bit Real Mode

%include "gdt.asm"

; MACROS
%define START_STAGE2 0x7E00
%define NL 0x0A                                 ; '\n'
%define CR 0x0D                                 ; CR
%define LOG_OK "[  OK  ] "
%define LOG_FAIL "[ FAIL ] "

; writing string to the display
%macro write_string 1
    mov si, %1                              ; si = address of string aka %1
%%repeat:
    lodsb                                   ; load a byte pointed by si
    cmp al, 0                               ; check for null terminated string
    je %%exit                               ; if true end procedure
    call write_char                         ; call sub procedure
    jmp %%repeat                            ; jump to repeat
%%exit:
%endmacro

; FUNCTIONS

; initialize disk
disk_init:
    pusha
    ; check if lba extension is supperted
    mov ah, 0x41                    ; check extensions
    mov bx, 0x55AA                  ; magic number
    mov dl, 0x80                    ; disk number
    int 0x13                        ; call BIOS
    jc .lba_ext_not_sup             ; if carry flag is set, jump to error handler
    jmp .read_lba_ext               ; if not, jump to read disk using LBA

.read_lba_ext:
    mov si, DAPACK                  ; load DAP address to si
    mov ah, 0x42                    ; extended read function
    mov dl, 0x80                    ; disk number
    int 0x13                        ; call BIOS
    jc .fail                        ; if carry flag is set, jump to error handler
    jmp .ok                         ; if not, jump to success handler
.lba_ext_not_sup:
    call print_disk_lba_sup_fail    ; print failure message
    jmp .read_chs_0c0h63s           ; jump to read disk using CHS

; TODO: implement LBA to CHS conversion
; convert LBA to CHS
; use ax as LBA address
.lba_to_chs:
    nop

; read disk only 63 sectors (32K) in head 0 and cylinder 0 into memory using CHS
.read_chs_0c0h63s:
    pusha
    xor si, si                      ; set si to 0
.retry:
    mov ah, 0x02                    ; read disk BIOS function
    mov al, 0x01                    ; number of sectors to read
    mov cl, 0x02                    ; sector
    mov ch, 0x00                    ; cylinder
    mov dh, 0x00                    ; head
    mov dl, 0x00                    ; set disk number for floppy
    xor cx, cx                      ; set cx to 0
    mov es, cx                      ; set ES to 0
    mov bx, START_STAGE2            ; offset in segment
    int 0x13                        ; call BIOS
    jc .continue                    ; if carry flag is set, jump to error handler
    jmp .ok                         ; if not, jump to success handler
.continue:
    inc si                          ; increment si
    cmp si, 3                       ; check if we tried 3 times
    jne .retry                      ; if not, retry
    jmp .fail                       ; if yes, jump to error handler
.fail:
    call print_disk_read_fail       ; print failure message
    popa
    ret
.ok:
    call print_disk_read_ok         ; print success message
    popa
    ret

; enable a20 line
en_a20:
    push ax
    in al, 0x93                     ; switch A20 gate via fast A20 port 92
    or al, 2                        ; set A20 Gate bit 1
    and al, ~1                      ; clear INIT_NOW bit
    out 0x92, al
    pop ax
    ret

; enable 32-bit Protected Mode
en_pm:
    pusha
    cli                             ; disable interrupts
    lgdt [gdtr]                     ; load GDT

    mov eax, cr0
    or eax, 0x01                    ; enable protection bit in (Control reg)
    mov cr0, eax

    popa
    jmp CODE_SEG:_pmstart

; writes char from string buffer which si points to
write_char:
    pusha
    mov ah, 0x0E                    ; display char in AL
    int 0x10                        ; call BIOS
    popa
    ret

; PRINT MESSAGES

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
disk_read_ok_str:       db LOG_OK,"Reading disk",NL,CR,0
disk_read_fail_str:     db LOG_FAIL,"Reading disk",NL,CR,0
disk_lba_sup_fail_str:  db LOG_FAIL,"LBA extensions not supported",NL,CR,0

%endif ; UTILS16_ASM