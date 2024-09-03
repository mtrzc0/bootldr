#ifndef VGA_H
#define VGA_H

#include "ctype.h"

/*
    Below are defined data structures,
    used in printing data to the screen.
    To print data we use 0xB8000 address,
    which is the address of the VGA text buffer.

    Each character is 16-bit long, it looks like this:
    +------------------+----------------------------+
    | 8 bits for style | 8 bits for ASCII character |
    +------------------+----------------------------+

    Where style is a combination of foreground and background colors.
    +-----------------------------------------------------------+
    | 4 bits for background color | 4 bits for foreground color |
    +-----------------------------------------------------------+
*/

#define VGA_BUFFER (wchar_t*)0xB8000

const uint16_t VGA_WIDTH = 80;
const uint16_t VGA_HEIGHT = 25;

/* VGA text mode color palette */
enum vga_color_palette16_t {
    VGA_COLOR_BLACK,
    VGA_COLOR_BLUE,
    VGA_COLOR_GREEN,
    VGA_COLOR_CYAN,
    VGA_COLOR_RED,
    VGA_COLOR_MAGENTA,
    VGA_COLOR_BROWN,
    VGA_COLOR_LIGHT_GRAY,
    VGA_COLOR_DARK_GRAY,
    VGA_COLOR_LIGHT_BLUE,
    VGA_COLOR_LIGHT_GREEN,
    VGA_COLOR_LIGHT_CYAN,
    VGA_COLOR_LIGHT_RED,
    VGA_COLOR_LIGHT_MAGENTA,
    VGA_COLOR_YELLOW,
    VGA_COLOR_WHITE
};

/* VGA text mode style */
enum vga_style_t {
    VGA_STYLE_TEXT = (VGA_COLOR_LIGHT_GRAY << 8),
    VGA_STYLE_LOG_OK = (VGA_COLOR_LIGHT_GREEN << 8),
    VGA_STYLE_LOG_WARN = (VGA_COLOR_YELLOW << 8),
    VGA_STYLE_LOG_FAIL = (VGA_COLOR_LIGHT_RED << 8)
};

/* VGA text mode cursor */
typedef struct {
    uint8_t x;
    uint8_t y;
} vga_cursor_t;

void bputc(char chr, char style);
void bprintf(const char *str);

#endif //VGA_H