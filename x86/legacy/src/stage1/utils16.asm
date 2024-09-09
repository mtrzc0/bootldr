%ifndef UTILS16_ASM
%define UTILS16_ASM

BITS 16                             ; use 16-bit Real Mode

%include "gdt.asm"
%include "bpb.asm"

; MACROS
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

; convert LBA to CHS
; use si as LBA address
; return CH=cylinder, DH=head, CL=sector
lba_to_chs:
    mov ax, si                      ; save LBA address
    push bx                         ; save bx

    ; calculate sectors (cl)
    xor dx, dx                      ; clear dx
    div word [BPB_SecPerTrk]        ; ax = LBA / SPT, dx = LBA % SPT
    inc dx                          ; dx = LBA % SPT + 1
    mov cx, dx                      ; save sector number in cx

    ; calculate head (dh)
    xor dx, dx                      ; clear dx
    div word [BPB_NumHeads]         ; ax = LBA / (SPT * Heads), dx = LBA % (SPT * Heads)
    mov dh, dl                      ; save head number in dh

    ; calculate cylinder (ch)
    mov ch, al                      ; save cylinder number in ch
    shl ah, 6
    or cl, ah                       ; put upper 2 bits of cylinder in cl

    pop bx                          ; restore bx
    ret

; initialize disk with CHS
disk_init_chs:
    xor di, di                      ; set di to 0
    xor si, si                      ; load LBA to ax
    mov bx, START_STAGE2            ; buffer for sector
    jmp .loop                       ; jump to loop
.loop:
    inc si                          ; increment LBA
    call lba_to_chs                 ; convert LBA to CHS
    mov ah, 2                       ; read disk BIOS function
    mov al, 1                       ; number of sectors to read (127)
    mov dl, 80h                     ; disk number
    int 13h                         ; call BIOS
    jc .retry                       ; if carry flag is set, jump to error handler
    add bx, 200h                    ; next sector buffer
    ; TODO: fix reading LBAs above 65
    ; TODO: read up to 1.44 MB (2879 sectors)
    cmp si, 62                      ; check if we read enough sectors to fill 1.44 MB
    jle .loop                       ; if true read next sector
    jmp .ok                         ; if not, jump to success handler
.retry:
    clc                             ; enable reading disk using CHS
    mov ah, 0h                      ; reset disk BIOS function
    mov dl, 80h                     ; disk number
    int 13h                         ; call BIOS
    jc .fail                        ; if carry flag is set, jump to error handler
    inc di                          ; increment di
    cmp di, 3                       ; check if we tried 3 times
    jne .loop                       ; if not, retry
    jmp .fail                       ; if yes, jump to error handler
.fail:
    call print_disk_read_fail       ; print failure message
    ret
.ok:
    call print_disk_read_ok         ; print success message
    ret

; initialize disk with LBA
disk_init_lba:
    pusha
    ; check if lba extension is supperted
    mov ah, 0x41                    ; check extensions
    mov bx, 0x55AA                  ; magic number
    mov dl, 0x80                    ; disk number
    int 0x13                        ; call BIOS
    stc                             ; disable reading disk using int 13h extensions
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
    jmp .read_lba_via_chs           ; jump to read disk using CHS
.read_lba_via_chs:
    clc                             ; enable reading disk using CHS
    mov si, 1                       ; LBA = second sector
    xor di, di                      ; set di to 0
    mov bx, START_STAGE2            ; buffer for sector
    jmp .loop                       ; jump to loop
.loop:
    call lba_to_chs                 ; convert LBA to CHS
    call print_disk_read_ok         ; print success message
    mov ah, 0x02                    ; read disk BIOS function
    mov al, 0x01                    ; number of sectors to read
    mov dl, 0x80                    ; disk number 0
    int 0x13                        ; call BIOS
    jc .retry                       ; if carry flag is set, jump to error handler
    add bx, 0x200                   ; next sector buffer
    cmp si, 2879                    ; check if we read enough sectors to fill 1.44 MB
    inc si                          ; increment LBA
    jle .loop                       ; if true read next sector
    jmp .ok                         ; if not, jump to success handler
.retry:
    inc di                          ; increment di
    cmp di, 3                       ; check if we tried 3 times
    jne .loop                       ; if not, retry
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
disk_read_ok_str:       db "Reading disk...OK",NL,CR,0
disk_read_fail_str:     db "Reading disk...FAIL",NL,CR,0
disk_lba_sup_fail_str:  db "LBA extensions not supported",NL,CR,0

%endif ; UTILS16_ASM