ENTRY(_cstart)

SECTIONS
{
    . = 0x7E00;

    .text ALIGN (0x00) :
    {
        *(.text)
    }
}