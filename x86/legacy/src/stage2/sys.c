#include "sys.h"

uint16_t numlen(uint32_t num) {
    uint16_t len = 0;
    while (num > 0) {
        num /= 10;
        len++;
    }
    return len;
}

size_t strlen(const char *str) {
    // Loop over the null terminated string, stop at '\0' character
    size_t len;
    for (len = 0; str[len] != '\0'; len++)
        ;
    return len + 1;
}

char *strformat(const char *str, uint32_t num) {
    const size_t strl = strlen(str);
    const size_t numl = numlen(num);
    char digits[numl];
    const size_t resl = strl + numl - 1;
    char res[resl];

    // convert number to string
    if (numl > 0) {
        for (size_t i = 0; i < numl; i++) {
            digits[i] = num % 10 + '0';
            num /= 10;
        }
    }

    // format string
    for(size_t i = 0; str[i] != '\0'; i++) {
        switch (str[i]) {
            case '%':
                if (str[i+1] == 'd') {
                    for (size_t j = 0; j < numl; j++) {
                        res[i+j] = digits[numl-1-j];
                    }
                    i++;
                }
                break;
            default:
                // copy char to result string
                res[i] = str[i];
                break;
        }
    }
    memcpy(_strtemp, res, resl);
    return _strtemp;
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