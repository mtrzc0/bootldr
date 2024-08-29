OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    . = 0x7C00;

    .text ALIGN (0x00) :
    {
        *(.text)
    }

    . = 0x7E00;


}