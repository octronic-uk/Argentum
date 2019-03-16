#pragma once

#define ATA_PRIMARY_IO_BUS        0x01F0
#define ATA_PRIMARY_CONTROL_BUS   0x03F6

#define ATA_SECONDARY_IO_BUS      0x0170
#define ATA_SECONDARY_CONTROL_BUS 0x0376

// Register Offsets
#define ATA_DATA_REG_OFFSET          0x00
#define ATA_ERROR_REG_OFFSET         0x01
#define ATA_FEATURES_REG_OFFSET      0x01
#define ATA_SECTOR_COUNT_REG_OFFSET  0x02
#define ATA_SECTOR_NUMBER_REG_OFFSET 0x03
#define ATA_CYLINDER_LOW_REG_OFFSET  0x04
#define ATA_CYLINDER_HIGH_REG_OFFSET 0x05
#define ATA_DRIVE_HEAD_REG_OFFSET    0x06
#define ATA_STATUS_REG_OFFSET        0x07
#define ATA_COMMAND_REG_OFFSET       0x07

// Control Register Offsets
#define ATA_CTRL_ALT_STATUS_REG_OFFSET  0x00
#define ATA_CTRL_DEVICE_CTRL_REG_OFFSET 0x00
#define ATA_CTRL_DRIVE_ADDR_REG_OFFSET  0x01

/* Errors
Bit Abbreviation	Function
0	AMNF	        Address mark not found.
1	TKZNF	        Track zero not found.
2	ABRT	        Aborted command.
3	MCR	            Media change request.
4	IDNF	        ID not found.
5	MC	            Media changed.
6	UNC	            Uncorrectable data error.
7	BBK	            Bad Block detected.
*/

#define ATE_ERR_AMNF  0x01
#define ATE_ERR_TKZNF 0x02
#define ATE_ERR_ABRT  0x04
#define ATE_ERR_MCR   0x08
#define ATE_ERR_IDNF  0x10
#define ATE_ERR_MC    0x20
#define ATE_ERR_UNC   0x40
#define ATE_ERR_BBK   0x80
