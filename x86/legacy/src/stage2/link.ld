OUTPUT_FORMAT("binary")
ENTRY(bmain)

SECTIONS {
    . = 0x7E00;

    .text ALIGN (0x00) :
    {
        *(.text)
    }

    .data ALIGN (0x00) :
    {
        *(.data)
    }

    .bss ALIGN (0x00) :
    {
        *(.bss)
        *(COMMON)
    }
}