#include "vga.h"

const char *msg = "Hello, World!";

int bmain(void) {
    vga_init();
    bputc('O', VGA_STYLE_TEXT);
    bputc('K', VGA_STYLE_TEXT);
    bputc('!', VGA_STYLE_TEXT);
    return 0;
}
