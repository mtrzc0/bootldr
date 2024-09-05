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

void memset();

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
