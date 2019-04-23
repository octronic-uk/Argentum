#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <Drivers/PCI/PCIConstants.h>
#include <Objects/Structures/LinkedList.h>

struct PCI_ConfigHeader
{
	uint8_t mBus;
	uint8_t mDevice;
	uint8_t mFunction;
	uint8_t  mHeaderType;
	uint16_t mVendorID;
	uint16_t mDeviceID;
	uint16_t mStatus;
	uint16_t mCommand;
	uint8_t  mClassCode;
	uint8_t  mSubclassCode;
	uint8_t  mInterruptLine;
	uint8_t  mInterruptPin;
	uint8_t mPrimaryBus;
	uint8_t mSecondaryBus;
	uint8_t mProgIF;
	uint32_t mBAR0;
	uint32_t mBAR1;
	uint32_t mBAR2;
	uint32_t mBAR3;
	uint32_t mBAR4;
	uint32_t mBAR5;
} ;


struct PCIDriver
{
	struct LinkedList ConfigHeaderList;
	bool Debug;
};

bool PCIDriver_Constructor(struct PCIDriver* self);

void PCIDriver_SetDebug(struct PCIDriver* self, uint32_t debug);
void PCIDriver_DumpDevices(struct PCIDriver* self);
void PCIDriver_DumpDevice(struct PCIDriver* self, const struct PCI_ConfigHeader* header);

struct PCI_ConfigHeader* PCIDriver_ReadConfigHeader(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func);
struct PCI_ConfigHeader* PCIDriver_GetConfigHeader(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
struct PCI_ConfigHeader* PCIDriver_GetIsaBridgeConfigHeader(struct PCIDriver* self);
struct PCI_ConfigHeader* PCIDriver_GetDeviceByID(struct PCIDriver* self, uint16_t vendor_id, uint16_t device_id);
struct PCI_ConfigHeader* PCIDriver_GetATADevice(struct PCIDriver* self);

uint8_t PCIDriver_IsMultifunctionDevice(struct PCIDriver* self, uint8_t headerType);

uint32_t PCIDriver_ReadConfig32b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint16_t PCIDriver_ReadConfig16b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint8_t PCIDriver_ReadConfig8b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

uint32_t PCIDriver_DeviceReadConfig32b(struct PCIDriver* self, struct PCI_ConfigHeader* device, uint8_t offset);
uint16_t PCIDriver_DeviceReadConfig16b(struct PCIDriver* self, struct PCI_ConfigHeader* device, uint8_t offset);
uint8_t  PCIDriver_DeviceReadConfig8b(struct PCIDriver* self, struct PCI_ConfigHeader* device, uint8_t offset);

void PCIDriver_ConfigSetAddress(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);

void PCIDriver_WriteConfig32b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data);
void PCIDriver_WriteConfig16b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint16_t data);
void PCIDriver_WriteConfig8b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint8_t data);

void PCIDriver_DeviceWriteConfig32b(struct PCIDriver* self, struct PCI_ConfigHeader* device, uint8_t offset, uint32_t data);
void PCIDriver_DeviceWriteConfig16b(struct PCIDriver* self, struct PCI_ConfigHeader* device, uint8_t offset, uint16_t data);
void PCIDriver_DeviceWriteConfig8b(struct PCIDriver* self, struct PCI_ConfigHeader* device, uint8_t offset, uint8_t data);

uint8_t PCIDriver_GetHeaderType(struct PCIDriver* self, uint8_t bus , uint8_t device , uint8_t function);
uint16_t PCIDriver_GetVendorID(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCIDriver_GetDeviceID(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetClassCode(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetSubClass(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetPrimaryBus(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetSecondaryBus(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCIDriver_GetStatus(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCIDriver_GetCommand(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCIDriver_GetInterruptLine(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCIDriver_GetInterruptPin(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR0(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR1(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR2(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR3(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR4(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCIDriver_GetBAR5(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCIDriver_GetProgIF(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function);