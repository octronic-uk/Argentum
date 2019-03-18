#pragma once

/*
   Lovingly inspired by
   https://forum.osdev.org/viewtopic.php?f=1&p=167798
*/

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

#define    ATA_SR_BSY      0x80
#define    ATA_SR_DRDY      0x40
#define    ATA_SR_DF      0x20
#define    ATA_SR_DSC      0x10
#define    ATA_SR_DRQ      0x08
#define    ATA_SR_CORR      0x04
#define    ATA_SR_IDX      0x02
#define    ATA_SR_ERR      0x01

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

#define ATA_ERR_AMNF  0x01
#define ATA_ERR_TKZNF 0x02
#define ATA_ERR_ABRT  0x04
#define ATA_ERR_MCR   0x08
#define ATA_ERR_IDNF  0x10
#define ATA_ERR_MC    0x20
#define ATA_ERR_UNC   0x40
#define ATA_ERR_BBK   0x80

// ATA-Commands:
#define      ATA_CMD_READ_PIO         0x20
#define      ATA_CMD_READ_PIO_EXT      0x24
#define      ATA_CMD_READ_DMA         0xC8
#define      ATA_CMD_READ_DMA_EXT      0x25
#define      ATA_CMD_WRITE_PIO         0x30
#define      ATA_CMD_WRITE_PIO_EXT      0x34
#define      ATA_CMD_WRITE_DMA         0xCA
#define      ATA_CMD_WRITE_DMA_EXT      0x35
#define      ATA_CMD_CACHE_FLUSH      0xE7
#define      ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define      ATA_CMD_PACKET            0xA0
#define       ATA_CMD_IDENTIFY_PACKET      0xA1
#define       ATA_CMD_IDENTIFY         0xEC

#define    ATA_IDENT_DEVICETYPE   0
#define    ATA_IDENT_CYLINDERS   2
#define    ATA_IDENT_HEADS      6
#define    ATA_IDENT_SECTORS      12
#define    ATA_IDENT_SERIAL   20
#define    ATA_IDENT_MODEL      54
#define    ATA_IDENT_CAPABILITIES   98
#define    ATA_IDENT_FIELDVALID   106
#define    ATA_IDENT_MAX_LBA   120
#define   ATA_IDENT_COMMANDSETS   164
#define    ATA_IDENT_MAX_LBA_EXT   200

#define      ATA_MASTER      0x00
#define      ATA_SLAVE      0x01

#define      IDE_ATA         0x00
#define      IDE_ATAPI      0x01

// ATA-ATAPI Task-File:
#define      ATA_REG_DATA      0x00
#define      ATA_REG_ERROR      0x01
#define      ATA_REG_FEATURES   0x01
#define      ATA_REG_SECCOUNT0   0x02
#define      ATA_REG_LBA0      0x03
#define      ATA_REG_LBA1      0x04
#define      ATA_REG_LBA2      0x05
#define      ATA_REG_HDDEVSEL   0x06
#define      ATA_REG_COMMAND      0x07
#define      ATA_REG_STATUS      0x07
#define      ATA_REG_SECCOUNT1   0x08
#define      ATA_REG_LBA3      0x09
#define      ATA_REG_LBA4      0x0A
#define      ATA_REG_LBA5      0x0B
#define      ATA_REG_CONTROL      0x0C
#define      ATA_REG_ALTSTATUS   0x0C
#define      ATA_REG_DEVADDRESS   0x0D

// Channels:
#define      ATA_PRIMARY      0x00
#define      ATA_SECONDARY      0x01

// Directions:
#define      ATA_READ      0x00
#define      ATA_WRITE      0x01

typedef struct 
{
   unsigned short base;  // I/O Base.
   unsigned short ctrl;  // Control Base
   unsigned short bmide; // Bus Master IDE
   unsigned char  nIEN;  // nIEN (No Interrupt);
} ATA_Channel;

typedef struct 
{
   unsigned char  reserved;    // 0 (Empty) or 1 (This Drive really exists).
   unsigned char  channel;     // 0 (Primary Channel) or 1 (Secondary Channel).
   unsigned char  drive;       // 0 (Master Drive) or 1 (Slave Drive).
   unsigned short type;        // 0: ATA, 1:ATAPI.
   unsigned short sign;       // Drive Signature
   unsigned short capabilities;// Features.
   unsigned int   commandsets; // Command Sets Supported.
   unsigned int   size;       // Size in Sectors.
   unsigned char  model[41];   // Model in string.
} ATA_IDEDevice;

void ATA_Constructor();
unsigned char ATA_IDERead(ATA_Channel* channels, unsigned char channel, unsigned char reg);
void ATA_IDEWrite(ATA_Channel* channels, unsigned char channel, unsigned char reg, unsigned char data);
unsigned char ATA_IDEPolling(ATA_Channel* channels, unsigned char channel, unsigned int advanced_check);
unsigned char ATA_IDEPrintError(ATA_Channel* channels, ATA_IDEDevice* devices, unsigned int drive, unsigned char err) ;
void ATA_IDEReadBuffer(ATA_Channel* channels, unsigned char channel, unsigned char reg, unsigned int buffer, unsigned int quads) ;
void ATA_IDEInit (unsigned char* ide_buf, ATA_IDEDevice* ide_devices, ATA_Channel* channels, 
   unsigned int BAR0, unsigned int BAR1, unsigned int BAR2, unsigned int BAR3,unsigned int BAR4);