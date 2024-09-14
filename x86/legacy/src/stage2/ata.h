#ifndef ATA_H
#define ATA_H

#include "ctype.h"

#define ATA_FLOATING_BUS 0xFF
#define ATA_STD_PRIMARY 0x1F0
#define ATA_STD_CTRL 0x3F6

typedef struct {
    uint16_t data_reg;
    uint8_t err_and_feats_reg;
    uint8_t sector_count_reg;
    uint8_t LBAlo_reg;
    uint8_t LBAmid_reg;
    uint8_t LBAhi_reg;
    uint8_t drive_head_reg;
    uint8_t stat_and_cmd_reg;
} ata_io_base_t;

void ata_io_read_data_reg(ata_io_base_t *ports);
void ata_io_write_data_reg(uint16_t data);
void ata_io_read_err_reg(ata_io_base_t *ports);
void ata_io_write_features_reg(uint8_t data);
void ata_io_read_sector_count_reg(ata_io_base_t *ports);
void ata_io_write_sector_count_reg(uint8_t data);
void ata_io_read_LBA_regs(ata_io_base_t *ports);
void ata_io_write_LBA_regs(uint32_t LBA);
void ata_io_read_drive_head_reg(ata_io_base_t *ports);
void ata_io_write_drive_head_reg(uint8_t data);
void ata_io_read_stat_reg(ata_io_base_t *ports);
void ata_io_write_cmd_reg(uint8_t data);

typedef enum {
    DATA_REG = 0x00,
    ERR_AND_FEATS = 0x01,
    SECTOR_COUNT = 0x02,
    LBA_LO = 0x03,
    LBA_MID = 0x04,
    LBA_HI = 0x05,
    DRIVE_HEAD = 0x06,
    STAT_AND_CMD = 0x07,
} ata_io_base_offset_t;

uint16_t ata_io_phy_addr(ata_io_base_offset_t offset);

typedef struct {
    uint8_t alt_stat_and_dev_ctrl_reg;
    uint8_t drive_addr_reg;
} ata_ctrl_base_t;

enum ata_ctrl_base_offset_t {
    ALT_STAT_AND_DEV_CTRL_REG_ADDR,
    DRIVE_ADDR_REG_ADDR,
};

typedef struct {
    uint16_t primary;
    uint16_t ctrl;
} ata_ports_map_t;

enum err_reg_t {
    AMNF = 1,
    TKZNF = 2,
    ABRT = 4,
    MCR = 8,
    IDNF = 16,
    MC = 32,
    UNC = 64,
    BBK = 128,
};

void ata_init(void);
void ata_io_detect_ports(ata_ports_map_t *map);
void ata_io_detect_connected_drives(ata_io_base_t *ports);

#endif //ATA_H