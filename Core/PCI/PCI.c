#include "PCI.h"
#include "../Utilities.h"
#include "../IO/IO.h"
#include "../Screen/Screen.h"

/*
| 31         | 30 - 24  | 23 - 16    | 15 - 11       | 10 - 8          | 7 - 0
| Enable Bit | Reserved | Bus Number | Device Number | Function Number | Register Offset
*/
uint16_t tkPCI_ReadConfigWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset) 
{
    uint32_t address;
    uint32_t lbus  = (uint32_t)bus;
    uint32_t ldevice = (uint32_t)device;
    uint32_t lfunc = (uint32_t)func;
    uint16_t tmp = 0;
    /* create configuration address as per Figure 1 */
    address = (uint32_t)(
        (lbus << 16) | 
        (ldevice << 11) | 
        (lfunc << 8) | 
        (offset & 0xFC) | 
        ((uint32_t)0x80000000)
    );
    /* write out the address */
    tkIO_WritePortLong(PCI_CONFIG_ADDRESS, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    tmp = (uint16_t)((tkIO_ReadPortLong(PCI_CONFIG_DATA) >> ((offset & 2) * 8)) & 0xffff);
    return tmp;
}

void tkPCI_CheckAllBuses()
{
    #ifdef __DEBUG_PCI
        tkScreen_PrintLine("PCI: Probing PCI Devices");
    #endif

    uint8_t function;
    uint8_t bus;
 
    uint8_t headerType = tkPCI_GetHeaderType(0, 0, 0);
    if( (headerType & PCI_HEADER_TYPE_MULTIFUNCTION) == 0) 
    {
        /* Single PCI host controller */
        #ifdef __DEBUG_PCI
            tkScreen_PrintLine("PCI: Found Single Function Host Controller");
        #endif
        tkPCI_CheckBus(0);
    } 
    else 
    {
        #ifdef __DEBUG_PCI
            tkScreen_PrintLine("PCI: Found Multi-Function Host Controller");
        #endif
        /* Multiple PCI host controllers */
        for(function = 0; function < 8; function++) 
        {
            if(tkPCI_GetVendorID(0, 0, function) != PCI_INVALID_VENDOR_ID) 
            {
                break;
            }
            bus = function;
            tkPCI_CheckBus(bus);
        }
    }
} 

void tkPCI_CheckBus(uint8_t bus) 
{
    uint8_t device;
 
    for(device = 0; device < 32; device++) 
    {
        tkPCI_CheckDevice(bus, device);
    }
 }

void tkPCI_CheckDevice(uint8_t bus, uint8_t device) 
{
    uint8_t function = 0;
 
    uint16_t vendorID = tkPCI_GetVendorID(bus, device, function);
    if(vendorID == PCI_INVALID_VENDOR_ID) 
    {
        return; // Device doesn't exist
    }
    #ifdef __DEBUG_PCI
        static char vendorBuffer[BUFLEN];
        memset(vendorBuffer,0,BUFLEN);
        itoa(vendorID,vendorBuffer,BASE_16);
        tkScreen_Print("PCI: Found Device with vid: 0x");
        tkScreen_PrintLine(vendorBuffer);
    #endif
    tkPCI_CheckFunction(bus, device, function);
    uint8_t headerType = tkPCI_GetHeaderType(bus, device, function);
    if( (headerType & PCI_HEADER_TYPE_MULTIFUNCTION) != 0) 
    {
        /* It is a multi-function device, so check remaining functions */
        for(function = 1; function < 8; function++) 
        {
            if(tkPCI_GetVendorID(bus, device, function) != PCI_INVALID_VENDOR_ID) 
            {
                tkPCI_CheckFunction(bus, device, function);
            }
        }
    }
 }
 
 void tkPCI_CheckFunction(uint8_t bus, uint8_t device, uint8_t function) 
 {
    uint8_t baseClass;
    uint8_t subClass;
    uint8_t secondaryBus;
 
    baseClass = tkPCI_GetBaseClass(bus, device, function);
    subClass = tkPCI_GetSubClass(bus, device, function);
    if( (baseClass == PCI_BASE_CLASS_BRIDGE_DEVICE) && 
        (subClass == PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE)) {
        secondaryBus = tkPCI_GetSecondaryBus(bus, device, function);
        tkPCI_CheckBus(secondaryBus);
    }
 }

 uint8_t tkPCI_GetHeaderType(uint8_t bus, uint8_t device, uint8_t function)
 {
    return (uint8_t)tkPCI_ReadConfigWord(bus, device, function, PCI_DEVICE_HEADER_TYPE_OFFSET) & 0x0000FFFF;
 }

 uint16_t tkPCI_GetVendorID(uint8_t bus, uint8_t device, uint8_t function)
 {
    return tkPCI_ReadConfigWord(bus,device,function,PCI_DEVICE_VENDOR_ID_OFFSET);
 }

 uint8_t tkPCI_GetBaseClass(uint8_t bus, uint8_t device, uint8_t function)
 {
    return (uint8_t)tkPCI_ReadConfigWord(bus,device,function,PCI_DEVICE_CLASS_CODE_OFFSET) & 0x0000FFFF;
 }

uint8_t tkPCI_GetSubClass(uint8_t bus, uint8_t device, uint8_t function)
{
    return (uint8_t)tkPCI_ReadConfigWord(bus,device,function,PCI_DEVICE_SUBCLASS_OFFSET) & 0x0000FFFF;
}

uint8_t tkPCI_GetSecondaryBus(uint8_t bus, uint8_t device, uint8_t function)
{
    return (uint8_t)tkPCI_ReadConfigWord(bus,device,function,PCI_DEVICE_SECONDARY_BUS_OFFSET) & 0x0000FFFF;
}