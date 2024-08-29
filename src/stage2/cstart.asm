bits 32

section .text
global _cstart

extern bmain                    ; call C code

_cstart:
    call bmain
    cli
    hlt