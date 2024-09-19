#include "ata.h"
#include "vga.h"

int bmain(void) {
    vga_init();
    // set up IDE controller in compatibility mode
    ata_init();
    // BIOS read first 16 sectors ~ 8K
    // FIXME: hanging
    // ata_io_read_sectors(2864);
    return 0;
}
