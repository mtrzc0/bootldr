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
    VGA_STYLE_TEXT = VGA_COLOR_LIGHT_GRAY,
    VGA_STYLE_LOG_OK = VGA_COLOR_LIGHT_GREEN,
    VGA_STYLE_LOG_WARN = VGA_COLOR_YELLOW,
    VGA_STYLE_LOG_FAIL = VGA_COLOR_LIGHT_RED
};

/* VGA text mode cursor */
typedef struct {
    uint8_t x;
    uint8_t y;
} vga_cursor_t;

/**
 * Initializes the VGA text mode.
 *
 * This function sets up the VGA text mode by configuring the necessary
 * hardware settings and preparing the text buffer for use.
 */
void vga_init(void);

/**
 * Updates the VGA cursor position.
 *
 * This function updates the position of the cursor in the VGA text buffer
 * to reflect the current cursor coordinates.
 */
void vga_cursor_update(void);

/**
 * Clears the screen.
 *
 * This function clears the VGA text buffer, effectively resetting the screen
 * to a blank state.
 */
// void cls(void);

/**
 * Outputs a character to the VGA text buffer with a specified style.
 *
 * This function places a character at the current cursor position in the
 * VGA text buffer and applies the given style to it.
 *
 * @param chr The character to output.
 * @param style The style to apply to the character.
 */
void bputc(char chr, char style);

/**
 * Outputs a formatted string to the VGA text buffer.
 *
 * This function takes a null-terminated string and outputs it to the VGA text buffer,
 * starting at the current cursor position. The string can contain format specifiers
 * which will be replaced by the corresponding arguments.
 *
 * @param str The null-terminated string to output.
 */
void bprintf(const char *str);

#endif //VGA_H