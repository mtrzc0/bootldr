#include "ata.h"

#include <stdbool.h>

#include "sys.h"
#include "vga.h"

// globals
static ide_channels_regs_t ide_bus[2] = { 0 };
static ide_device_t ide_devices[4] = { 0 };
static uint8_t ide_ident_buff[2048] = { 0 };

// TODO: add support for Interrupts
// volatile static uint8_t ide_irq_invoked = 0;

// TODO: add support for atapi Drives
// static uint8_t atapi_packet[12] = { ATAPI_CMD_READ, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// Buffer for first sector read from disk
static uint32_t *ata_io_base_sect = (uint32_t *) 0xFFE00;

// LBA of first disk sector NOT read via BIOS call
const static uint32_t ata_io_base_LBA = 0x10;

void ide_init(void) {
    // check if drive is connected aka floating bus
    ata_io_detect_storage_devs();
    // detect pio ports
    ata_io_detect_ports(ATA_COMPAT_BAR0,
                        ATA_COMPAT_BAR1,
                        ATA_COMPAT_BAR2,
                        ATA_COMPAT_BAR3,
                        ATA_COMPAT_BAR4);
}

void ata_io_read_sectors(uint16_t count) {
    // TODO
    log_info("Finished reading sectors");
}

bool ata_io_read_sector(ata_channels_t channel, uint32_t LBA28, uint32_t *buff) {
    // TODO
    return false;
}

void ata_io_detect_ports(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4) {
    // TODO: detect ports on unstandardized addresses
    // set ports to standardized addresses
    (void) BAR4;
    ide_bus[ATA_PRIMARY].base = BAR0;
    ide_bus[ATA_PRIMARY].ctrl = BAR1;
    ide_bus[ATA_SECONDARY].base = BAR2;
    ide_bus[ATA_SECONDARY].ctrl = BAR3;
    log_ok("Detecting ATA PIO ports");
}

void ata_io_detect_storage_devs(void) {
    // check if any drives are connected
    // inform if bus is "floating"
    uint16_t pio = ata_io_addr(ATA_PRIMARY, ATA_REG_CMD_STAT);
    if (inb(pio) == ATA_FLOATING_BUS) {
        log_fail("Primary channel floating bus");
    }

    pio = ata_io_addr(ATA_SECONDARY, ATA_REG_CMD_STAT);
    if (inb(pio) == ATA_FLOATING_BUS) {
        log_fail("Secondary channel floating bus");
    }
    // gather information about the drives using IDENTIFY command
    // 1. select a target drive by sending bytes to the drive select port::
    //    + 0xA0 for the master drive
    //    + 0xB0 for the slave drive
    // 2. set sector count, lba0, lba1, lba2 to 0
    // 3. send the IDENTIFY command to the command port
    // 4. poll the status port until the BSY bit is cleared

    log_ok("Drives on the ATA IO bus detected");
}

void ata_io_write_reg(ata_channels_t channel, ata_io_base_t port, uint8_t data) {
    const uint16_t pio = ata_io_addr(channel, port);
    outb(pio, data);
}

uint16_t ata_io_read_reg(ata_channels_t channel, ata_io_base_t offset) {
    const uint16_t port = ata_io_addr(channel, offset);
    return inw(port);
}

void ata_ctrl_write_reg(ata_channels_t channel, ata_ctrl_base_t port, uint8_t data) {
    const uint16_t pio = ata_ctrl_addr(channel, port);
    outb(pio, data);
}

uint16_t ata_ctrl_read_reg(ata_channels_t channel, ata_ctrl_base_t offset) {
    const uint16_t port = ata_ctrl_addr(channel, offset);
    return inw(port);
}

void ata_io_dump_err_reg(ata_channels_t channel) {
    uint16_t reg = ata_io_read_reg(channel, ATA_REG_ERR_FEATS);
    for (size_t i = 0; i < 8; i++) {
        switch (reg & 1 << i) {
            case ATA_ER_AMNF:
                log_fail("Address mark not found");
                break;
            case ATA_ER_TKZNF:
                log_fail("Track 0 not found");
                break;
            case ATA_ER_ABRT:
                log_fail("Aborted command");
                break;
            case ATA_ER_MCR:
                log_fail("Media change request");
                break;
            case ATA_ER_IDNF:
                log_fail("ID not found");
                break;
            case ATA_ER_MC:
                log_fail("Media changed");
                break;
            case ATA_ER_UNC:
                log_fail("Uncorrectable data error");
                break;
            case ATA_ER_BBK:
                log_fail("Bad block detected");
                break;
            default:
                break;
        }
    }
}

void ata_io_dump_stat_reg(ata_channels_t channel) {
    uint16_t reg = ata_io_read_reg(channel, ATA_REG_CMD_STAT);
    for (size_t i = 0; i < 8; i++) {
        switch (reg & 1 << i) {
            case ATA_SR_ERR:
                log_fail("Disk error bit is NOT 0");
                break;
            case ATA_SR_IDX:
                log_fail("Index bit is NOT 0");
                break;
            case ATA_SR_CORR:
                log_fail("Corrected data bit is NOT 0");
                break;
            case ATA_SR_DRQ:
                log_info("Data to transfer ready");
                break;
            case ATA_SR_SRV:
                log_info("Overlapped Mode Service Request");
                break;
            case ATA_SR_DF:
                log_fail("Device fault");
                break;
            case ATA_SR_RDY:
                log_info("Drive is ready");
                break;
            case ATA_SR_BSY:
                log_info("Drive is busy");
                break;
            default:
                break;
        }
    }
}

uint16_t ata_io_addr(ata_channels_t channel, uint32_t offset) {
    return ide_bus[channel].base + offset;
}

uint16_t ata_ctrl_addr(ata_channels_t channel, uint32_t offset) {
    return ide_bus[channel].ctrl + offset;
}

void ata_io_400ns_delay(ata_channels_t channel) {
    // Delay 400 ns for BSY to be set
    for (size_t i = 0; i < 4; i++) {
        // Reading alternate status wastes 100 ns each time
        ata_ctrl_read_reg(channel, ATA_REG_CTRL_ALT_STAT);
    }
}

bool ata_io_drive_polling(ata_channels_t channel) {
    ata_io_400ns_delay(channel);
    uint16_t reg;

    // wait for ATA_SR_BSY bit to be cleared
    do {
        reg = ata_io_read_reg(channel, ATA_REG_CMD_STAT);
    } while (reg & ATA_SR_BSY);

    // check if drive is ATA (this is only used while using IDENTIFY command)
    if (ata_io_read_reg(channel, ATA_REG_CMD_STAT) == ATA_CMD_IDENTIFY) {
        if (ata_io_read_reg(channel, ATA_REG_LBA1) || ata_io_read_reg(channel, ATA_REG_LBA2)) {
            log_fail("Drive is not ATA");
            return false;
        }
    }

    // check errors if ERR bit is set
    if (reg & ATA_SR_ERR) {
        log_fail("Device set error bits");
        return false;
    }

    // check if device fault
    if (reg & ATA_SR_DF) {
        log_fail("Device fault");
        return false;
    }

    // check if DRQ bit is properly set
    if ((reg & ATA_SR_DRQ) == false) {
        log_fail("Data request is NOT set");
        return false;
    }

    // no errors
    return true;
}
