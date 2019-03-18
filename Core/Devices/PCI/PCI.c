#include "PCI.h"
#include <Devices/IO/IO.h>
#include <Devices/Memory/Memory.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

LinkedList* PCI_ConfigHeaderList = 0;
uint8_t PCI_Debug = 0;

void PCI_SetDebug(uint32_t debug)
{
	PCI_Debug = debug;
}

void PCI_Constructor()
{
    PCI_ConfigHeaderList = LinkedList_Constructor();
	if (PCI_Debug) {
		printf("PCI: Initialising\n");
	}

	uint8_t bus, device, function;
	PCI_ConfigHeader* tmp = 0;
	for (bus = 0; bus < 8; bus++)
	{
		for (device = 0; device < 8; device++)
		{
			for (function = 0; function < 8; function++)
			{
				tmp = PCI_ReadConfigHeader(bus,device,function);				
				if (tmp)
				{
					LinkedList_PushBack(PCI_ConfigHeaderList,tmp);
				}
			}
		}
	}
	PCI_DumpDevices();
	if (PCI_Debug) {
		printf("PCI: Init Complete\n");
	}
}

void PCI_DumpDevices()
{
	unsigned int size = LinkedList_Size(PCI_ConfigHeaderList);
	for (unsigned int i=0; i<size; i++)
	{
		PCI_ConfigHeader* header = LinkedList_At(PCI_ConfigHeaderList,i);
		PCI_DumpDevice(header);
	}
}

void PCI_DumpDevice(const PCI_ConfigHeader* header)
{
	printf(
		"PCI: %d/%d/%d (%x:%x)\n", 
		header->mBus, header->mDevice, header->mFunction, 
		header->mVendorID, header->mDeviceID);

	if (PCI_Debug)
	{
		printf("\tType: 0x%x\n",header->mHeaderType);
		printf("\tClass: 0x%x\n",	header->mClassCode);
		printf("\tSubclass: 0x%x\n",header->mSubclassCode);
		printf("\tBAR0 0x%x\n",header->mBAR0);
		printf("\tBAR1 0x%x\n",header->mBAR1);
		printf("\tBAR2 0x%x\n",header->mBAR2);
		printf("\tBAR3 0x%x\n",header->mBAR3);
		printf("\tBAR4 0x%x\n",header->mBAR4);
		printf("\tBAR5 0x%x\n",header->mBAR5);
	}
}

PCI_ConfigHeader* PCI_GetConfigHeader(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t nHeaders = LinkedList_Size(PCI_ConfigHeaderList);
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		PCI_ConfigHeader* header = LinkedList_At(PCI_ConfigHeaderList,i);
		if (header->mBus == bus && header->mDevice == device && header->mFunction == function)
		{
			return header;
		}
	}
	return 0;
}

PCI_ConfigHeader* PCI_GetIsaBridgeConfigHeader()
{
	uint16_t nHeaders = LinkedList_Size(PCI_ConfigHeaderList);
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		PCI_ConfigHeader* header = LinkedList_At(PCI_ConfigHeaderList,i);
		if (header->mSubclassCode == PCI_SUB_CLASS_PCI_TO_ISA_BRIDGE)
		{
			return header;
		}
	}
	return 0;
}

PCI_ConfigHeader* PCI_GetATADevice()
{
	uint16_t nHeaders = LinkedList_Size(PCI_ConfigHeaderList);
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		PCI_ConfigHeader* header = LinkedList_At(PCI_ConfigHeaderList,i);
		if (header->mClassCode == 0x01 && header->mSubclassCode == 0x01)
		{
			return header;
		}
	}
	return 0;
}

// Direct IO Functions ========================================================

PCI_ConfigHeader* PCI_ReadConfigHeader
(uint8_t bus, uint8_t device, uint8_t function)
{
	if (PCI_Debug) {
		printf("PCI: Reading Header %d/%d/%d\n",bus,device,function);
	}

	uint16_t vid = PCI_GetVendorID(bus,device,function);

	if (vid == PCI_INVALID_VENDOR_ID)
	{
		return 0;
	}

	PCI_ConfigHeader* header = Memory_Allocate(sizeof(PCI_ConfigHeader));
	memset(header,0,sizeof(PCI_ConfigHeader));
	
	header->mVendorID = vid;
	header->mBus = bus;
	header->mDevice = device;
	header->mFunction = function;
	header->mHeaderType = PCI_GetHeaderType(bus, device, function);
	header->mDeviceID = PCI_GetDeviceID(bus,device,function);
	header->mStatus = PCI_GetStatus(bus,device,function);
	header->mCommand = PCI_GetCommand(bus,device,function);
	header->mClassCode = PCI_GetClassCode(bus,device,function);
	header->mInterruptLine = PCI_GetInterruptLine(bus,device,function);
	header->mInterruptPin = PCI_GetInterruptPin(bus,device,function);
	header->mSubclassCode = PCI_GetSubClass(bus,device,function);
	header->mProgIF = PCI_GetProgIF(bus,device,function);

	switch (header->mHeaderType)
	{
		case 0x00:
			header->mBAR0 = PCI_GetBAR0(bus,device,function);
			header->mBAR1 = PCI_GetBAR1(bus,device,function);
			header->mBAR2 = PCI_GetBAR2(bus,device,function);
			header->mBAR3 = PCI_GetBAR3(bus,device,function);
			header->mBAR4 = PCI_GetBAR4(bus,device,function);
			header->mBAR5 = PCI_GetBAR5(bus,device,function);
			break;
		case 0x01:
			header->mPrimaryBus = PCI_GetPrimaryBus(bus,device,function);
			header->mSecondaryBus = PCI_GetSecondaryBus(bus,device,function);
			header->mBAR0 = PCI_GetBAR0(bus,device,function);
			header->mBAR1 = PCI_GetBAR1(bus,device,function);
			break;
	}
	if (PCI_Debug) {
		printf("PCI: Got Header OK\n");
	}
	return header;
}

uint8_t PCI_isMultifunctionDevice(uint8_t headerType)
{
	return ((headerType & PCI_HEADER_TYPE_MULTIFUNCTION) != 0);
}

uint8_t PCI_GetHeaderType(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus, device, function, PCI_DEVICE_HEADER_TYPE_OFFSET);
}

uint16_t PCI_GetVendorID(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig16b(bus,device,function,PCI_DEVICE_VENDOR_ID_OFFSET);
}

uint16_t PCI_GetDeviceID(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig16b(bus,device,function,PCI_DEVICE_DEVICE_ID_OFFSET);
}

uint8_t PCI_GetClassCode(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
}

uint8_t PCI_GetSubClass(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
}

uint16_t PCI_GetStatus(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig16b(bus,device,function,PCI_DEVICE_STATUS_OFFSET);
}

uint16_t PCI_GetCommand(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig16b(bus,device,function,PCI_DEVICE_COMMAND_OFFSET);
}

uint32_t PCI_GetBAR0(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig32b(bus,device,function,PCI_DEVICE_BAR0L_OFFSET);
}

uint32_t PCI_GetBAR1(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig32b(bus,device,function,PCI_DEVICE_BAR1L_OFFSET);
}

uint32_t PCI_GetBAR2(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig32b(bus,device,function,PCI_DEVICE_BAR2L_OFFSET);
}

uint32_t PCI_GetBAR3(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig32b(bus,device,function,PCI_DEVICE_BAR3L_OFFSET);
}

uint32_t PCI_GetBAR4(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig32b(bus,device,function,PCI_DEVICE_BAR4L_OFFSET);
}

uint32_t PCI_GetBAR5(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig32b(bus,device,function,PCI_DEVICE_BAR5L_OFFSET);
}

uint8_t PCI_GetPrimaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_PRIMARY_BUS_OFFSET);
}

uint8_t PCI_GetSecondaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_SECONDARY_BUS_OFFSET);
}

uint8_t PCI_GetInterruptLine(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_INTERRUPT_LINE_OFFSET);
}

uint8_t PCI_GetInterruptPin(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_INTERRUPT_PIN_OFFSET);
}

uint8_t PCI_GetProgIF(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfig8b(bus,device,function,PCI_DEVICE_PROG_IF_OFFSET);
}

// Addressing =================================================================

/*
| 31         | 30 - 24  | 23 - 16    | 15 - 11       | 10 - 8          | 7 - 0
| Enable Bit | Reserved | Bus Number | Device Number | Function Number | Register Offset
*/
void PCI_ConfigSetAddress(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	uint32_t address;
	uint32_t lbus  = (uint32_t)bus;
	uint32_t ldevice = (uint32_t)device;
	uint32_t lfunc = (uint32_t)func;
	uint16_t tmp = 0;
	/* create configuration address as per Figure 1 */
	address = (uint32_t)
	(
		(lbus << 16)    |
		(ldevice << 11) |
		(lfunc << 8)    |
		(offset & 0xFC) |
		((uint32_t)0x80000000)
	);
	IO_WritePort32b(PCI_CONFIG_ADDRESS,address);
}

// Read =======================================================================

uint32_t PCI_ReadConfig32b(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	PCI_ConfigSetAddress(bus,device,func,offset);
	return  IO_ReadPort32b(PCI_CONFIG_DATA);
}

uint16_t PCI_ReadConfig16b(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	PCI_ConfigSetAddress(bus,device,func,offset);
	return (uint16_t)((IO_ReadPort32b(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
}

uint8_t PCI_ReadConfig8b(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	PCI_ConfigSetAddress(bus,device,func,offset);
	return (uint8_t)((IO_ReadPort32b(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xff);
}

uint32_t PCI_DeviceReadConfig32b(PCI_ConfigHeader* device, uint8_t offset)
{
	return PCI_ReadConfig32b(device->mBus, device->mDevice, device->mFunction, offset);
}

uint16_t PCI_DeviceReadConfig16b(PCI_ConfigHeader* device, uint8_t offset)
{
	return PCI_ReadConfig16b(device->mBus, device->mDevice, device->mFunction, offset);
}

uint8_t PCI_DeviceReadConfig8b(PCI_ConfigHeader* device, uint8_t offset)
{
	return PCI_ReadConfig8b(device->mBus, device->mDevice, device->mFunction, offset);
}

// Write =======================================================================

void PCI_WriteConfig32b(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data)
{
	PCI_ConfigSetAddress(bus,device,func,offset);
	IO_WritePort32b(PCI_CONFIG_DATA,data);
}

void PCI_WriteConfig16b(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint16_t data)
{
	PCI_ConfigSetAddress(bus,device,func,offset);
	IO_WritePort16b(PCI_CONFIG_DATA,data);
}

void PCI_WriteConfig8b(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint8_t data)
{
	PCI_ConfigSetAddress(bus,device,func,offset);
	IO_WritePort8b(PCI_CONFIG_DATA,data);
}

void PCI_DeviceWriteConfig32b(PCI_ConfigHeader* device, uint8_t offset, uint32_t data)
{
	PCI_WriteConfig32b(device->mBus, device->mDevice, device->mFunction, offset, data);
}

void PCI_DeviceWriteConfig16b(PCI_ConfigHeader* device, uint8_t offset, uint16_t data)
{
	PCI_WriteConfig16b(device->mBus, device->mDevice, device->mFunction, offset, data);
}

void PCI_DeviceWriteConfig8b(PCI_ConfigHeader* device, uint8_t offset, uint8_t data)
{
	PCI_WriteConfig8b(device->mBus, device->mDevice, device->mFunction, offset, data);
}