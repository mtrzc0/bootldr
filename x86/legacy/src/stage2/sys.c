#include "sys.h"

inline size_t strlen(const char *str) {
    // Loop over the null terminated string, stop at '\0' character
    size_t len;
    for (len = 0; str[len] != '\0'; len++)
        ;
    return len + 1;
}

void memcpy(void *dest, const void *src, size_t count) {
    // Cast the pointers to char pointers for byte-by-byte copying
    char *d = (char *)dest;
    const char *s = (const char *)src;

    // Copy each byte from source to destination
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
}

void *memset(void *dest, int32_t ch, size_t count) {
    char *d = (char *)dest;
    for (size_t i = 0; i < count; i++) {
        d[i] = ch;
    }
    return dest;
}

void *memsetw(wchar_t *dest, wchar_t ch, size_t count) {
    for (size_t i = 0; i < count; i++) {
        dest[i] = ch;
    }
    return dest;
}