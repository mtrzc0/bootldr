#include "ata.h"
#include "sys.h"
#include "vga.h"

static ata_io_base_t ata_io_base_prim = { 0 };
static ata_ports_map_t ata_ports_map = { 0 };

void ata_init(void) {
    // detect port addresses
    ata_io_detect_ports(&ata_ports_map);
    // check if drive is connected aka floating bus
    ata_io_detect_connected_drives(&ata_io_base_prim);
}

void ata_io_detect_ports(ata_ports_map_t *map) {
    // TODO: detect ports on unstandardized addresses
    // set ports to standardized addresses
    map->primary = ATA_STD_PRIMARY;
    map->ctrl = ATA_STD_CTRL;
    log_ok("Detecting ATA PIO ports");
}

void ata_io_detect_connected_drives(ata_io_base_t *ports) {
    ata_io_read_stat_reg(ports);
    if (ports->stat_and_cmd_reg == ATA_FLOATING_BUS) {
        log_fail("Bus is floating");
    } else {
        log_ok("Drive is connected");
    }
}

void ata_io_read_data_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(DATA_REG);
    ports->data_reg = inw(port);
}

void ata_io_write_data_reg(uint16_t data) {
    const uint16_t port = ata_io_phy_addr(DATA_REG);
    outw(port, data);
}

void ata_io_read_stat_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(STAT_AND_CMD);
    ports->stat_and_cmd_reg = inb(port);
}

void ata_io_read_err_reg(ata_io_base_t *ports) {
    for (size_t i = 0; i < 8; i++) {
        switch (ports->err_and_feats_reg & 1 << i) {
            case AMNF:
                log_fail("Address mark not found");
                break;
            case TKZNF:
                log_fail("Track 0 not found");
                break;
            case ABRT:
                log_fail("Aborted command");
                break;
            case MCR:
                log_fail("Media change request");
                break;
            case IDNF:
                log_fail("ID not found");
                break;
            case MC:
                log_fail("Media changed");
                break;
            case UNC:
                log_fail("Uncorrectable data error");
                break;
            case BBK:
                log_fail("Bad block detected");
                break;
            default:
                log_ok("Reading disk");
                break;
        }
    }
}

uint16_t ata_io_phy_addr(ata_io_base_offset_t offset) {
    const ata_ports_map_t *map = &ata_ports_map;
    return map->primary + offset;
}