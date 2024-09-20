#include "vga.h"

#include "sys.h"

static const uint16_t VGA_WIDTH = 80;
static const uint16_t VGA_HEIGHT = 25;

static wchar_t *vga_addr;
static vga_cursor_t vga_cursor = {0, 0};

void vga_init(void) {
    // set up the pointer to the VGA buffer
    vga_addr = (wchar_t *) 0xB8000;
    cls();
    log_ok("VGA init");
}

uint32_t vga_i(vga_cursor_t *cursor) {
    // access the VGA buffer as a 1D array
    // using the formula: index = y * width + x
    return cursor->y * VGA_WIDTH + cursor->x;
}

void vga_cursor_update(void) {
    int32_t index = vga_i(&vga_cursor);
    outb(0x3d4, 14);
    outb(0x3d5, index >> 8);
    outb(0x3d4, 15);
    outb(0x3d5, index);
}

void cls(void) {
    uint32_t blank = 0x20 | (VGA_COLOR_WHITE << 8);

    // fill each row with blank characters
    for (size_t i = 0; i < VGA_HEIGHT; i++) {
        // fill each column with blank characters
        memsetw(vga_addr + i * VGA_WIDTH, blank, VGA_WIDTH);
    }

    vga_cursor.x = 0;
    vga_cursor.y = 0;
}

void bputc(char chr, char style) {
    wchar_t *vga_buffer = vga_addr + vga_i(&vga_cursor);

    if (chr == '\n') {
        vga_cursor.y++;
        vga_cursor.x = 0;
    } else if (chr == '\t') {
        vga_cursor.x += 4;
    } else if (chr >= WHITE_SPACE_ASCII) {
        // print every ascii character code after white space
        *vga_buffer = chr | (style << 8);
        vga_cursor.x++;
    }

    vga_cursor_update();
}

void printb(const char *str) {
    for(size_t i = 0; str[i] != '\0'; i++) {
        bputc(str[i], VGA_STYLE_TEXT);
    }
}

void log_ok(const char *str) {
    const char *msg = "[ OK ] ";
    bputc(msg[0], VGA_STYLE_TEXT);
    bputc(msg[1], VGA_STYLE_TEXT);
    bputc(msg[2], VGA_STYLE_LOG_OK);
    bputc(msg[3], VGA_STYLE_LOG_OK);
    bputc(msg[4], VGA_STYLE_TEXT);
    bputc(msg[5], VGA_STYLE_TEXT);
    bputc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    bputc('\n', VGA_STYLE_TEXT);
}

void log_fail(const char *str) {
    const char *msg = "[FAIL] ";
    bputc(msg[0], VGA_STYLE_TEXT);
    bputc(msg[1], VGA_STYLE_LOG_FAIL);
    bputc(msg[2], VGA_STYLE_LOG_FAIL);
    bputc(msg[3], VGA_STYLE_LOG_FAIL);
    bputc(msg[4], VGA_STYLE_LOG_FAIL);
    bputc(msg[5], VGA_STYLE_TEXT);
    bputc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    bputc('\n', VGA_STYLE_TEXT);
}

void log_info(const char *str) {
    const char *msg = "[INFO] ";
    bputc(msg[0], VGA_STYLE_TEXT);
    bputc(msg[1], VGA_STYLE_LOG_INFO);
    bputc(msg[2], VGA_STYLE_LOG_INFO);
    bputc(msg[3], VGA_STYLE_LOG_INFO);
    bputc(msg[4], VGA_STYLE_LOG_INFO);
    bputc(msg[5], VGA_STYLE_TEXT);
    bputc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    bputc('\n', VGA_STYLE_TEXT);
}