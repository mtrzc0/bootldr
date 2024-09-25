#include <stdbool.h>

#include "ata.h"
#include "sys.h"

// globals
static ata_channels_regs_t ata_bus[ATA_BUS_MAX] = { 0 };
static ata_dev_t ata_devs[ATA_DEVS_MAX] = { 0 };
static uint8_t ata_ident_buf[ATA_IDENT_BUF_MAX] = { 0 };

// TODO: add support for Interrupts
// volatile static uint8_t ide_irq_invoked = 0;

// TODO: add support for atapi Drives
// static uint8_t atapi_packet[12] = { ATAPI_CMD_READ, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// buffer for first sector read from disk
// static uint32_t *ata_io_base_sect = (uint32_t *) 0xFFE00;
static uint8_t ata_sect_buf[ATA_SECT_SIZE] = { 0 };

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
    log_ok("ATA driver: Init");
}

void ata_disk_read(uint16_t count) {
    // LBA of first disk sector NOT read via BIOS call
    const uint8_t ATA_BASE_LBA = 0x80;

    // FIXME: first read sector at LBA 0x7F (127)
    ata_read_sector(ATA_PRIMARY, ATA_BASE_LBA-1, NULL);

    // then begin reading the disk sectors starting from LBA 0x80
    // otherwise all data from the LBA 0x80 will be 0x00
    log_info("ATA driver: Reading disk sectors");
    for (uint16_t i = 0; i < count; i++) {
        log_info(strfd("ATA driver: Reading sector: %d", ATA_BASE_LBA + i));
        ata_read_sector(ATA_PRIMARY, ATA_BASE_LBA + i, ata_sect_buf);
    }
    log_ok("ATA driver: Disk read complete");
    dump_hex(ata_sect_buf, ATA_SECT_SIZE/2);
}

void ata_read_sector(ata_channel_t channel, uint32_t LBA, uint8_t *buf) {
    ata_drive_poll(channel);
    ata_write_reg(ATA_IO, channel, ATA_REG_HD_DEV_SEL, 0xE0 | (LBA >> 24) & 0x0F);
    ata_write_reg(ATA_IO, channel, ATA_REG_ERR_FEATS, 0x00);
    ata_write_reg(ATA_IO, channel, ATA_REG_SECT_COUNT0, 1);
    ata_write_reg(ATA_IO, channel, ATA_REG_LBA0, LBA & 0xFF);
    ata_write_reg(ATA_IO, channel, ATA_REG_LBA1, (LBA >> 8) & 0xFF);
    ata_write_reg(ATA_IO, channel, ATA_REG_LBA2, (LBA >> 16) & 0xFF);
    ata_write_reg(ATA_IO, channel, ATA_REG_CMD_STAT, ATA_CMD_READ);
    ata_drive_poll(channel);
    for (uint16_t i = 0; i < 256; i+=2) {
        uint16_t data = ata_read_reg(ATA_IO, channel, ATA_REG_DATA);
        if (buf != NULL) {
            buf[i + 1] = data & 0xFF;
            buf[i] = data >> 8;
        }
    }
    ata_drive_poll(channel);
}

void ata_detect_ports(const uint32_t BAR0, const uint32_t BAR1, const uint32_t BAR2, const uint32_t BAR3, const uint32_t BAR4) {
    (void) BAR4;
    if (BAR0 == ATA_COMPAT_BAR0 && BAR1 == ATA_COMPAT_BAR1 &&
        BAR2 == ATA_COMPAT_BAR2 && BAR3 == ATA_COMPAT_BAR3) {
        log_info("ATA driver: Using legacy PCI disk controller ports");
    } else {
        log_warn("ATA driver: Using custom PCI disk controller ports");
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
    log_info("ATA driver: IRQs disabled");
}

void ata_check_float_bus(void) {
    uint8_t status = 0, channel_count = 0, channel = 0;

    // prepare the IO ports by reading them before writing to them
    // detect possible devices on the bus
    while (channel < ATA_CHANNELS_COUNT) {
        status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);
        if (status == ATA_FLOATING_BUS) {
            log_warn(channel ? "ATA driver: Secondary channel bus might be floating" :
                               "ATA driver: Primary channel bus might be floating");
        } else {
            channel_count++;
        }
        channel++;
    }

    // tell how many POSSIBLE disk devices were detected
    log_info(strfd("ATA driver: Number of reserved bus channels: %d", channel_count));
}

void ata_detect_devices(void) {
    uint8_t status = 0, dev_count = 0, type = 0;
    bool reserved = false, err = false;

    // gather information about the drives using IDENTIFY command
    for (uint8_t channel = 0; channel < ATA_CHANNELS_COUNT; channel++) {
        log_info(channel ? "ATA driver: Detecting ATA devices on SECONDARY channel" :
                           "ATA driver: Detecting ATA devices on PRIMARY channel");
        for (uint8_t drive = 0; drive < ATA_DRIVE_COUNT; drive++) {
            log_info(drive ? "ATA driver: Detecting slave drive" :
                             "ATA driver: Detecting master drive");
            // 1. select a target drive by sending bytes to the drive select port
            //    + 0xA0 for the master drive
            //    + 0xB0 for the slave drive
            ata_write_reg(ATA_IO, channel, ATA_REG_HD_DEV_SEL, 0xA0 | drive << 4);
            ata_delay(channel, 400);

            // detect slave drive
            // this is important because master drive is answering for both if there is no slave
            // test this with dummy values and check if they are read back
            if (drive) {
                ata_write_reg(ATA_IO, channel, ATA_REG_LBA1, 0x71);
                ata_write_reg(ATA_IO, channel, ATA_REG_LBA2, 0x37);
                const uint8_t cl = ata_read_reg(ATA_IO, channel, ATA_REG_LBA1);
                const uint8_t ch = ata_read_reg(ATA_IO, channel, ATA_REG_LBA2);
                if (cl != 0x71 || ch != 0x37) {
                    log_warn("ATA driver: No slave device detected");
                    continue;
                }
            }

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
                log_fail("ATA driver: Device not detected");
                reserved = false;
                continue;
            }

            // poll the drive
            if (ata_drive_poll(channel) == ATA_NO_DEVICE) {
                // device is not ATA
                log_warn("ATA driver: ATA device not detected trying ATAPI");
                err = true;
            }

            if (err) {
                // check device type by reading "signature"
                // cylinder low and high bytes
                const uint8_t cl = ata_read_reg(ATA_IO, channel, ATA_REG_LBA1);
                const uint8_t ch = ata_read_reg(ATA_IO, channel, ATA_REG_LBA2);

                // information about the device type
                reserved = true;
                if (cl == 0x14 && ch == 0xEB) {
                    log_ok("ATA driver: ATAPI device detected");
                    type = ATA_DEV_ATAPI;
                } else if (cl == 0x69 && ch == 0x96) {
                    log_ok("ATA driver: ATAPI device detected");
                    type = ATA_DEV_ATAPI;
                } else if (cl == 0x3c && ch == 0xc3) {
                    log_ok("ATA driver: SATA device detected");
                    type = ATA_DEV_ATAPI;
                } else {
                    log_fail("ATA driver: Unknown device type");
                    reserved = false;
                    continue;
                }

                // send the IDENTIFY PACKET command to the command port
                ata_write_reg(ATA_IO, channel, ATA_REG_CMD_STAT, ATA_CMD_IDENTIFY_PACKET);
                ata_delay(channel, 1);
            } else {
                log_ok("ATA driver: PATA device detected");
                reserved = true;
                type = ATA_DEV_ATA;
            }

            // read the IDENTIFY data
            uint16_t const port = ata_addr(ATA_IO, channel, ATA_REG_DATA);
            rep_insw(port, ata_ident_buf, 255);
            ata_ident_buf[511] = '\0';

            ata_devs[dev_count].reserved = reserved;
            ata_devs[dev_count].type = type;
            ata_devs[dev_count].drive = drive;
            ata_devs[dev_count].channel = channel;
            ata_devs[dev_count].signature = ata_ident_buf[ATA_IDENT_DEVICETYPE];
            ata_devs[dev_count].features = ata_ident_buf[ATA_IDENT_CAPABILITIES];
            ata_devs[dev_count].command_sets = ata_ident_buf[ATA_IDENT_COMMANDSETS];

            // get 32-bit size of the device
            if (ata_devs[dev_count].command_sets & 1 << 26) {
                for (size_t i = 0; i < 4; i++) {
                    ata_devs[dev_count].size |= ata_ident_buf[ATA_IDENT_MAX_LBA_EXT + i] << i*8;
                }
            } else {
                for (size_t i = 0; i < 4; i++) {
                    ata_devs[dev_count].size |= ata_ident_buf[ATA_IDENT_MAX_LBA_EXT + i] << i*8;
                }
            }
            // copy the model name
            for (size_t i = 0; i < 40; i += 2) {
                ata_devs[dev_count].model[i] = ata_ident_buf[ATA_IDENT_MODEL + i + 1];
                ata_devs[dev_count].model[i + 1] = ata_ident_buf[ATA_IDENT_MODEL + i];
            }

            // dump the device information
            ata_dump_drv_info(dev_count);
            dev_count++;
        }
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
    if (reg & ATA_ER_AMNF) log_fail("ATA driver: Address mark not found");
    if (reg & ATA_ER_TKZNF) log_fail("ATA driver: Track 0 not found");
    if (reg & ATA_ER_ABRT) log_fail("ATA driver: Aborted command");
    if (reg & ATA_ER_MCR) log_fail("ATA driver: Media change request");
    if (reg & ATA_ER_IDNF) log_fail("ATA driver: ID not found");
    if (reg & ATA_ER_MC) log_fail("ATA driver: Media changed");
    if (reg & ATA_ER_UNC) log_fail("ATA driver: Uncorrectable data error");
    if (reg & ATA_ER_BBK) log_fail("ATA driver: Bad block detected");
}

void ata_dump_stat_reg(ata_channel_t channel) {
    const uint8_t status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);

    if (status & ATA_SR_ERR) {
        log_fail("ATA driver: Error bit is NOT 0");
        ata_dump_err_reg(channel);
    }
    if (status & ATA_SR_IDX) {
        log_fail("ATA driver: Index bit is NOT 0");
    }
    if (status & ATA_SR_CORR) {
        log_fail("ATA driver: Corrected data bit is NOT 0");
    }
    if (status & ATA_SR_DRQ) {
        log_info("ATA driver: Data to transfer ready");
    }
    if (status & ATA_SR_SRV) {
        log_warn("ATA driver: Overlapped Mode Service Request");
    }
    if (status & ATA_SR_DF) {
        log_fail("ATA driver: Device fault");
    }
    if (status & ATA_SR_RDY) {
        log_info("ATA driver: Drive is ready");
    }
    if (status & ATA_SR_BSY) {
        log_warn("ATA driver: Drive is busy");
    }
}

void ata_dump_drv_info(uint8_t index) {
    log_info(strfs("ATA driver: Device type: %s", ata_devs[index].type ? "ATAPI" : "PATA"));
    log_info(strfs("ATA driver: Drive type: %s", ata_devs[index].drive ? "Slave" : "Master"));
    log_info(strfs("ATA driver: Channel: %s", ata_devs[index].channel ? "Secondary" : "Primary"));
    log_info(strfX("ATA driver: Signature: %X", ata_devs[index].signature));
    log_info(strfs("ATA driver: Features: %s", ata_devs[index].features != 0 ? "Available" : "Not known"));
    log_info(strfX("ATA driver: Command sets: %X", ata_devs[index].command_sets));
    log_info(strfd("ATA driver: Device size in bytes: %d", ata_devs[index].size*512));
    log_info(strfs("ATA driver: Model: %s", (const char *)ata_devs[index].model));
}

void ata_srst(ata_channel_t channel) {
    ata_write_reg(ATA_IO, channel, ATA_REG_CTRL_ALT_STAT, ATA_CR_SRST);
    ata_delay(channel, 400);
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
void ata_delay(ata_channel_t channel, uint32_t ns) {
    if (ns < 100) {
        return;
    }
    for (size_t i = 0; i < ns/100; i++) {
        // Reading alternate status wastes ~ 100 ns each time
        ata_read_reg(ATA_CTRL, channel, ATA_REG_CTRL_ALT_STAT);
    }
}

int8_t ata_drive_poll(ata_channel_t channel) {
    uint8_t status = 0;
    ata_delay(channel, 400);
    do {
        status = ata_read_reg(ATA_IO, channel, ATA_REG_CMD_STAT);
        if (status & ATA_SR_ERR || status & ATA_SR_DF) {
            return ATA_NO_DEVICE;
        }
    } while (status & ATA_SR_BSY && !(status & ATA_SR_DRQ));
    return 0;
}