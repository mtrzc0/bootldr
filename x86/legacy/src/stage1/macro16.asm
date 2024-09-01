%ifndef MACRO16_ASM
%define MACRO16_ASM

%include "mem.asm"

%define NL 0x0A                                 ; '\n'
%define CR 0x0D                                 ; CR
%define ENDL NL,CR                              ; '\n' + CR

%define MIN_SECTORS 1                           ; min sectors to read
%define MAX_SECTORS 63                          ; max sectors to read
%define MAX_HEADS 15                            ; max heads
%define MIN_HEADS 0                             ; min heads
%define MAX_CYLINDERS 2                         ; max cylinders
%define MIN_CYLINDERS 0                         ; min cylinders
%define MAX_BUFFER_SIZE 0xFC000                 ; max buffer size
%define SECTOR_SIZE 0x200                       ; sector size (512 bytes)

BITS 16                                         ; use 16-bit Real Mode

; read disk into memory using CHS
; remember to save drive label !
%macro read_disk 0
%%retry:
    mov ah, 0x02                                ; read disk
    mov [BS_DrvNum], dl                      ; save disk number
    mov al, 1                                   ; read 1 sector at a time
    mov bx, START_STAGE2                        ; buffer address
    mov cl, MIN_SECTORS + 1                     ; start from sector 2
    mov ch, MIN_CYLINDERS                       ; start from cylinder 0
    mov dh, MIN_HEADS                           ; start from head 0
    int 0x13                                    ; call BIOS to read disk
    xor si, si                                  ; used in counting retrys
    jc %%error                                  ; if carry flag is set, handle error
    ; Read 1 MB of data
%%loop:
    add bx, SECTOR_SIZE                         ; move buffer (512 bytes) to next sector
    cmp bx, MAX_BUFFER_SIZE                     ; check if buffer is full
    jge %%done                                  ; if buffer is full, end procedure
    inc cl                                      ; move to the next sector
    cmp cl, MAX_SECTORS                         ; check if reached max sectors
    jle %%continue                              ; if not, continue reading
    mov cl, 1                                   ; reset sector number to 1
    inc dh                                      ; move to next head
    cmp dh, MAX_HEADS                           ; check if reached max heads
    jle %%continue                              ; if not, continue reading
    xor dh, dh                                  ; reset head to 0
    inc ch                                      ; move to the next cylinder
    cmp ch, MAX_CYLINDERS                       ; check if reached max cylinders
    jle %%continue                              ; if not, continue reading
    jmp %%done                                  ; if reached max cylinders, done
%%continue:
    int 0x13                                    ; call BIOS to read disk
    jc %%error                                  ; if carry flag is set, handle error
    jmp %%loop                                  ; loop back
%%done:
    call print_disk_read_ok                     ; on success, print success message
    jmp %%exit
%%error:
    call print_disk_read_fail                   ; on error, print error message
    cmp si, 3
    inc si
    jl %%retry
%%exit:
%endmacro

; initialize boot sector
%macro boot_sec_init 0
    xor ax, ax                                  ; ax = 0

    ; setup data segments registers
    mov ds, ax
    mov es, ax

    ; setup stack
    mov ss, ax
    mov sp, 0x7C00
%endmacro

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

; detecting low mem size (stored in ax) via BIOS
%macro low_mem 0
    clc                                         ; clear carry flag
    int 0x12                                    ; call BIOS for low memory size
    jc %%error                                  ; if carry flag is set handle error
    call print_low_mem_ok                       ; if success print msg
    jmp %%exit
%%error:
    call print_low_mem_fail                    ; if error print msg
%%exit:
%endmacro

%endif ; MACRO16_ASM
