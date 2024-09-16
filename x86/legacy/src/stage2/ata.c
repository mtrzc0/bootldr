#include "ata.h"

#include <stdbool.h>

#include "sys.h"
#include "vga.h"

static ata_io_base_t ata_io_base_prim = { 0 };
static ata_ports_map_t ata_ports_map = { 0 };

void ata_init(void) {
    // detect port addresses
    ata_io_detect_ports(&ata_ports_map);
    // check if drive is connected aka floating bus
    ata_io_detect_connected_drives(&ata_io_base_prim);


    // delay 400 ns for BSY to be set
    ata_io_drive_polling(&ata_io_base_prim);
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

void ata_io_read_err_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(ERR_AND_FEATS);
    ports->err_and_feats_reg = inb(port);
}

void ata_io_dump_err_reg(ata_io_base_t *ports) {
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
                break;
        }
    }
}

void ata_io_write_feats_reg(uint8_t features) {
    const uint16_t port = ata_io_phy_addr(ERR_AND_FEATS);
    outb(port, features);
}

void ata_io_read_sec_count_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(SEC_COUNT);
    ports->sec_count_reg = inb(port);
}

void ata_io_write_sec_count_reg(uint8_t count) {
    const uint16_t port = ata_io_phy_addr(SEC_COUNT);
    outb(port, count);
}

void ata_io_read_stat_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(STAT_AND_CMD);
    ports->stat_and_cmd_reg = inb(port);
}

void ata_io_dump_stat_reg(ata_io_base_t *ports) {
    for (size_t i = 0; i < 8; i++) {
        switch (ports->stat_and_cmd_reg & 1 << i) {
            case ERR:
                log_fail("Error");
                break;
            case IDX:
                log_fail("Index bit is NOT 0");
                break;
            case CORR:
                log_fail("Corrected data bit is NOT 0");
                break;
            case DRQ:
                log_info("Data to transfer ready");
                break;
            case SRV:
                log_info("Overlapped Mode Service Request");
                break;
            case DF:
                log_fail("Device fault");
                break;
            case RDY:
                log_info("Drive is ready");
                break;
            case BSY:
                log_info("Drive is busy");
                break;
            default:
                break;
        }
    }
}

void ata_io_read_LBA_regs(ata_io_base_t *ports) {
    uint16_t port = ata_io_phy_addr(LBA_LO);
    ports->LBAlo_reg = inb(port);
    port = ata_io_phy_addr(LBA_MID);
    ports->LBAmid_reg = inb(port);
    port = ata_io_phy_addr(LBA_HI);
    ports->LBAhi_reg = inb(port);
}

void ata_io_write_LBA_regs(uint32_t LBA28) {
    uint16_t port = ata_io_phy_addr(LBA_LO);
    outb(port, LBA28);
    port = ata_io_phy_addr(LBA_MID);
    outb(port, LBA28 >> 8);
    port = ata_io_phy_addr(LBA_HI);
    outb(port, LBA28 >> 16);
}

void ata_io_read_drv_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(DRV);
    ports->drive_head_reg = inb(port);
}

void ata_io_write_drv_reg(uint8_t drive) {
    const uint16_t port = ata_io_phy_addr(DRV);
    outb(port, drive);
}

void ata_io_write_cmd_reg(uint8_t cmd) {
    const uint16_t port = ata_io_phy_addr(STAT_AND_CMD);
    outb(port, cmd);
}

void ata_io_read_alt_stat_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(ALT_STAT_AND_DEV_CTRL);
    ports->stat_and_cmd_reg = inb(port);
}

void ata_io_write_dev_ctrl_reg(uint8_t cmd) {
    const uint16_t port = ata_io_phy_addr(ALT_STAT_AND_DEV_CTRL);
    outb(port, cmd);
}

void ata_io_read_drv_addr_reg(ata_io_base_t *ports) {
    const uint16_t port = ata_io_phy_addr(DRV_ADDR);
    ports->drive_head_reg = inb(port);
}

uint16_t ata_io_phy_addr(ata_io_base_offset_t offset) {
    const ata_ports_map_t *map = &ata_ports_map;
    return map->primary + offset;
}

void ata_io_drive_polling(ata_io_base_t *ports) {
    // Delay 400 ns for BSY to be set
    for (size_t i = 0; i < 4; i++) {
        // Reading alternate status wastes 100 ns each time
        ata_io_read_alt_stat_reg(ports);
    }

    while (1) {
        ata_io_read_stat_reg(ports);
        if ((ports->stat_and_cmd_reg & BSY) != true) {
            break;
        }
    }

    // check status
    ata_io_dump_stat_reg(ports);
    if (ports->stat_and_cmd_reg & ERR) {
        ata_io_read_err_reg(ports);
        ata_io_dump_err_reg(ports);
    } else if (ports->stat_and_cmd_reg & DF) {
        log_fail("Device fault");
    } else if ((ports->stat_and_cmd_reg & DRQ) == false) {
        log_fail("Data request is not set");
    } else {
        log_info("Polling complete");
    }
}