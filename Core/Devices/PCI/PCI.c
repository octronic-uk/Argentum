#include "PCI.h"
#include <Devices/IO/IO.h>
#include <LibC/include/stdio.h>
#include <LibC/include/string.h>

void PCI_Constructor()
{
    PCI_ConfigHeaderList = LinkedList_Constructor();
	#ifdef __DEBUG_PCI
	printf("PCI: Initialising\n");
	#endif

	/*
	uint16_t bus;
	uint8_t device;

	PCI_ConfigHeader busHeader;
	PCI_readConfigHeader(&busHeader,0,0,0);
	busHeader.mParentBus = -1;
	tkLinkedList_InsertCopy(PCI_ConfigHeaderList, &busHeader,sizeof(PCI_ConfigHeader));
	for(bus = 0; bus < 256; bus++)
	{
		for(device = 0; device < 32; device++)
		{
			PCI_checkDeviceForFunctions(-1, bus, device);
		}
	}
	*/
	uint8_t function;
	uint8_t bus;
	PCIConfigHeader busHeader = PCI_readConfigHeader(0,0,0);
	busHeader.mParentBus = -1;
	LinkedList_PushBack(PCI_ConfigHeaderList,&busHeader);

	uint8_t headerType = PCI_getHeaderType(0, 0, 0);
	if( (headerType & PCI_HEADER_TYPE_MULTIFUNCTION) == 0)
	{
		/* Single PCI host controller */
		PCI_checkBusForDevices(-1,0);
	}
	else
	{
		/* Multiple PCI host controllers */
		for(function = 0; function < 8; function++)
		{
			#ifdef __DEBUG_PCI
			printf("FunctionLoop\n");
			#endif
			if(PCI_getVendorID(0, 0, function) != PCI_INVALID_VENDOR_ID)
			{
				break;
			}
			bus = function;
			PCI_checkBusForDevices(-1,bus);
		}
	 }
	PCI_dumpDevices();
	#ifdef __DEBUG_PCI
	printf("PCI: Init Complete\n");

	#endif
}

void PCI_dumpDevices()
{
	unsigned int size = LinkedList_Size(PCI_ConfigHeaderList);
	for (unsigned int i=0; i<size; i++)
	{
		PCIConfigHeader* header = LinkedList_At(PCI_ConfigHeaderList,i);
		PCI_dumpDevice(header);
	}
}

void PCI_dumpDevice(const PCIConfigHeader* header)
{
	printf("PCI: (%d) %d:%d\n", header->mParentBus, header->mVendorID, header->mDeviceID);
}

void PCI_checkBusForDevices(int16_t parentBus, uint8_t bus)
{
	uint8_t device;
	for(device = 0; device < 32; device++)
	{
		printf("for device loop\n");
		uint16_t vendorID = PCI_getVendorID(bus, device, 0);
		if(vendorID == PCI_INVALID_VENDOR_ID)
		{
			break;
		}
		PCI_checkDeviceForFunctions(parentBus, bus, device);
	}
}

void PCI_checkDeviceForFunctions(int16_t parentBus, uint8_t bus, uint8_t device)
{
	uint8_t function = 0;
	uint16_t vendorID = PCI_getVendorID(bus, device, function);
	if(vendorID == PCI_INVALID_VENDOR_ID)
	{
		return; // Device doesn't exist
	}

	printf("vid: 0x%d", vendorID);

	PCIConfigHeader deviceHeader;
	//PCI_ConfigHeader functionHeader;

	deviceHeader = PCI_readConfigHeader(bus,device,0);
	deviceHeader.mParentBus = parentBus;
	LinkedList_PushBack(PCI_ConfigHeaderList, &deviceHeader);

	//PCI_readConfigHeader(&functionHeader,bus,device,function);
	//functionHeader.mParentBus = parentBus;
	//tkLinkedList_InsertCopy(PCI_ConfigHeaderList,&functionHeader,sizeof(PCI_ConfigHeader));

	PCI_checkFunctionForBridges(parentBus, bus, device, function);
	uint8_t headerType = PCI_getHeaderType(bus, device, function);
	if( (headerType & PCI_HEADER_TYPE_MULTIFUNCTION) != 0)
	{
		PCIConfigHeader confHeader = PCI_readConfigHeader(bus,device,function);
		confHeader.mParentBus = parentBus;
		LinkedList_PushBack(PCI_ConfigHeaderList,&confHeader);
		/* It is a multi-function device, so check remaining functions */
		for(function = 1; function < 8; function++)
		{
			printf("for check device for fn loop\n");
			if(PCI_getVendorID(bus, device, function) != PCI_INVALID_VENDOR_ID)
			{
				PCI_checkFunctionForBridges(parentBus, bus, device, function);
			}
		}
	}
}

void PCI_checkFunctionForBridges
(int16_t parentBus, uint8_t bus, uint8_t device, uint8_t function)
{
	uint8_t baseClass;
	uint8_t subClass;

	baseClass = PCI_getClassCode(bus, device, function);
	subClass = PCI_getSubClass(bus, device, function);

	printf("PCI: Bridge Check - base 0x%d sub 0x%d\n", baseClass, subClass);

	if(baseClass == PCI_BASE_CLASS_BRIDGE_DEVICE && subClass == PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE)
	{
		uint8_t secondaryBus;
		secondaryBus = PCI_getSecondaryBus(bus, device, function);
		printf("secondary bus 0x%d\n", secondaryBus);
		PCI_checkBusForDevices(parentBus+1, secondaryBus);
	}
}

// Direct IO Functions ========================================================

PCIConfigHeader PCI_readConfigHeader
(uint8_t bus, uint8_t device, uint8_t function)
{
	PCIConfigHeader header;
	#ifdef __DEBUG_PCI
	printf("PCI: reading Header";
	#endif
	memset(&header,0,sizeof(PCIConfigHeader));
	header.mHeaderType = PCI_getHeaderType(bus, device, function);
	printf("type ");
	header.mVendorID = PCI_getVendorID(bus,device,function);
	printf("vendor ");
	header.mDeviceID = PCI_getDeviceID(bus,device,function);
	printf("device ");
	header.mStatus = PCI_getStatus(bus,device,function);
	printf("status ");
	header.mCommand = PCI_getCommand(bus,device,function);
	printf("command ");
	header.mClassCode = PCI_getClassCode(bus,device,function);
	printf("class code ");
	header.mBaseAddressRegister = PCI_getBaseAddressRegister(bus,device,function);
	printf("BAR ");
	header.mInterruptLine = PCI_getInterruptLine(bus,device,function);
	printf("intline ");
	header.mInterruptPin = PCI_getInterruptPin(bus,device,function);
	printf("intpin ");
	header.mSubclassCode = PCI_getSubClass(bus,device,function);
	printf("subclass ");
	#ifdef __DEBUG_PCI
	printf("PCI: Got Header OK";
	#endif
	return header;
}

uint8_t PCI_isMultifunctionDevice(uint8_t headerType)
{
	return ((headerType & PCI_HEADER_TYPE_MULTIFUNCTION) != 0);
}

uint8_t PCI_getHeaderType(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_readConfigWord(bus, device, function, PCI_DEVICE_HEADER_TYPE_OFFSET);
}

uint16_t PCI_getVendorID(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_readConfigWord(bus,device,function,PCI_DEVICE_VENDOR_ID_OFFSET);
}

uint16_t PCI_getDeviceID(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_readConfigWord(bus,device,function,PCI_DEVICE_DEVICE_ID_OFFSET);
}

uint8_t PCI_getClassCode(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t code = PCI_readConfigWord(bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
	return (code & 0xFF00) >> 8;
}

uint8_t PCI_getSubClass(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_readConfigWord(bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
}

uint8_t PCI_getSecondaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_readConfigWord(bus,device,function,PCI_DEVICE_SECONDARY_BUS_OFFSET);
}

uint16_t PCI_getStatus(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_readConfigWord(bus,device,function,PCI_DEVICE_STATUS_OFFSET);
}

uint16_t PCI_getCommand(uint8_t bus, uint8_t device, uint8_t function)
{
	return PCI_readConfigWord(bus,device,function,PCI_DEVICE_COMMAND_OFFSET);
}

uint32_t PCI_getBaseAddressRegister(uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t bar0_l = PCI_readConfigWord(bus,device,function,PCI_DEVICE_BAR0L_OFFSET);
	uint16_t bar0_h = PCI_readConfigWord(bus,device,function,PCI_DEVICE_BAR0H_OFFSET);
	return (bar0_h << 16) & bar0_l;
}

uint8_t PCI_getInterruptLine(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_readConfigWord(bus,device,function,PCI_DEVICE_INTERRUPT_LINE_OFFSET);
}

uint8_t PCI_getInterruptPin(uint8_t bus, uint8_t device, uint8_t function)
{
	return (uint8_t)PCI_readConfigWord(bus,device,function,PCI_DEVICE_INTERRUPT_PIN_OFFSET);
}

/*
| 31         | 30 - 24  | 23 - 16    | 15 - 11       | 10 - 8          | 7 - 0
| Enable Bit | Reserved | Bus Number | Device Number | Function Number | Register Offset
*/
uint16_t PCI_readConfigWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
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
	/* read in the data */
	/* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
	tmp = (uint16_t)((IO_ReadPortLong(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
	return tmp;
}

