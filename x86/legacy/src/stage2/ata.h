#ifndef ATA_H
#define ATA_H

#include "sys.h"

#define ATA_FLOATING_BUS 0xFF
#define ATA_STD_PRIMARY 0x1F0
#define ATA_STD_CTRL 0x3F6

typedef struct {
    uint16_t data_reg;
    uint8_t err_and_feats_reg;
    uint8_t sec_count_reg;
    uint8_t LBAlo_reg;
    uint8_t LBAmid_reg;
    uint8_t LBAhi_reg;
    uint8_t drive_head_reg;
    uint8_t stat_and_cmd_reg;
} ata_io_base_t;

typedef enum {
    DATA_REG,
    ERR_AND_FEATS,
    SEC_COUNT,
    LBA_LO,
    LBA_MID,
    LBA_HI,
    DRV,
    STAT_AND_CMD,
} ata_io_base_offset_t;

/**
 * @brief Reads the data register from the ATA interface.
 *
 * This function reads the data register from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the data register.
 */
void ata_io_read_data_reg(ata_io_base_t *ports);

/**
 * @brief Writes data to the ATA data register.
 *
 * This function writes the given 16-bit data to the ATA data register.
 *
 * @param data The 16-bit data to write to the data register.
 */
void ata_io_write_data_reg(uint16_t data);

/**
 * @brief Reads the error register from the ATA interface.
 *
 * This function reads the error register from the provided ATA I/O base structure
 * and logs the corresponding error messages.
 *
 * @param ports Pointer to the ATA I/O base structure containing the error register.
 */
void ata_io_read_err_reg(ata_io_base_t *ports);

/**
 * @brief Writes features to the ATA features register.
 *
 * This function writes the given 8-bit features to the ATA features register.
 *
 * @param features The 8-bit features to write to the features register.
 */
void ata_io_write_feats_reg(uint8_t features);

/**
 * @brief Reads the sector count register from the ATA interface.
 *
 * This function reads the sector count register from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the sector count register.
 */
void ata_io_read_sec_count_reg(ata_io_base_t *ports);

/**
 * @brief Writes the sector count to the ATA sector count register.
 *
 * This function writes the given 8-bit sector count to the ATA sector count register.
 *
 * @param sector_count The 8-bit sector count to write to the sector count register.
 */
void ata_io_write_sec_count_reg(uint8_t sector_count);

/**
 * @brief Reads the LBA (Logical Block Addressing) registers from the ATA interface.
 *
 * This function reads the LBA low, mid, and high registers from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the LBA registers.
 */
void ata_io_read_LBA_regs(ata_io_base_t *ports);

/**
 * @brief Writes the LBA (Logical Block Addressing) value to the ATA LBA registers.
 *
 * This function writes the given "28-bit" LBA value to the LBA low, mid, and high registers.
 *
 * @param LBA28 The 28-bit LBA value to write to the LBA registers.
 */
void ata_io_write_LBA_regs(uint32_t LBA28);

/**
 * @brief Reads the drive register from the ATA interface.
 *
 * This function reads the drive register from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the drive register.
 */
void ata_io_read_drv_reg(ata_io_base_t *ports);

/**
 * @brief Writes the drive value to the ATA drive register.
 *
 * This function writes the given 8-bit drive value to the ATA drive register.
 *
 * @param drive The 8-bit drive value to write to the drive register.
 */
void ata_io_write_drv_reg(uint8_t drive);

/**
 * @brief Reads the status register from the ATA interface.
 *
 * This function reads the status register from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the status register.
 */
void ata_io_read_stat_reg(ata_io_base_t *ports);

/**
 * @brief Writes a command to the ATA command register.
 *
 * This function writes the given 8-bit command to the ATA command register.
 *
 * @param cmd The 8-bit command to write to the command register.
 */
void ata_io_write_cmd_reg(uint8_t cmd);

typedef enum {
    AMNF = 0x01,    // Address mark not found
    TKZNF = 0x02,   // Track 0 not found
    ABRT = 0x04,    // Aborted command
    MCR = 0x08,     // Media Change Request
    IDNF = 0x10,    // ID mark not found
    MC = 0x20,      // Media changed
    UNC = 0x40,     // Uncorrectable data error
    BBK = 0x80,     // Bad block detected
} err_reg_bitmask_t ;

typedef enum {
    BLK_NUM = 0x00, // Block number in LBA mode
    DRV_NUM = 0x10, // Drive number
    LBA = 0x40,     // LBA mode
} drv_reg_bitmask_t;

typedef enum {
    ERR = 0x01,     // Error
    IDX = 0x02,     // Index. Always set to 0
    CORR = 0x04,    // Corrected Data. Always set to 0
    DRQ = 0x08,     // Data Request
    SRV = 0x10,     // Overlapped Mode Service Request
    DF = 0x20,      // Drive Fault Error
    RDY = 0x40,     // Indicates the drive is ready
    BSY = 0x80,     // Indicates the drive is preparing to send/receive data
} stat_reg_bitmask_t;

typedef struct {
    uint8_t alt_stat_and_dev_ctrl_reg;  // "Copy" of the status register
    uint8_t drive_addr_reg;             //
} ata_ctrl_base_t;

typedef enum {
    ALT_STAT_AND_DEV_CTRL,
    DRV_ADDR,
} ata_ctrl_base_offset_t ;

/**
 * @brief Reads the alternate status register from the ATA interface.
 *
 * This function reads the alternate status register from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the alternate status register.
 */
void ata_io_read_alt_stat_reg(ata_io_base_t *ports);;

/**
 * @brief Writes a command to the ATA device control register.
 *
 * This function writes the given 8-bit command to the ATA device control register.
 *
 * @param cmd The 8-bit command to write to the device control register.
 */
void ata_io_write_dev_ctrl_reg(uint8_t cmd);

/**
 * @brief Reads the drive address register from the ATA interface.
 *
 * This function reads the drive address register from the provided ATA I/O base structure.
 *
 * @param ports Pointer to the ATA I/O base structure containing the drive address register.
 */
void ata_io_read_drv_addr_reg(ata_io_base_t *ports);

typedef enum {
    nIEN = 0x02,    // Disable interrupt
    SRST = 0x04,    // Software reset
    HOB = 0x80,     // High Order Byte
} dev_ctrl_reg_bitmask_t;

typedef enum {
    DS0 = 0x01,     // Drive 0
    DS1 = 0x02,     // Drive 1
    HS0 = 0x04,     // One's
    HS1 = 0x08,     // complement
    HS2 = 0x10,     // of the
    HS3 = 0x20,     // head address
    WTG = 0x40,     // Write gate; goes low while writing to the drive is in progress
    NA = 0x80,      // Reserved for compatibility with Floppy drive controllers
} drv_addr_reg_bitmask_t;

typedef struct {
    uint16_t primary;
    uint16_t ctrl;
} ata_ports_map_t;

typedef enum {
    CMD_READ = 0x20,     // Read sectors
    CMD_WRITE = 0x30,    // Write sectors
    CMD_IDENTIFY = 0xEC, // Identify drive
} cmd_reg_t;

// TODO: Device struct
// TODO: Gather all info about the device

/**
 * @brief Gets the physical address of an ATA I/O base register.
 *
 * This function returns the physical address of the specified ATA I/O base register
 * based on the provided offset.
 *
 * @param offset The offset of the ATA I/O base register.
 * @return The physical address of the specified ATA I/O base register.
 */
uint16_t ata_io_phy_addr(uint32_t offset);

/**
 * @brief Initializes the ATA interface.
 *
 * This function sets up the ATA interface by detecting available ports and connected drives.
 * It should be called during the system initialization process.
 */
void ata_init(void);

/**
 * @brief Detects available ATA ports.
 *
 * This function scans for available ATA ports and populates the provided
 * `ata_ports_map_t` structure with the detected primary and control port addresses.
 *
 * @param map Pointer to an `ata_ports_map_t` structure that will be populated
 *            with the detected port addresses.
 */
void ata_io_detect_ports(ata_ports_map_t *map);

/**
 * @brief Detects connected ATA drives.
 *
 * This function checks if a drive is connected to the ATA interface by reading
 * the status register from the provided ATA I/O base structure. It logs a message
 * indicating whether the bus is floating or a drive is connected.
 *
 * @param ports Pointer to the ATA I/O base structure containing the status register.
 */
void ata_io_detect_connected_drives(ata_io_base_t *ports);

void ata_io_drive_polling(ata_io_base_t *ports);

#endif //ATA_H