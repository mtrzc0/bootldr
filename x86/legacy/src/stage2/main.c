#include "ata.h"
#include "vga.h"

int bmain(void) {
    vga_init();
    ata_init();
    ata_disk_read();
    exec_kernel((void *)0x100000);
    return 0;
}
