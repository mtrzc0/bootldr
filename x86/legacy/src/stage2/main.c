#include "ata.h"
#include "vga.h"

int bmain(void) {
    vga_init();
    ata_init();
    return 0;
}
