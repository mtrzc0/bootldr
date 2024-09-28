#include "ata.h"
#include "vga.h"

int bmain(void) {
    vga_init();
    ata_init();
    ata_disk_read(2880);
    return 0;
}
