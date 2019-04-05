#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <Drivers/ATA/ATAConstants.h>
#include <Drivers/ATA/ATATypes.h>

struct Kernel;

/*
   Lovingly inspired by
   https://forum.osdev.org/viewtopic.php?f=1&p=167798
*/

struct ATADriver
{
   uint8_t Debug;
   struct Kernel* Kernel;
   struct ATA_IDEDevice IDEDevices[4];
   struct ATA_Channel Channels[2];
   uint8_t IDEBuffer[2048];
   uint8_t IDEIrqInvoked; 
   uint8_t ATAPIPacket[12]; 
   uint8_t ATAPIPackage[16];
};

bool ATADriver_Constructor(struct ATADriver* self, struct Kernel* kernel);
uint8_t ATADriver_IDERead(struct ATADriver* self, uint8_t channel, uint8_t reg);
void ATADriver_IDEWrite(struct ATADriver* self, uint8_t channel, uint8_t reg, uint8_t data);
uint8_t ATADriver_IDEPolling(struct ATADriver* self, uint8_t channel,  uint32_t advanced_check);
uint8_t ATADriver_IDEPrintError(struct ATADriver* self,  uint32_t drive, uint8_t err) ;
void ATADriver_IDEReadBuffer(struct ATADriver* self, uint8_t channel, uint8_t reg,  void* buffer,  uint32_t quads) ;
void ATADriver_IDEInit (struct ATADriver* self,  uint32_t BAR0,  uint32_t BAR1,  uint32_t BAR2,  uint32_t BAR3, uint32_t BAR4);
void ATADriver_IDEWaitForIrq(struct ATADriver* self);
void ATADriver_IDEIrq(struct ATADriver* self);
uint8_t ATADriver_IDEAtapiRead(struct ATADriver* self, uint8_t drive,  uint32_t lba, uint8_t numsects,  void* buffer); 
void ATADriver_IDEReadSectors(struct ATADriver* self, uint8_t drive, uint8_t numsects,  uint32_t lba, void* buffer);
void ATADriver_IDEWriteSectors(struct ATADriver* self, uint8_t drive, uint8_t numsects,  uint32_t lba, void* buffer);
void ATADriver_IDEAtapiEject(struct ATADriver* self, uint8_t drive);
uint8_t ATADriver_DeviceUsesLBA48(struct ATADriver* self, struct ATA_IDEDevice device);

/**
* ATA/ATAPI Read/Write Modes:
* ++++++++++++++++++++++++++++++++
*  Addressing Modes:
*  ================
*   - LBA28 Mode.
*   - LBA48 Mode.
*   - CHS.
*  Reading Modes:
*  ================
*   - PIO Modes (0 : 6) - Slower than DMA, but not a problem.
*   - Single Word DMA Modes (0, 1, 2).
*   - Double Word DMA Modes (0, 1, 2).
*   - Ultra DMA Modes (0 : 6).
*  Polling Modes:
*  ================
*   - IRQs
*   - Polling Status - Suitable for Singletasking
*
* @brief This Function reads/writes sectors from ATA-Drive.
*
* @param direction if == 0 we are reading, else we are writing.
*
* @param drive, is drive number which can be from 0 to 3.
*
* @param lba, is the LBA Address which allows us to access disks up to 2TB.
*
* @param numsects, number of sectors to be read, it is a char, as reading more 
*    than 256 sector immediately may cause the OS to hang. 
*    notice that if numsects = 0, controller will know that we want 256 sectors.
*
* @param buffer (i/o buffer)
*/

uint8_t ATADriver_IDEAccess(struct ATADriver* self, uint8_t direction, uint8_t drive, uint32_t lba, uint8_t numsects, void* buffer);
uint8_t ATADriver_IDEAtapiRead(struct ATADriver* self, uint8_t drive, uint32_t lba, uint8_t numsects, void* buffer);