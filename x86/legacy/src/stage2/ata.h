#ifndef ATA_H
#define ATA_H

#include <stdbool.h>

#include "sys.h"

#define ATA_FLOATING_BUS 0xFF
#define EMPTY 0x00

// defines for BARs when PCI channel is in compatibility mode
// otherwise, we detect the BARs from the PCI configuration space
#define ATA_COMPAT_BAR0 0x1F0
#define ATA_COMPAT_BAR1 0x3F6
#define ATA_COMPAT_BAR2 0x170
#define ATA_COMPAT_BAR3 0x376
#define ATA_COMPAT_BAR4 0x000

typedef struct {
    uint16_t base;     // I/O Base
    uint16_t ctrl;     // Control Base
    uint16_t bmide;    // Bus Master IDE
    uint16_t nIEN;     // Disable interrupt
} ide_channels_regs_t;

typedef enum {
    ATA_PRIMARY,
    ATA_SECONDARY,
    ATA_CHANNELS_COUNT,
} ata_channel_t;

typedef enum {
    ATA_IO,
    ATA_CTRL
} ata_channel_base_t;

typedef enum {
    ATA_REG_DATA,
    ATA_REG_ERR_FEATS,
    ATA_REG_SECT_COUNT0,
    ATA_REG_LBA0,
    ATA_REG_LBA1,
    ATA_REG_LBA2,
    ATA_REG_HD_DEV_SEL,
    ATA_REG_CMD_STAT,
    ATA_REG_SECT_COUNT1,
    ATA_REG_LBA3,
    ATA_REG_LBA4,
    ATA_REG_LBA5,
} ata_io_base_t;

typedef enum {
    ATA_REG_CTRL_ALT_STAT,
    ATA_REG_DEV_ADDR,
} ata_ctrl_base_t;

typedef enum {
    ATA_ER_AMNF = 0x01,    // Address mark not found
    ATA_ER_TKZNF = 0x02,   // Track 0 not found
    ATA_ER_ABRT = 0x04,    // Aborted command
    ATA_ER_MCR = 0x08,     // Media Change Request
    ATA_ER_IDNF = 0x10,    // ID mark not found
    ATA_ER_MC = 0x20,      // Media changed
    ATA_ER_UNC = 0x40,     // Uncorrectable data error
    ATA_ER_BBK = 0x80,     // Bad block detected
} err_reg_bitmask_t ;

typedef enum {
    ATA_SR_ERR = 0x01,     // Indicates an error occurred
    ATA_SR_IDX = 0x02,     // Index. Always set to 0
    ATA_SR_CORR = 0x04,    // Corrected Data. Always set to 0
    ATA_SR_DRQ = 0x08,     // Data Request
    ATA_SR_SRV = 0x10,     // Overlapped Mode Service Request
    ATA_SR_DF = 0x20,      // Drive Fault Error
    ATA_SR_RDY = 0x40,     // Indicates the drive is ready
    ATA_SR_BSY = 0x80,     // Indicates the drive is preparing to send/receive data
} stat_reg_bitmask_t;

typedef enum {
    ATA_CMD_READ = 0x20,            // Read sectors
    ATA_CMD_READ_EXT = 0x24,        // Read sectors extended
    ATA_CMD_READ_DMA = 0xC8,        // Read sectors using DMA
    ATA_CMD_READ_DMA_EXT = 0x25,    // Read sectors using DMA extended
    ATA_CMD_WRITE = 0x30,           // Write sectors
    ATA_CMD_WRITE_EXT = 0x34,       // Write sectors extended
    ATA_CMD_WRITE_DMA = 0xCA,       // Write sectors using DMA
    ATA_CMD_WRITE_DMA_EXT = 0x35,   // Write sectors using DMA extended
    ATA_CMD_CACHE_FLUSH = 0xE7,     // Flush cache
    ATA_CMD_CACHE_FLUSH_EXT = 0xEA, // Flush cache extended
    ATA_CMD_PACKET = 0xA0,          // Packet command
    ATA_CMD_IDENTIFY_PACKET = 0xA1, // Identify packet device
    ATA_CMD_IDENTIFY = 0xEC,        // Identify drive
} ata_cmds_t;

typedef enum {
    ATAPI_CMD_READ = 0xA8,          // Read sectors
    ATAPI_CMD_EJECT = 0x1B,         // Eject media
} atapi_cmds_t;

typedef struct {
    uint8_t reserved;      // 0 (Empty) or 0x1 (This Drive is Present)
    uint8_t channel;       // 0 (Primary Channel) or 1 (Secondary Channel)
    uint8_t drive;         // 0 (Master Drive) or 1 (Slave Drive)
    uint16_t type;         // 0: ATA, 1:ATAPI
    uint16_t signature;    // Drive Signature
    uint16_t capabilities; // Features
    uint32_t command_sets; // Command Sets Supported
    uint32_t size;         // Size in Sectors
    uint8_t model[41];     // Model in string
} ide_device_t;

typedef enum {
    ATA_IDENT_DEVICETYPE = 0,       // Device type
    ATA_IDENT_CYLINDERS = 2,        // Number of cylinders
    ATA_IDENT_HEADS = 6,            // Number of heads
    ATA_IDENT_SECTORS = 12,         // Number of sectors per track
    ATA_IDENT_SERIAL = 20,          // Serial number
    ATA_IDENT_MODEL = 54,           // Model number
    ATA_IDENT_CAPABILITIES = 98,    // Capabilities
    ATA_IDENT_FIELDVALID = 106,     // Field validity
    ATA_IDENT_MAX_LBA = 120,        // Maximum LBA
    ATA_IDENT_COMMANDSETS = 164,    // Command sets
    ATA_IDENT_MAX_LBA_EXT = 200,    // Maximum LBA extended
} ata_dev_ident_t;

typedef enum {
    ATA_MASTER = 0x00,
    ATA_SLAVE = 0x01,
} ata_dev_t;

typedef enum {
    IDE_ATA = 0x00,
    IDE_ATAPI = 0x01,
} ide_dev_t;

uint16_t ata_read_reg(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset);

void ata_write_reg(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset, uint8_t data);

void ata_dump_err_reg(ata_channel_t channel);

void ata_dump_stat_reg(ata_channel_t channel);

uint16_t ata_addr(ata_channel_base_t channel_base, ata_channel_t channel, uint32_t offset);

void ata_init(void);

void ata_detect_ports(uint32_t BAR0, uint32_t BAR1, uint32_t BAR2, uint32_t BAR3, uint32_t BAR4);

void ata_detect_devices(void);

void ata_400ns_delay(ata_channel_t channel);

bool ata_drive_polling(ata_channel_t channel);

bool ata_read_sector(ata_channel_t channel, uint32_t LBA48, uint32_t *buff);

void ata_read_sectors(uint16_t count);

#endif //ATA_H