OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    . = 0x7C00;

    .text ALIGN (0x00) :
    {
        *(.text)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame_hdr)
        *(.eh_frame)
    }

    . = 0x7E00;
}