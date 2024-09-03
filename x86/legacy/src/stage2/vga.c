#include "vga.h"

static vga_cursor_t vga_cursor = {0, 0};
static uint16_t vga_index = 0;

void bputc(char chr, char style) {

    vga_index = (vga_cursor.y * VGA_WIDTH) + vga_cursor.x;
    wchar_t *vga_buffer = VGA_BUFFER + vga_index;
    *vga_buffer = chr | (style << 8);
}