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
// static uint32_t *ata_io_base_sect = (uint32_t *) 0xFFE00;

// LBA of first disk sector NOT read via BIOS call
// const static uint32_t ata_io_base_LBA = 0x10;

void ata_init(void) {
    // detect pio ports
    ata_detect_ports(ATA_COMPAT_BAR0,
                        ATA_COMPAT_BAR1,
                        ATA_COMPAT_BAR2,
                        ATA_COMPAT_BAR3,
                        ATA_COMPAT_BAR4);

    // check if drive is connected aka floating bus
    ata_detect_devices();
}

void ata_read_sectors(uint16_t count) {
    // TODO
    log_info("Finished reading sectors");
}

bool ata_read_sector(ata_channel_t channel, uint32_t LBA28, uint32_t *buff) {
    // TODO
    return false;
}

void ata_detect_ports(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4) {
    // TODO: detect ports on unstandardized addresses
    // set ports to standardized addresses
    (void) BAR4;
    ide_bus[ATA_PRIMARY].base = BAR0;
    ide_bus[ATA_PRIMARY].ctrl = BAR1;
    ide_bus[ATA_SECONDARY].base = BAR2;
    ide_bus[ATA_SECONDARY].ctrl = BAR3;
    log_ok("Detecting ATA PIO ports");
}

void ata_detect_devices(void) {
    uint8_t dev_count = 0;
    // check if any drives are connected
    // inform if bus is "floating"
    uint8_t channel = 0;
    uint16_t port = ata_addr(ATA_IO, ATA_PRIMARY, ATA_REG_CMD_STAT);
    if (inb(port) == ATA_FLOATING_BUS) {
        log_fail("Detecting drives on primary channel");
        channel += ATA_SECONDARY;
    } else {
        channel += ATA_PRIMARY;
    }

    port = ata_addr(ATA_IO, ATA_SECONDARY, ATA_REG_CMD_STAT);
    if (inb(port) == ATA_FLOATING_BUS) {
        log_fail("Detecting drives on secondary channel");
        channel += ATA_SECONDARY;
    } else {
        channel += ATA_PRIMARY;
    }

    // gather information about the drives using IDENTIFY command
    while (channel < ATA_CHANNELS_COUNT) {
        log_info("Checking next channel");
        // 1. select a target drive by sending bytes to the drive select port
        //    + 0xA0 for the master drive
        //    + 0xB0 for the slave drive
        ata_write_reg(ATA_IO, channel, ATA_REG_HD_DEV_SEL, 0xA0 | channel << 4);

        // 2. set sector count, lba0, lba1, lba2 to 0
        ata_write_reg(ATA_IO, channel, ATA_REG_SECT_COUNT0, 0x00);
        ata_write_reg(ATA_IO, channel, ATA_REG_LBA0, 0x00);
        ata_write_reg(ATA_IO, channel, ATA_REG_LBA1, 0x00);
        ata_write_reg(ATA_IO, channel, ATA_REG_LBA2, 0x00);

        // 3. send the IDENTIFY command to the command port
        ata_write_reg(ATA_IO, channel, ATA_REG_CMD_STAT, ATA_CMD_IDENTIFY);

        // 4. poll the status port until the BSY bit is cleared
        if(ata_drive_polling(channel) == false) {
            // Detect ATAPI devices
            uint8_t atapi0 = ata_read_reg(ATA_IO, channel, ATA_REG_LBA1);
            uint8_t atapi1 = ata_read_reg(ATA_IO, channel, ATA_REG_LBA2);
            if (atapi0 == 0x14 && atapi1 == 0xEB) {
                log_ok("ATAPI device detected");
            } else if (atapi0 == 0x69 && atapi1 == 0x96) {
                log_ok("ATAPI device detected");
            } else {
                log_fail("Detecting ATAPI devices");
            }
        } else {
            log_ok("ATA device detected");
        }
        ++channel;
    }
}

void ata_write_reg(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset, uint8_t data) {
    const uint16_t port = ata_addr(channel_base, channel, offset);
    outb(port, data);
}

uint16_t ata_read_reg(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset) {
    const uint16_t port = ata_addr(channel_base, channel, offset);
    return inw(port);
}

void ata_dump_err_reg(ata_channel_t channel) {
    const uint16_t reg = ata_read_reg(ATA_IO, channel, ATA_REG_ERR_FEATS);
    if (reg & ATA_ER_AMNF) log_fail("Address mark not found");
    if (reg & ATA_ER_TKZNF) log_fail("Track 0 not found");
    if (reg & ATA_ER_ABRT) log_fail("Aborted command");
    if (reg & ATA_ER_MCR) log_fail("Media change request");
    if (reg & ATA_ER_IDNF) log_fail("ID not found");
    if (reg & ATA_ER_MC) log_fail("Media changed");
    if (reg & ATA_ER_UNC) log_fail("Uncorrectable data error");
    if (reg & ATA_ER_BBK) log_fail("Bad block detected");
}

void ata_dump_stat_reg(ata_channel_t channel) {
    const uint16_t reg = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);

    if (reg & ATA_SR_ERR) {
        log_fail("Error bit is NOT 0");
        ata_dump_err_reg(channel);
    }
    if (reg & ATA_SR_IDX) {
        log_fail("Index bit is NOT 0");
    }
    if (reg & ATA_SR_CORR) {
        log_fail("Corrected data bit is NOT 0");
    }
    if (reg & ATA_SR_DRQ) {
        log_info("Data to transfer ready");
    }
    if (reg & ATA_SR_SRV) {
        log_info("Overlapped Mode Service Request");
    }
    if (reg & ATA_SR_DF) {
        log_fail("Device fault");
    }
    if (reg & ATA_SR_RDY) {
        log_info("Drive is ready");
    }
    if (reg & ATA_SR_BSY) {
        log_info("Drive is busy");
    }

    if (!(reg & EMPTY)) {
        log_fail("ATA device may not exist, try to detect ATAPI device");
    }
}

uint16_t ata_addr(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset) {
    switch (channel_base) {
        case ATA_IO:
            return ide_bus[channel].base + offset;
        case ATA_CTRL:
            return ide_bus[channel].ctrl + offset;
    }
    return 0;
}

void ata_400ns_delay(ata_channel_t channel) {
    // Delay 400 ns for BSY to be set
    for (size_t i = 0; i < 4; i++) {
        // Reading alternate status wastes 100 ns each time
        ata_read_reg(ATA_CTRL, channel, ATA_REG_CTRL_ALT_STAT);
    }
}

bool ata_drive_polling(ata_channel_t channel) {
    uint16_t reg;
    ata_400ns_delay(channel);

    // wait for ATA_SR_BSY bit to be cleared
    do {
        reg = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);
    } while (reg & ATA_SR_BSY);

    // check if ERR bit is set
    // check if DF bit is set
    // check if DRQ bit is set
    // check if device exists
    if (reg & ATA_SR_ERR || reg & ATA_SR_DF || !(reg & ATA_SR_DRQ)|| reg & EMPTY) {
        ata_dump_stat_reg(channel);
        return false;
    }

    // no errors
    return true;
}