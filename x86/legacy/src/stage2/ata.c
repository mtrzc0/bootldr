#include <stdbool.h>

#include "ata.h"
#include "sys.h"
#include "vga.h"

// globals
static ata_channels_regs_t ata_bus[2] = { 0 };
static ata_dev_t ata_devs[4] = { 0 };
static uint8_t *ata_buf;

// TODO: add support for Interrupts
// volatile static uint8_t ide_irq_invoked = 0;

// TODO: add support for atapi Drives
// static uint8_t atapi_packet[12] = { ATAPI_CMD_READ, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// buffer for first sector read from disk
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

    // prepare the IO ports for the IDENTIFY command by pre-checking if the bus is floating
    ata_check_float_bus();
    // tell disk not to send interrupts
    ata_disable_irqs();
    // gather information about the drives
    ata_detect_devices();
}

void ata_read_sectors(uint16_t count) {
    // TODO
}

bool ata_read_sector(ata_channel_t channel, uint32_t LBA28, uint32_t *buff) {
    // TODO
    return false;
}

void ata_detect_ports(const uint32_t BAR0, const uint32_t BAR1, const uint32_t BAR2, const uint32_t BAR3, const uint32_t BAR4) {
    (void) BAR4;
    if (BAR0 == ATA_COMPAT_BAR0 && BAR1 == ATA_COMPAT_BAR1 &&
        BAR2 == ATA_COMPAT_BAR2 && BAR3 == ATA_COMPAT_BAR3) {
        log_info("Using legacy PCI disk controller ports");
    } else {
        log_warn("Using custom PCI disk controller ports");
    }

    // set up the base and control ports for the primary and secondary channels
    ata_bus[ATA_PRIMARY  ].base  = (BAR0 & 0xFFFFFFFC) + 0x1F0 * !BAR0;
    ata_bus[ATA_PRIMARY  ].ctrl  = (BAR1 & 0xFFFFFFFC) + 0x3F6 * !BAR1;
    ata_bus[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * !BAR2;
    ata_bus[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * !BAR3;
    ata_bus[ATA_PRIMARY  ].bmide = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
    ata_bus[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE
}

void ata_disable_irqs(void) {
    // disable IRQs
    ata_write_reg(ATA_CTRL, ATA_PRIMARY, ATA_REG_CTRL_ALT_STAT, 0x02);
    ata_write_reg(ATA_CTRL, ATA_SECONDARY, ATA_REG_CTRL_ALT_STAT, 0x02);
    log_info("IRQs disabled");
}

void ata_check_float_bus(void) {
    uint8_t status = 0, channel_count = 0, channel = 0;

    // prepare the IO ports by reading them before writing to them
    // detect possible devices on the bus
    while (channel < ATA_CHANNELS_COUNT) {
        status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);
        if (status == ATA_FLOATING_BUS) {
            log_warn(!channel ? "Primary channel bus might be floating" :
                                "Secondary channel bus might be floating");
        } else {
            channel_count++;
        }
        channel++;
    }

    // tell how many POSSIBLE disk devices were detected
    log_info(strformat("Number of reserved bus channels: %d", channel_count));
}

void ata_detect_devices(void) {
    uint8_t status = 0, dev_count = 0, type = 0;
    bool reserved = false, err = false;

    // gather information about the drives using IDENTIFY command
    for (uint8_t channel = 0; channel < ATA_CHANNELS_COUNT; channel++) {
        log_info(channel ? "Detecting ATA devices on SECONDARY channel" :
                           "Detecting ATA devices on PRIMARY channel");
        for (uint8_t drive = 0; drive < ATA_DRIVE_COUNT; drive++) {
            log_info(drive ? "Detecting slave drive" :
                             "Detecting master drive");
            // 1. select a target drive by sending bytes to the drive select port
            //    + 0xA0 for the master drive
            //    + 0xB0 for the slave drive
            ata_write_reg(ATA_IO, channel, ATA_REG_HD_DEV_SEL, 0xA0 | drive << 4);
            ata_delay(channel, 1);

            // 2. set sector count, lba0, lba1, lba2 to 0
            ata_write_reg(ATA_IO, channel, ATA_REG_SECT_COUNT0, 0x00);
            ata_write_reg(ATA_IO, channel, ATA_REG_LBA0, 0x00);
            ata_write_reg(ATA_IO, channel, ATA_REG_LBA1, 0x00);
            ata_write_reg(ATA_IO, channel, ATA_REG_LBA2, 0x00);

            // 3. send the IDENTIFY command to the command port
            ata_write_reg(ATA_IO, channel, ATA_REG_CMD_STAT, ATA_CMD_IDENTIFY);

            // 4. check status register
            status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);
            if (!(status & (ATA_SR_BSY | ATA_SR_DRQ))) {
                log_fail("Device not detected");
                reserved = false;
                continue;
            }

            // + poll the drive
            if (ata_drive_poll(channel) == ATA_NO_DEVICE) {
                // device is not ATA
                err = true;
                log_warn("ATA device not detected trying ATAPI");
            }

            if (err) {
                // check device type by reading "signature" bytes
                const uint8_t dev_type0 = ata_read_reg(ATA_IO, channel, ATA_REG_LBA1);
                const uint8_t dev_type1 = ata_read_reg(ATA_IO, channel, ATA_REG_LBA2);

                // information about the device type
                reserved = true;
                if (dev_type0 == 0x14 && dev_type1 == 0xEB) {
                    log_ok("ATAPI device detected");
                    type = ATA_DEV_ATAPI;
                } else if (dev_type0 == 0x69 && dev_type1 == 0x96) {
                    log_ok("ATAPI device detected");
                    type = ATA_DEV_ATAPI;
                } else if (dev_type0 == 0x3c && dev_type1 == 0xc3) {
                    log_ok("SATA device detected");
                    type = ATA_DEV_SATA;
                } else {
                    log_fail("Unknown device type");
                    continue;
                }

                // send the IDENTIFY PACKET command to the command port
                ata_write_reg(ATA_IO, channel, ATA_REG_CMD_STAT, ATA_CMD_IDENTIFY_PACKET);
                ata_delay(channel, 1);
            } else {
                log_ok("PATA device detected");
                reserved = true;
                type = ATA_DEV_PATA;
            }
            ata_devs[dev_count].reserved = reserved;
            ata_devs[dev_count].type = type;
            ata_devs[dev_count].drive = drive;
            ata_devs[dev_count].channel = channel;
            // TODO: read the IDENTIFY data
            dev_count++;
        }
        ata_srst(channel);
    }
}

void ata_write_reg(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset, uint8_t data) {
    const uint16_t port = ata_addr(channel_base, channel, offset);
    outb(port, data);
}

uint8_t ata_read_reg(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset) {
    const uint16_t port = ata_addr(channel_base, channel, offset);
    return inb(port);
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
    const uint8_t status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);

    if (status & ATA_SR_ERR) {
        log_fail("Error bit is NOT 0");
        ata_dump_err_reg(channel);
    }
    if (status & ATA_SR_IDX) {
        log_fail("Index bit is NOT 0");
    }
    if (status & ATA_SR_CORR) {
        log_fail("Corrected data bit is NOT 0");
    }
    if (status & ATA_SR_DRQ) {
        log_info("Data to transfer ready");
    }
    if (status & ATA_SR_SRV) {
        log_warn("Overlapped Mode Service Request");
    }
    if (status & ATA_SR_DF) {
        log_fail("Device fault");
    }
    if (status & ATA_SR_RDY) {
        log_info("Drive is ready");
    }
    if (status & ATA_SR_BSY) {
        log_warn("Drive is busy");
    }

}

void ata_srst(ata_channel_t channel) {
    ata_write_reg(ATA_IO, channel, ATA_REG_CTRL_ALT_STAT, ATA_CR_SRST);
    ata_delay(channel, 1);
    ata_write_reg(ATA_IO, channel, ATA_REG_CTRL_ALT_STAT, 0x00);
}

uint16_t ata_addr(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset) {
    switch (channel_base) {
        case ATA_IO:
            return ata_bus[channel].base + offset;
        case ATA_CTRL:
            return ata_bus[channel].ctrl + offset;
    }
    return 0;
}

// TODO: add support for Interrupts and do optimization
void ata_delay(ata_channel_t channel, uint32_t ms) {
    for (size_t i = 0; i < ms*10; i++) {
        // Reading alternate status wastes ~ 100 ns each time
        ata_read_reg(ATA_CTRL, channel, ATA_REG_CTRL_ALT_STAT);
    }
}

int8_t ata_drive_poll(ata_channel_t channel) {
    uint8_t status = 0;
    ata_delay(channel, 1);
    do {
        status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);
        if (status & ATA_SR_ERR || status & ATA_SR_DF) {
            return ATA_NO_DEVICE;
        }
    } while (status & ATA_SR_BSY && !(status & ATA_SR_DRQ));
    return 0;
}