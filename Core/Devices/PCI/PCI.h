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

#define PCI_DEVICE_BAR1L_OFFSET 0x14
#define PCI_DEVICE_BAR1H_OFFSET 0x16

#define PCI_DEVICE_BAR2L_OFFSET 0x18
#define PCI_DEVICE_BAR2H_OFFSET 0x1A

#define PCI_DEVICE_BAR3L_OFFSET 0x1C
#define PCI_DEVICE_BAR3H_OFFSET 0x1E

#define PCI_DEVICE_BAR4L_OFFSET 0x20
#define PCI_DEVICE_BAR4H_OFFSET 0x22

#define PCI_DEVICE_BAR5L_OFFSET 0x24
#define PCI_DEVICE_BAR5H_OFFSET 0x26

#define PCI_DEVICE_INTERRUPT_LINE_OFFSET 0x3C
#define PCI_DEVICE_INTERRUPT_PIN_OFFSET 0x3D
#define PCI_DEVICE_PRIMARY_BUS_OFFSET 0x08
#define PCI_DEVICE_SECONDARY_BUS_OFFSET 0x19

#define PCI_HEADER_TYPE_MULTIFUNCTION (uint8_t)0x80
#define PCI_INVALID_VENDOR_ID (uint16_t)0xFFFF
#define PCI_BASE_CLASS_BRIDGE_DEVICE (uint8_t)0x06
#define PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE (uint8_t)0x04
#define PCI_SUB_CLASS_PCI_TO_ISA_BRIDGE (uint8_t)0x01

typedef struct
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
} PCI_ConfigHeader;


void PCI_Constructor();

void PCI_DumpDevices();
void PCI_DumpDevice(const PCI_ConfigHeader* header);

PCI_ConfigHeader* PCI_ReadConfigHeader(uint8_t bus, uint8_t device, uint8_t func);
PCI_ConfigHeader* PCI_GetConfigHeader(uint8_t bus, uint8_t device, uint8_t function);
PCI_ConfigHeader* PCI_GetIsaBridgeConfigHeader();
PCI_ConfigHeader* PCI_GetATADevice();

uint8_t PCI_IsMultifunctionDevice(uint8_t headerType);
uint16_t PCI_ReadConfigWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint8_t PCI_GetHeaderType(uint8_t bus , uint8_t device , uint8_t function);
uint16_t PCI_GetVendorID(uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCI_GetDeviceID(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_GetClassCode(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_GetSubClass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_GetPrimaryBus(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_GetSecondaryBus(uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCI_GetStatus(uint8_t bus, uint8_t device, uint8_t function);
uint16_t PCI_GetCommand(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCI_GetInterruptLine(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  PCI_GetInterruptPin(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_GetBAR0(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_GetBAR1(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_GetBAR2(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_GetBAR3(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_GetBAR4(uint8_t bus, uint8_t device, uint8_t function);
uint32_t PCI_GetBAR5(uint8_t bus, uint8_t device, uint8_t function);
uint8_t PCI_GetProgIF(uint8_t bus, uint8_t device, uint8_t function);