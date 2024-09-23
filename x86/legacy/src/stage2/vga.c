#include "vga.h"

#include "sys.h"

static wchar_t *_vga_addr;
static vga_cursor_t _vga_crs;

void vga_init(void) {
    // set up the pointer to the VGA buffer
    _vga_addr = (wchar_t *) 0xB8000;
    vga_crs_set(0, 0);
    vga_cls();
    log_ok("VGA driver: init");
}

uint32_t vga_i(void) {
    // access the VGA buffer as a 1D array
    // using the formula: index = y * width + x
    return _vga_crs.y * VGA_WIDTH + _vga_crs.x;
}

void vga_crs_set(uint8_t x, uint8_t y) {
    _vga_crs.x = x;
    _vga_crs.y = y;
}

void vga_crs_update(void) {
    int32_t index = vga_i();
    outb(0x3d4, 14);
    outb(0x3d5, index >> 8);
    outb(0x3d4, 15);
    outb(0x3d5, index);
}

void vga_cls(void) {
    uint32_t blank = 0x20 | (VGA_COLOR_WHITE << 8);

    // fill each row with blank characters
    for (size_t i = 0; i < VGA_HEIGHT; i++) {
        // fill each column with blank characters
        memsetw(_vga_addr + i * VGA_WIDTH, blank, VGA_WIDTH);
    }

    vga_crs_set(0, 0);
}

void vga_scroll(void) {
    if (_vga_crs.y < VGA_HEIGHT) {
        return;
    }
    for (size_t i = 0; i <= VGA_HEIGHT; i++) {
        memcpyw(_vga_addr + (i - 1) * VGA_WIDTH, _vga_addr + i * VGA_WIDTH, VGA_WIDTH);
    }
    vga_crs_set(0, VGA_HEIGHT - 1);
    vga_crs_update();
}

void vga_putc(char chr, char style) {
    wchar_t *vga_buffer = _vga_addr + vga_i();

    if (chr == '\n') {
        _vga_crs.y++;
        _vga_crs.x = 0;
    } else if (chr == '\t') {
        _vga_crs.x += 4;
    } else if (chr >= WHITE_SPACE_ASCII) {
        // print every ascii character code after white space
        *vga_buffer = chr | (style << 8);
        _vga_crs.x++;
    }

    vga_crs_update();
}

