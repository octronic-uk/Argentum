#pragma once

#include <Structures/LinkedList.h>
#include <LibC/include/stdint.h>

#define PCI_CONFIG_ADDRESS 0x0CF8
#define PCI_CONFIG_DATA    0x0CFC

#define PCI_DEVICE_VENDOR_ID_OFFSET 0x00
#define PCI_DEVICE_DEVICE_ID_OFFSET 0x02
#define PCI_DEVICE_COMMAND_OFFSET 0x04
#define PCI_DEVICE_STATUS_OFFSET 0x06
#define PCI_DEVICE_REVISION_ID_OFFSET 0x08
#define PCI_DEVICE_PROG_IF_OFFSET 0x09
#define PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET 0x0A
#define PCI_DEVICE_CACHE_LINE_SZ_OFFSET 0x0C
#define PCI_DEVICE_LATENCY_TIMER 0x0D
#define PCI_DEVICE_HEADER_TYPE_OFFSET 0x0E
#define PCI_DEVICE_BIST 0x0F

#define PCI_DEVICE_BAR0L_OFFSET 0x10
#define PCI_DEVICE_BAR0H_OFFSET 0x12

#define PCI_DEVICE_BAR1_OFFSET 0x14
#define PCI_DEVICE_BAR2_OFFSET 0x18
#define PCI_DEVICE_BAR3_OFFSET 0x1C
#define PCI_DEVICE_BAR4_OFFSET 0x20
#define PCI_DEVICE_BAR5_OFFSET 0x24
#define PCI_DEVICE_SECONDARY_BUS_OFFSET 0x19

#define PCI_DEVICE_INTERRUPT_LINE_OFFSET 0x3C
#define PCI_DEVICE_INTERRUPT_PIN_OFFSET 0x3D

#define PCI_HEADER_TYPE_MULTIFUNCTION (uint8_t)0x80
#define PCI_INVALID_VENDOR_ID (uint16_t)0xFFFF
#define PCI_BASE_CLASS_BRIDGE_DEVICE (uint8_t)0x06
#define PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE (uint8_t)0x04

typedef struct
{
	int16_t  mParentBus;
	uint8_t  mHeaderType;
	uint16_t mVendorID;
	uint16_t mDeviceID;
	uint16_t mStatus;
	uint16_t mCommand;
	uint8_t  mClassCode;
	uint8_t  mSubclassCode;
	uint32_t mBaseAddressRegister;
	uint8_t  mInterruptLine;
	uint8_t  mInterruptPin;
} PCIConfigHeader;

static LinkedList* PCI_ConfigHeaderList;

void PCI_Constructor();

void PCI_dumpDevices();
uint8_t PCI_isMultifunctionDevice(uint8_t headerType);
PCIConfigHeader PCI_readConfigHeader(uint8_t bus, uint8_t device, uint8_t func);
uint16_t PCI_readConfigWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint8_t PCI_getHeaderType(uint8_t bus , uint8_t device , uint8_t function);
uint16_t PCI_getVendorID(uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCI_getDeviceID(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_getClassCode(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_getSubClass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_getSecondaryBus(uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCI_getStatus(uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCI_getCommand(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_getBaseAddressRegister(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCI_getInterruptLine(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCI_getInterruptPin(uint8_t bus, uint8_t device, uint8_t function);
void PCI_dumpDevice(const PCIConfigHeader* header);
void PCI_checkBusForDevices(int16_t parentBus, uint8_t bus);
void PCI_checkDeviceForFunctions(int16_t parentBus, uint8_t bus, uint8_t device);
void PCI_checkFunctionForBridges(int16_t parentBus, uint8_t bus, uint8_t device, uint8_t function);
