#include "vga.h"

const char *msg = "Hello, World!";

int bmain(void) {
    vga_init();
    bprintf(msg);
    return 0;
}
