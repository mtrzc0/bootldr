#ifndef SYS_H
#define SYS_H

// this macro emulates the behavior of the log10 function for unsigned integers
#define LOG10(x) ((x) < 10 ? 0 : \
                 ((x) < 100 ? 1 : \
                 ((x) < 1000 ? 2 : -1)))

// global for functions that return stack allocated strings
// use this in single threaded environment without heap
static char *_strtemp;

typedef unsigned int size_t;
typedef unsigned char uchar_t;
typedef unsigned short wchar_t; // 16-bit Unicode character
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned int uint32_t;
typedef signed int int32_t;

/**
 * @brief Sends a byte to the specified I/O port.
 *
 * This function writes the given byte to the specified I/O port.
 *
 * @param port The I/O port to write to.
 * @param data The byte to write to the I/O port.
 */
void outb(uint16_t port, uint8_t data);

/**
 * @brief Sends a word (2 bytes) to the specified I/O port.
 *
 * This function writes the given 16-bit word to the specified I/O port.
 *
 * @param port The I/O port to write to.
 * @param data The 16-bit word to write to the I/O port.
 */
void outw(uint16_t port, uint16_t data);

/**
 * @brief Reads a byte from the specified I/O port.
 *
 * This function reads a byte from the given I/O port and returns it.
 *
 * @param port The I/O port to read from.
 * @return The byte read from the I/O port.
 */
uint8_t inb(uint16_t port);

/**
 * @brief Reads a word (2 bytes) from the specified I/O port.
 *
 * This function reads a 16-bit word from the given I/O port and returns it.
 *
 * @param port The I/O port to read from.
 * @return The 16-bit word read from the I/O port.
 */
uint16_t inw(uint16_t port);

/**
 * @brief Fills a block of memory with a specified value.
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
 * @brief Fills a block of memory with a specified wide character value.
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
 * @brief Copies `count` bytes from the memory area `src` to the memory area `dest`.
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
 * @brief Calculates the length of a null-terminated string.
 *
 * This function iterates through the string until it finds the null terminator
 * and returns the length of the string including the null terminator.
 *
 * @param str A pointer to the null-terminated string.
 * @return The length of the string including the null terminator.
 */
size_t strlen(const char *str);

char *strformat(const char *str, uint16_t num);

#endif //SYS_H
