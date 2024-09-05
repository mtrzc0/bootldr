%ifndef MACRO16_ASM
%define MACRO16_ASM

%include "mem.asm"

%define NL 0x0A                                 ; '\n'
%define CR 0x0D                                 ; CR
%define ENDL NL,CR                              ; '\n' + CR

%define MIN_SECTORS 1                           ; minimum sectors to read (0 is unused)
%define MAX_SECTORS 63                          ; maximum sectors per track
%define MAX_HEADS 15                            ; maximum number of heads
%define MIN_HEADS 0                             ; minimum number of heads (0 is the first head)
%define MAX_CYLINDERS 2                         ; maximum number of cylinders
%define MIN_CYLINDERS 0                         ; minimum number of cylinders
%define MAX_BUFFER_SIZE 0xFC000                 ; maximum buffer size (1 MB)
%define SECTOR_SIZE 0x200                       ; sector size (512 bytes)

BITS 16                                         ; use 16-bit Real Mode

; Read disk into memory using CHS (Cylinder, Head, Sector)
; Note: Drive label should be saved before calling this macro.
%macro read_disk 0
    mov [BS_DrvNum], dl                         ; save disk number in the memory location BS_DrvNum
    mov ah, 0x02                                ; BIOS function: read disk sectors
    mov al, 1                                   ; number of sectors to read at a time
    mov bx, START_STAGE2                        ; destination buffer address
    mov cl, MIN_SECTORS + 1                     ; start from sector 2 (usually bootloader is in sector 1)
    mov ch, MIN_CYLINDERS                       ; start from cylinder 0
    mov dh, MIN_HEADS                           ; start from head 0

    xor si, si                                  ; retry counter

%%retry:
    int 0x13                                    ; call BIOS to read disk
    jc %%error                                  ; if carry flag is set, jump to error handler

%%read_loop:
    add bx, SECTOR_SIZE                         ; increment buffer pointer to next sector
    cmp bx, MAX_BUFFER_SIZE                     ; check if buffer exceeds its maximum size
    jge %%done                                  ; if buffer is full, end procedure

    inc cl                                      ; increment sector number
    cmp cl, MAX_SECTORS                         ; check if we've reached the maximum sectors per track
    jle %%continue                              ; if not, continue reading

    mov cl, MIN_SECTORS + 1                     ; reset sector number to start
    inc dh                                      ; move to the next head
    cmp dh, MAX_HEADS                           ; check if we've reached the maximum number of heads
    jle %%continue                              ; if not, continue reading

    mov dh, MIN_HEADS                           ; reset head to start
    inc ch                                      ; move to the next cylinder
    cmp ch, MAX_CYLINDERS                       ; check if we've reached the maximum number of cylinders
    jle %%continue                              ; if not, continue reading

    jmp %%done                                  ; all cylinders, heads, and sectors have been read, exit

%%continue:
    int 0x13                                    ; call BIOS to read next sector
    jc %%error                                  ; if carry flag is set, handle the error
    jmp %%read_loop                             ; repeat reading loop

%%done:
    call print_disk_read_ok                     ; print success message
    jmp %%exit                                  ; exit the macro

%%error:
    call print_disk_read_fail                   ; print failure message
    inc si                                      ; increment retry counter
    cmp si, 3                                   ; allow up to 3 retries
    jl %%retry                                  ; retry if not exceeded

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
