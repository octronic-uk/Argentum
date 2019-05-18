#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <Drivers/PCI/PCIConstants.h>

#define PCI_MAX_HEADERS 32 

struct PCIConfigHeader
{
	uint8_t  mBus;
	uint8_t  mDevice;
	uint8_t  mFunction;
	uint8_t  mHeaderType;
	uint16_t mVendorID;
	uint16_t mDeviceID;
	uint16_t mStatus;
	uint16_t mCommand;
	uint8_t  mClassCode;
	uint8_t  mSubclassCode;
	uint8_t  mInterruptLine;
	uint8_t  mInterruptPin;
	uint8_t  mPrimaryBus;
	uint8_t  mSecondaryBus;
	uint8_t  mProgIF;
	uint32_t mBAR0;
	uint32_t mBAR1;
	uint32_t mBAR2;
	uint32_t mBAR3;
	uint32_t mBAR4;
	uint32_t mBAR5;
};
typedef struct PCIConfigHeader PCIConfigHeader;

struct PCIDriver
{
	PCIConfigHeader ConfigHeaderList[PCI_MAX_HEADERS];
	bool Debug;
};
typedef struct PCIDriver PCIDriver;

bool PCIDriver_Constructor(PCIDriver* self);

void PCIDriver_DumpDevices(PCIDriver* self);
void PCIDriver_DumpDevice(PCIDriver* self, const PCIConfigHeader* header);

uint8_t PCIDriver_CountHeaders(PCIDriver* self);
bool PCIDriver_ReadConfigHeader(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, PCIConfigHeader* header);
PCIConfigHeader* PCIDriver_GetConfigHeader(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
PCIConfigHeader* PCIDriver_GetIsaBridgeConfigHeader(PCIDriver* self);
PCIConfigHeader* PCIDriver_GetDeviceByID(PCIDriver* self, uint16_t vendor_id, uint16_t device_id);
PCIConfigHeader* PCIDriver_GetATADevice(PCIDriver* self);

uint8_t PCIDriver_IsMultifunctionDevice(PCIDriver* self, uint8_t headerType);

uint32_t PCIDriver_ReadConfig32b(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint16_t PCIDriver_ReadConfig16b(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint8_t PCIDriver_ReadConfig8b(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

uint32_t PCIDriver_DeviceReadConfig32b(PCIDriver* self, PCIConfigHeader* device, uint8_t offset);
uint16_t PCIDriver_DeviceReadConfig16b(PCIDriver* self, PCIConfigHeader* device, uint8_t offset);
uint8_t  PCIDriver_DeviceReadConfig8b(PCIDriver* self, PCIConfigHeader* device, uint8_t offset);

void PCIDriver_ConfigSetAddress(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

void PCIDriver_WriteConfig32b(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data);
void PCIDriver_WriteConfig16b(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint16_t data);
void PCIDriver_WriteConfig8b(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint8_t data);

void PCIDriver_DeviceWriteConfig32b(PCIDriver* self, PCIConfigHeader* device, uint8_t offset, uint32_t data);
void PCIDriver_DeviceWriteConfig16b(PCIDriver* self, PCIConfigHeader* device, uint8_t offset, uint16_t data);
void PCIDriver_DeviceWriteConfig8b(PCIDriver* self, PCIConfigHeader* device, uint8_t offset, uint8_t data);

uint8_t PCIDriver_GetHeaderType(PCIDriver* self, uint8_t bus , uint8_t device , uint8_t function);
uint16_t PCIDriver_GetVendorID(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCIDriver_GetDeviceID(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetClassCode(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetSubClass(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetPrimaryBus(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetSecondaryBus(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCIDriver_GetStatus(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCIDriver_GetCommand(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCIDriver_GetInterruptLine(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCIDriver_GetInterruptPin(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR0(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR1(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR2(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR3(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR4(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR5(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetProgIF(PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);