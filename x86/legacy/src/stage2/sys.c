#include "sys.h"
#include "vga.h"

// global variable for functions that return stack allocated strings
// used in single threaded environment without heap
static char *_strtemp;

uint16_t numlen(uint32_t num) {
    uint16_t len = 0;
    if (num == 0) {
        return 1;
    }
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

// TODO: replece this procedures with proper printf implementation
char *strfs(const char *str1, const char  *str2) {
    const size_t str1l = strlen(str1);
    const size_t str2l = strlen(str2);
    const size_t resl = str1l + str2l - 2;
    char res[resl];

    // format string
    for(size_t i = 0; i < resl; i++) {
        if (str1[i] == '%' && str1[i+1] == 's') {
            for (size_t j = 0; j < str2l; j++) {
                res[i+j] = str2[j];
            }
            i+=str2l;
        } else {
            // copy char to result string
            res[i] = str1[i];
        }
    }

    res[resl-1] = '\0';
    memcpy(_strtemp, res, resl);
    return _strtemp;
}

char *strfd(const char *str, uint32_t dec) {
    const size_t strl = strlen(str);
    const size_t numl = numlen(dec);
    char digits[numl];
    const size_t resl = strl + numl - 2;
    char res[resl];

    // convert number to string
    if (numl > 0) {
        for (size_t i = 0; i < numl; i++) {
            digits[i] = dec % 10 + '0';
            dec /= 10;
        }
    }

    // format string
    for(size_t i = 0; i < resl; i++) {
        if (str[i] == '%' && str[i+1] == 'd') {
            for (size_t j = 0; j < numl; j++) {
                res[i+j] = digits[numl-1-j];
            }
            i+=numl;
        } else {
            // copy char to result string
            res[i] = str[i];
        }
    }
    res[resl-1] = '\0';
    memcpy(_strtemp, res, resl);
    return _strtemp;
}

char *strfX(const char *str, uint32_t hex) {
    const size_t strl = strlen(str);
    const size_t numl = 8;
    char digits[numl];
    const size_t resl = strl + numl;
    char res[resl];

    // convert number to hex string
    if (numl > 0) {
        for (size_t i = 0; i < numl; i+=2) {
            const uint8_t temp = hex >> 4 * i;
            digits[i] = "0123456789ABCDEF"[temp >> 4];
            digits[i+1] = "0123456789ABCDEF"[temp & 0x0F];
        }
    }

    // format string
    for(size_t i = 0; i < resl; i++) {
        if (str[i] == '%' && str[i+1] == 'X') {
            res[i] = '0';
            res[i+1] = 'x';
            i+=2;
            for (size_t j = 0; j < numl; j++) {
                res[i+j] = digits[numl-1-j];
            }
            i+=numl;
        }
        else {
            // copy char to result string
            res[i] = str[i];
        }
    }
    res[resl-1] = '\0';
    memcpy(_strtemp, res, resl);
    return _strtemp;
}

void memcpy(void *dest, const void *src, size_t count) {
    // cast the pointers to char pointers for byte-by-byte copying
    char *d = (char *)dest;
    const char *s = (const char *)src;

    // copy each byte from source to destination
    for (size_t i = 0; i < count; i++) {
        d[i] = s[i];
    }
}

void memcpyw(wchar_t *dest, wchar_t *src, size_t count) {
    for (size_t i = 0; i < count; i++) {
        // copy each wide byte from source to destination
        dest[i] = src[i];
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

void printb(const char *str) {
    for(size_t i = 0; str[i] != '\0'; i++) {
        vga_putc(str[i], VGA_STYLE_TEXT);
    }
}

void log_ok(const char *str) {
    const char *msg = "[ OK ] ";
    vga_putc(msg[0], VGA_STYLE_BRACKET);
    vga_putc(msg[1], VGA_STYLE_TEXT);
    vga_putc(msg[2], VGA_STYLE_LOG_OK);
    vga_putc(msg[3], VGA_STYLE_LOG_OK);
    vga_putc(msg[4], VGA_STYLE_TEXT);
    vga_putc(msg[5], VGA_STYLE_BRACKET);
    vga_putc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    vga_putc('\n', VGA_STYLE_TEXT);
}

void log_fail(const char *str) {
    const char *msg = "[FAIL] ";
    vga_putc(msg[0], VGA_STYLE_BRACKET);
    vga_putc(msg[1], VGA_STYLE_LOG_FAIL);
    vga_putc(msg[2], VGA_STYLE_LOG_FAIL);
    vga_putc(msg[3], VGA_STYLE_LOG_FAIL);
    vga_putc(msg[4], VGA_STYLE_LOG_FAIL);
    vga_putc(msg[5], VGA_STYLE_BRACKET);
    vga_putc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    vga_putc('\n', VGA_STYLE_TEXT);
}

void log_info(const char *str) {
    const char *msg = "[INFO] ";
    vga_putc(msg[0], VGA_STYLE_BRACKET);
    vga_putc(msg[1], VGA_STYLE_LOG_INFO);
    vga_putc(msg[2], VGA_STYLE_LOG_INFO);
    vga_putc(msg[3], VGA_STYLE_LOG_INFO);
    vga_putc(msg[4], VGA_STYLE_LOG_INFO);
    vga_putc(msg[5], VGA_STYLE_BRACKET);
    vga_putc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    vga_putc('\n', VGA_STYLE_TEXT);
}

void log_warn(const char *str) {
    const char *msg = "[WARN] ";
    vga_putc(msg[0], VGA_STYLE_BRACKET);
    vga_putc(msg[1], VGA_STYLE_LOG_WARN);
    vga_putc(msg[2], VGA_STYLE_LOG_WARN);
    vga_putc(msg[3], VGA_STYLE_LOG_WARN);
    vga_putc(msg[4], VGA_STYLE_LOG_WARN);
    vga_putc(msg[5], VGA_STYLE_BRACKET);
    vga_putc(msg[6], VGA_STYLE_TEXT);
    printb(str);
    vga_putc('\n', VGA_STYLE_TEXT);
}

void dump_hex(uint8_t *data, size_t count) {
    // TODO: print address in hex
    log_info(strfX("Dumping data at address: %X", (uint32_t)data));
    for (size_t i = 0; i < count; i++) {
        if (i % 16 == 0 && i != 0) {
            vga_putc('\n', VGA_STYLE_TEXT);
        }
        vga_putc("0123456789ABCDEF"[data[i] >> 4], VGA_STYLE_TEXT);
        vga_putc("0123456789ABCDEF"[data[i] & 0x0F], VGA_STYLE_TEXT);
        vga_putc(' ', VGA_STYLE_TEXT);
    }
    vga_putc('\n', VGA_STYLE_TEXT);
}