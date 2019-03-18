#include "PCI.h"
#include <Devices/IO/IO.h>
#include <Devices/Memory/Memory.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

LinkedList* PCI_ConfigHeaderList = 0;

void PCI_Constructor()
{
    PCI_ConfigHeaderList = LinkedList_Constructor();
	#ifdef __DEBUG_PCI
	printf("PCI: Initialising\n");
	#endif

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
	#ifdef __DEBUG_PCI
	printf("PCI: Init Complete\n");
	#endif
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
	char *subName = "";
	if (header->mSubclassCode == PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE)
	{
		subName = "PCI-PCI Bridge";
	}
	else if (header->mSubclassCode == PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE)
	{
		subName = "PCI-ISA Bridge";
	}

	printf(
		"PCI: pci:/%d/%d/%d (%x:%x) Type: 0x%x, Class: 0x%d, SubClass: 0x%x %s\n", 
		header->mBus,
		header->mDevice,
		header->mFunction,
		header->mVendorID, 
		header->mDeviceID,
		header->mHeaderType,
		header->mClassCode,
		header->mSubclassCode,
		subName
	);
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
	#ifdef __DEBUG_PCI
	printf("PCI: Reading Header %d/%d/%d\n",bus,device,function);
	#endif

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
	#ifdef __DEBUG_PCI
	printf("PCI: Got Header OK\n");
	#endif
	return header;
}

uint8_t PCI_isMultifunctionDevice(uint8_t headerType)
{
	return ((headerType & PCI_HEADER_TYPE_MULTIFUNCTION) != 0);
}

uint8_t PCI_GetHeaderType(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_ReadConfigWord(bus, device, function, PCI_DEVICE_HEADER_TYPE_OFFSET);
}

uint16_t PCI_GetVendorID(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_VENDOR_ID_OFFSET);
}

uint16_t PCI_GetDeviceID(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_DEVICE_ID_OFFSET);
}

uint8_t PCI_GetClassCode(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t code = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
	return (code & 0xFF00) >> 8;
}

uint8_t PCI_GetSubClass(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
}

uint16_t PCI_GetStatus(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_STATUS_OFFSET);
}

uint16_t PCI_GetCommand(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_COMMAND_OFFSET);
}

uint32_t PCI_GetBAR0(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar_l = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR0L_OFFSET);
	uint16_t bar_h = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR0H_OFFSET);
	return (uint32_t)bar_h << 16 | (uint32_t)bar_l;
}

uint32_t PCI_GetBAR1(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar_l = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR1L_OFFSET);
	uint16_t bar_h = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR1H_OFFSET);
	return (uint32_t)bar_h << 16 | (uint32_t)bar_l;
}

uint32_t PCI_GetBAR2(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar_l = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR2L_OFFSET);
	uint16_t bar_h = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR2H_OFFSET);
	return (uint32_t)bar_h << 16 | (uint32_t)bar_l;
}

uint32_t PCI_GetBAR3(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar_l = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR3L_OFFSET);
	uint16_t bar_h = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR3H_OFFSET);
	return (uint32_t)bar_h << 16 | (uint32_t)bar_l;
}

uint32_t PCI_GetBAR4(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar_l = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR4L_OFFSET);
	uint16_t bar_h = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR4H_OFFSET);
	return (uint32_t)bar_h << 16 | (uint32_t)bar_l;
}

uint32_t PCI_GetBAR5(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar_l = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR5L_OFFSET);
	uint16_t bar_h = PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_BAR5H_OFFSET);
	return (uint32_t)bar_h << 16 | (uint32_t)bar_l;
}

uint8_t PCI_GetPrimaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_PRIMARY_BUS_OFFSET);
}

uint8_t PCI_GetSecondaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_SECONDARY_BUS_OFFSET);
}

uint8_t PCI_GetInterruptLine(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_INTERRUPT_LINE_OFFSET);
}

uint8_t PCI_GetInterruptPin(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_INTERRUPT_PIN_OFFSET);
}

uint8_t PCI_GetProgIF(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)(PCI_ReadConfigWord(bus,device,function,PCI_DEVICE_PROG_IF_OFFSET) >> 8);
}
/*
| 31         | 30 - 24  | 23 - 16    | 15 - 11       | 10 - 8          | 7 - 0
| Enable Bit | Reserved | Bus Number | Device Number | Function Number | Register Offset
*/
uint16_t PCI_ReadConfigWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
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
	/* write out the address */
	IO_WritePortLong(PCI_CONFIG_ADDRESS, address);
	/* Read in the data */
	/* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
	tmp = (uint16_t)((IO_ReadPortLong(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
	return tmp;
}

