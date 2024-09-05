#include "vga.h"
#include "sys.h"

static const uint16_t VGA_WIDTH = 80;
static const uint16_t VGA_HEIGHT = 25;

static wchar_t *vga_addr;
static vga_cursor_t vga_cursor = {0, 0};
static uint16_t vga_index = 0;

void vga_init(void) {
    vga_addr = (wchar_t *) 0xB8000;
}

void vga_cursor_update(void) {
    int32_t index = vga_cursor.y * VGA_WIDTH + vga_cursor.x;
    outb(0x3d4, 14);
    outb(0x3d5, index >> 8);
    outb(0x3d4, 15);
    outb(0x3d5, index);
}

void bputc(char chr, char style) {
    vga_index = (vga_cursor.y * VGA_WIDTH) + vga_cursor.x;
    wchar_t *vga_buffer = vga_addr + vga_index;
    *vga_buffer = chr | (style << 8);

    if (chr == '\n') {
        vga_cursor.y++;
        vga_cursor.x = 0;
    } else if (chr == '\t') {
        vga_cursor.x += 4;
    } else {
        vga_cursor.x++;
    }
}

void bprintf(const char *str) {
    for(size_t i = 0; str[i] != '\0'; i++) {
        bputc(str[i], VGA_STYLE_TEXT);
    }
}