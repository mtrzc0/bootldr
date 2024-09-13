#ifndef SYS_H
#define SYS_H

#include "ctype.h"

/**
 * Sends a byte to the specified I/O port.
 *
 * This function writes the given byte to the specified I/O port.
 *
 * @param port The I/O port to write to.
 * @param data The byte to write to the I/O port.
 */
void outb(uint16_t port, uint8_t data);

/**
 * Reads a byte from the specified I/O port.
 *
 * This function reads a byte from the given I/O port and returns it.
 *
 * @param port The I/O port to read from.
 * @return The byte read from the I/O port.
 */
uint8_t inb(uint16_t port);

/**
 * Fills a block of memory with a specified value.
 *
 * This function sets the first `count` bytes of the memory area pointed to by `dest`
 * to the specified value `ch`.
 *
 * @param dest A pointer to the memory area to be filled.
 * @param ch The value to be set. It is passed as an int but is converted to an unsigned char.
 * @param count The number of bytes to be set to the value.
 * @return A pointer to the memory area `dest`.
 */
void *memset(void *dest, int32_t ch, size_t count);

/**
 * Fills a block of memory with a specified wide character value.
 *
 * This function sets the first `count` wide characters of the memory area pointed to by `dest`
 * to the specified wide character value `ch`.
 *
 * @param dest A pointer to the memory area to be filled.
 * @param ch The wide character value to be set.
 * @param count The number of wide characters to be set to the value.
 * @return A pointer to the memory area `dest`.
 */
void *memsetw(wchar_t *dest, wchar_t ch, size_t count);

/**
 * Copies `count` bytes from the memory area `src` to the memory area `dest`.
 *
 * This function performs a byte-by-byte copy from the source to the destination.
 * The memory areas must not overlap.
 *
 * @param dest A pointer to the destination memory area.
 * @param src A pointer to the source memory area.
 * @param count The number of bytes to copy.
 */
void memcpy(void *dest, const void *src, size_t count);

/**
 * Calculates the length of a null-terminated string.
 *
 * This function iterates through the string until it finds the null terminator
 * and returns the length of the string including the null terminator.
 *
 * @param str A pointer to the null-terminated string.
 * @return The length of the string including the null terminator.
 */
inline size_t strlen(const char *str);

#endif //SYS_H
