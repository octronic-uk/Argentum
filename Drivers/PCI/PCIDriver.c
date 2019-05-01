#include <Drivers/PCI/PCIDriver.h>

#include <string.h>
#include <stdio.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Memory/MemoryDriver.h>
#include <Drivers/PS2/PS2Driver.h>
#include <Drivers/PCI/Vendors.h>

#include <Drivers/ACPI/ACPIDriver.h>
#include <Drivers/ACPI/ACPITableTypes.h>

extern struct Kernel _Kernel;

bool PCIDriver_Constructor(struct PCIDriver* self)
{
	printf("PCI Driver: Constructing\n");

	memset(self,0,sizeof(struct PCIDriver));

	self->Debug = false;

	uint8_t bus, device, function;
	uint8_t current_header = 0;

	for (bus = 0; bus < 16; bus++)
	{
		//if (self->Debug) printf("PCI: Scanning Bus %d\n",bus);
		for (device = 0; device < 32; device++)
		{
			//if (self->Debug) printf("PCI: Scanning Device %d/%d\n",bus,device);
			for (function = 0; function < 8; function++)
			{
				//if (self->Debug) printf("PCI: Scanning Function %d/%d/%d\n",bus,device,function);

				if (PCIDriver_ReadConfigHeader(self, bus, device, function, &self->ConfigHeaderList[current_header]))
				{
					if (self->Debug) 
					{
						printf("PCI: Put device header %d\n",current_header);
					}

					current_header++;

					if (current_header > PCI_MAX_HEADERS) 
					{
						printf("PCI: Maximum number of device headers reached. Increase it!!\n");
						PS2Driver_WaitForKeyPress("PCIDriver wait");
						break;
					}
				}
			}
		}
	}

	if (self->Debug) PCIDriver_DumpDevices(self);
	return true;
}

void PCIDriver_DumpDevices(struct PCIDriver* self)
{
	unsigned int size = PCIDriver_CountHeaders(self);
	for (unsigned int i=0; i<size; i++)
	{
		struct PCIConfigHeader* header = &self->ConfigHeaderList[i];
		PCIDriver_DumpDevice(self, header);
	}

	if (self->Debug)
	{
		PS2Driver_WaitForKeyPress("PCI: Init Complete");
	}
}

void PCIDriver_DumpDevice(struct PCIDriver* self, const struct PCIConfigHeader* header)
{
	printf("PCI: %d/%d/%d (%x:%x)\n",header->mBus, header->mDevice, header->mFunction, header->mVendorID, header->mDeviceID);
	printf("\tProgIF:   0x%x\n", header->mProgIF);
	printf("\tType:     0x%x\n", header->mHeaderType);
	printf("\tClass:    0x%x\n", header->mClassCode);
	printf("\tSubclass: 0x%x\n", header->mSubclassCode);
	printf("\tBAR0      0x%x\n", header->mBAR0);
	printf("\tBAR1      0x%x\n", header->mBAR1);
	printf("\tBAR2      0x%x\n", header->mBAR2);
	printf("\tBAR3      0x%x\n", header->mBAR3);
	printf("\tBAR4      0x%x\n", header->mBAR4);
	printf("\tBAR5      0x%x\n", header->mBAR5);
	PS2Driver_WaitForKeyPress("PCI Device Pause");
}

uint8_t PCIDriver_CountHeaders(struct PCIDriver* self)
{
	uint8_t count;

	for (count=0; count < PCI_MAX_HEADERS; count++)
	{
		if (self->ConfigHeaderList[count].mVendorID == 0) break;
	}
	return count;
}

struct PCIConfigHeader* PCIDriver_GetConfigHeader(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	uint16_t nHeaders = PCIDriver_CountHeaders(self);
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		struct PCIConfigHeader* header = &self->ConfigHeaderList[i];
		if (header->mBus == bus && header->mDevice == device && header->mFunction == function)
		{
			return header;
		}
	}
	return 0;
}

struct PCIConfigHeader* PCIDriver_GetIsaBridgeConfigHeader(struct PCIDriver* self)
{
	uint16_t nHeaders = PCIDriver_CountHeaders(self); 
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		struct PCIConfigHeader* header = &self->ConfigHeaderList[i];
		if (header->mSubclassCode == PCI_SUB_CLASS_PCI_TO_ISA_BRIDGE)
		{
			return header;
		}
	}
	return 0;
}

struct PCIConfigHeader* PCIDriver_GetATADevice(struct PCIDriver* self)
{
	uint16_t nHeaders = PCIDriver_CountHeaders(self);
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		struct PCIConfigHeader* header = &self->ConfigHeaderList[i];
		if (header->mClassCode == 0x01 && header->mSubclassCode == 0x01)
		{
			return header;
		}
	}
	return 0;
}

// Direct IO Functions ========================================================

bool PCIDriver_ReadConfigHeader(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function, struct PCIConfigHeader* header)
{
	uint16_t vid = PCIDriver_GetVendorID(self, bus,device,function);

	if (vid == PCI_INVALID_VENDOR_ID)
	{
		return false;
	}

	if (self->Debug) 
	{
		printf("PCI: Reading Header %d/%d/%d\n",bus,device,function);
	}

	header->mVendorID = vid;
	header->mBus = bus;
	header->mDevice = device;
	header->mFunction = function;
	header->mHeaderType = PCIDriver_GetHeaderType(self, bus, device, function);
	header->mDeviceID = PCIDriver_GetDeviceID(self, bus,device,function);
	header->mStatus = PCIDriver_GetStatus(self, bus,device,function);
	header->mCommand = PCIDriver_GetCommand(self, bus,device,function);
	header->mClassCode = PCIDriver_GetClassCode(self, bus,device,function);
	header->mInterruptLine = PCIDriver_GetInterruptLine(self, bus,device,function);
	header->mInterruptPin = PCIDriver_GetInterruptPin(self, bus,device,function);
	header->mSubclassCode = PCIDriver_GetSubClass(self, bus,device,function);
	header->mProgIF = PCIDriver_GetProgIF(self, bus,device,function);

	switch (header->mHeaderType)
	{
		case 0x00:
			header->mBAR0 = PCIDriver_GetBAR0(self, bus,device,function);
			header->mBAR1 = PCIDriver_GetBAR1(self, bus,device,function);
			header->mBAR2 = PCIDriver_GetBAR2(self, bus,device,function);
			header->mBAR3 = PCIDriver_GetBAR3(self, bus,device,function);
			header->mBAR4 = PCIDriver_GetBAR4(self, bus,device,function);
			header->mBAR5 = PCIDriver_GetBAR5(self, bus,device,function);
			break;
		case 0x01:
			header->mPrimaryBus = PCIDriver_GetPrimaryBus(self, bus,device,function);
			header->mSecondaryBus = PCIDriver_GetSecondaryBus(self, bus,device,function);
			header->mBAR0 = PCIDriver_GetBAR0(self, bus,device,function);
			header->mBAR1 = PCIDriver_GetBAR1(self, bus,device,function);
			break;
	}
	if (self->Debug) 
	{
		printf("PCI: Got Header OK\n");
	}
	return true;
}

uint8_t PCIDriver_isMultifunctionDevice(struct PCIDriver* self, uint8_t headerType)
{
	return ((headerType & PCI_HEADER_TYPE_MULTIFUNCTION) != 0);
}

uint8_t PCIDriver_GetHeaderType(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus, device, function, PCI_DEVICE_HEADER_TYPE_OFFSET);
}

uint16_t PCIDriver_GetVendorID(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig16b(self, bus,device,function,PCI_DEVICE_VENDOR_ID_OFFSET);
}

uint16_t PCIDriver_GetDeviceID(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig16b(self, bus,device,function,PCI_DEVICE_DEVICE_ID_OFFSET);
}

uint8_t PCIDriver_GetClassCode(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
}

uint8_t PCIDriver_GetSubClass(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_SUBCLASS_CLASS_CODE_OFFSET);
}

uint16_t PCIDriver_GetStatus(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig16b(self, bus,device,function,PCI_DEVICE_STATUS_OFFSET);
}

uint16_t PCIDriver_GetCommand(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig16b(self, bus,device,function,PCI_DEVICE_COMMAND_OFFSET);
}

uint32_t PCIDriver_GetBAR0(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig32b(self, bus,device,function,PCI_DEVICE_BAR0L_OFFSET);
}

uint32_t PCIDriver_GetBAR1(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig32b(self, bus,device,function,PCI_DEVICE_BAR1L_OFFSET);
}

uint32_t PCIDriver_GetBAR2(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig32b(self, bus,device,function,PCI_DEVICE_BAR2L_OFFSET);
}

uint32_t PCIDriver_GetBAR3(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig32b(self, bus,device,function,PCI_DEVICE_BAR3L_OFFSET);
}

uint32_t PCIDriver_GetBAR4(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig32b(self, bus,device,function,PCI_DEVICE_BAR4L_OFFSET);
}

uint32_t PCIDriver_GetBAR5(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig32b(self, bus,device,function,PCI_DEVICE_BAR5L_OFFSET);
}

uint8_t PCIDriver_GetPrimaryBus(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_PRIMARY_BUS_OFFSET);
}

uint8_t PCIDriver_GetSecondaryBus(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_SECONDARY_BUS_OFFSET);
}

uint8_t PCIDriver_GetInterruptLine(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_INTERRUPT_LINE_OFFSET);
}

uint8_t PCIDriver_GetInterruptPin(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_INTERRUPT_PIN_OFFSET);
}

uint8_t PCIDriver_GetProgIF(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t function)
{
	return PCIDriver_ReadConfig8b(self, bus,device,function,PCI_DEVICE_PROG_IF_OFFSET);
}

// Addressing =================================================================

/*
| 31         | 30 - 24  | 23 - 16    | 15 - 11       | 10 - 8          | 7 - 0
| Enable Bit | Reserved | Bus Number | Device Number | Function Number | Register Offset
*/
void PCIDriver_ConfigSetAddress(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
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

uint32_t PCIDriver_ReadConfig32b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	PCIDriver_ConfigSetAddress(self, bus,device,func,offset);
	return  IO_ReadPort32b(PCI_CONFIG_DATA);
}

uint16_t PCIDriver_ReadConfig16b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	PCIDriver_ConfigSetAddress(self, bus,device,func,offset);
	return (uint16_t)((IO_ReadPort32b(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
}

uint8_t PCIDriver_ReadConfig8b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
{
	PCIDriver_ConfigSetAddress(self, bus,device,func,offset);
	return (uint8_t)((IO_ReadPort32b(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xff);
}

uint32_t PCIDriver_DeviceReadConfig32b(struct PCIDriver* self, struct PCIConfigHeader* device, uint8_t offset)
{
	return PCIDriver_ReadConfig32b(self, device->mBus, device->mDevice, device->mFunction, offset);
}

uint16_t PCIDriver_DeviceReadConfig16b(struct PCIDriver* self, struct PCIConfigHeader* device, uint8_t offset)
{
	return PCIDriver_ReadConfig16b(self, device->mBus, device->mDevice, device->mFunction, offset);
}

uint8_t PCIDriver_DeviceReadConfig8b(struct PCIDriver* self, struct PCIConfigHeader* device, uint8_t offset)
{
	return PCIDriver_ReadConfig8b(self, device->mBus, device->mDevice, device->mFunction, offset);
}

// Write =======================================================================

void PCIDriver_WriteConfig32b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint32_t data)
{
	PCIDriver_ConfigSetAddress(self, bus,device,func,offset);
	IO_WritePort32b(PCI_CONFIG_DATA,data);
}

void PCIDriver_WriteConfig16b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint16_t data)
{
	PCIDriver_ConfigSetAddress(self, bus,device,func,offset);
	IO_WritePort16b(PCI_CONFIG_DATA,data);
}

void PCIDriver_WriteConfig8b(struct PCIDriver* self, uint8_t bus, uint8_t device, uint8_t func, uint8_t offset, uint8_t data)
{
	PCIDriver_ConfigSetAddress(self, bus,device,func,offset);
	IO_WritePort8b(PCI_CONFIG_DATA,data);
}

void PCIDriver_DeviceWriteConfig32b(struct PCIDriver* self, struct PCIConfigHeader* device, uint8_t offset, uint32_t data)
{
	PCIDriver_WriteConfig32b(self, device->mBus, device->mDevice, device->mFunction, offset, data);
}

void PCIDriver_DeviceWriteConfig16b(struct PCIDriver* self, struct PCIConfigHeader* device, uint8_t offset, uint16_t data)
{
	PCIDriver_WriteConfig16b(self, device->mBus, device->mDevice, device->mFunction, offset, data);
}

void PCIDriver_DeviceWriteConfig8b(struct PCIDriver* self, struct PCIConfigHeader* device, uint8_t offset, uint8_t data)
{
	PCIDriver_WriteConfig8b(self, device->mBus, device->mDevice, device->mFunction, offset, data);
}


struct PCIConfigHeader* PCIDriver_GetDeviceByID(struct PCIDriver* self, uint16_t vendor_id, uint16_t device_id)
{
	uint16_t nHeaders = PCIDriver_CountHeaders(self);
	uint16_t i;
	for (i=0; i<nHeaders; i++)
	{
		struct PCIConfigHeader* header = &self->ConfigHeaderList[i];
		if (header->mVendorID == vendor_id && header->mDeviceID == device_id)
		{
			return header;
		}
	}
	return 0;
}