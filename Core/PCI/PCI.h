#pragma once
#include "../Types.h"

#define PCI_CONFIG_ADDRESS 0x0CF8
#define PCI_CONFIG_DATA    0x0CFC 

#define PCI_DEVICE_VENDOR_ID_OFFSET 0x00

#define PCI_DEVICE_DEVICE_ID_OFFSET 0x02

#define PCI_DEVICE_COMMAND_OFFSET 0x04
#define PCI_DEVICE_STATUS_OFFSET 0x06

#define PCI_DEVICE_REVISION_ID_OFFSET 0x08
#define PCI_DEVICE_PROG_IF_OFFSET 0x09
#define PCI_DEVICE_SUBCLASS_OFFSET 0x0A
#define PCI_DEVICE_CLASS_CODE_OFFSET 0x0B

#define PCI_DEVICE_CACHE_LINE_SZ_OFFSET 0x0C
#define PCI_DEVICE_LATENCY_TIMER 0x0D
#define PCI_DEVICE_HEADER_TYPE_OFFSET 0x0E
#define PCI_DEVICE_BIST 0x0F

#define PCI_DEVICE_BAR0_OFFSET 0x10
#define PCI_DEVICE_BAR1_OFFSET 0x14
#define PCI_DEVICE_BAR2_OFFSET 0x18
#define PCI_DEVICE_BAR3_OFFSET 0x1C
#define PCI_DEVICE_BAR4_OFFSET 0x20
#define PCI_DEVICE_BAR5_OFFSET 0x24

#define PCI_DEVICE_SECONDARY_BUS_OFFSET 0x19

#define PCI_HEADER_TYPE_MULTIFUNCTION 0x80
#define PCI_INVALID_VENDOR_ID 0xFFFF
#define PCI_BASE_CLASS_BRIDGE_DEVICE 0x06
#define PCI_SUB_CLASS_PCI_TO_PCI_BRIDGE 0x04
/*
The following field descriptions are common to all Header Types:
    Device ID:       Identifies the particular device. Where valid IDs are allocated by the vendor.
    Vendor ID:       Identifies the manufacturer of the device. Where valid IDs are allocated by PCI-SIG (the list is here) to ensure uniqueness and 0xFFFF is an invalid value that will be returned on read accesses to Configuration Space registers of non-existent devices.
    Status:          A register used to record status information for PCI bus related events.
    Command:         Provides control over a device's ability to generate and respond to PCI cycles. Where the only functionality guaranteed to be supported by all devices is, when a 0 is written to this register, the device is disconnected from the PCI bus for all accesses except Configuration Space access.
    Class Code:      A read-only register that specifies the type of function the device performs.
    Subclass:        A read-only register that specifies the specific function the device performs.
    Prog IF:         A read-only register that specifies a register-level programming interface the device has, if it has any at all.
    Revision ID:     Specifies a revision identifier for a particular device. Where valid IDs are allocated by the vendor.
    BIST:            Represents that status and allows control of a devices BIST (built-in self test).
    Header Type:     Identifies the layout of the rest of the header beginning at byte 0x10 of the header and also specifies whether or not the device has multiple functions. Where a value of 0x00 specifies a general device, a value of 0x01 specifies a PCI-to-PCI bridge, and a value of 0x02 specifies a CardBus bridge. If bit 7 of this register is set, the device has multiple functions; otherwise, it is a single function device.
    Latency Timer:   Specifies the latency timer in units of PCI bus clocks.
    Cache Line Size: Specifies the system cache line size in 32-bit units. A device can limit the number of cacheline sizes it can support, if a unsupported value is written to this field, the device will behave as if a value of 0 was written.
*/
typedef struct 
{
    uint16_t mVendorID;
    uint16_t mDeviceID;

    uint16_t mCommand;
    uint16_t mStatus;

    uint8_t mRevisionID;
    uint8_t mProgIF;
    uint8_t mSubclass;
    uint8_t mClassCode;

    uint8_t mCacheLineSize;
    uint8_t mLatencyTimer;
    uint8_t mHeaderType;
    uint8_t mBIST;
} tkPCI_DeviceHeaderCommon;

/*
The following field descriptions apply if the Header Type is 0x00:
    CardBus CIS Pointer:  Points to the Card Information Structure and is used by devices that share silicon between CardBus and PCI.
    Interrupt Line:       Specifies which input of the system interrupt controllers the device's interrupt pin is connected to and is implemented by any device that makes use of an interrupt pin. For the x86 architecture this register corresponds to the PIC IRQ numbers 0-15 (and not I/O APIC IRQ numbers) and a value of 0xFF defines no connection.
    Interrupt Pin:        Specifies which interrupt pin the device uses. Where a value of 0x01 is INTA#, 0x02 is INTB#, 0x03 is INTC#, 0x04 is INTD#, and 0x00 means the device does not use an interrupt pin.
    Max Latency:          A read-only register that specifies how often the device needs access to the PCI bus (in 1/4 microsecond units).
    Min Grant:            A read-only register that specifies the burst period length, in 1/4 microsecond units, that the device needs (assuming a 33 MHz clock rate).
    Capabilities Pointer: Points to a linked list of new capabilities implemented by the device. Used if bit 4 of the status register (Capabilities List bit) is set to 1. The bottom two bits are reserved and should be masked before the Pointer is used to access the Configuration Space.
 */
typedef struct 
{
    tkPCI_DeviceHeaderCommon mCommonHeader;
    uint32_t mBaseAddress0;
    uint32_t mBaseAddress1;
    uint32_t mBaseAddress2;
    uint32_t mBaseAddress3;
    uint32_t mBaseAddress4;
    uint32_t mBaseAddress5;
    uint32_t mCardbusCISPointer;
    uint16_t mSubsustemVendorID;
    uint16_t mSubsystemID;
    uint32_t mExpansionRomBaseAddress;
    uint8_t  mCapabilitiesPointer;
    uint8_t  mReserved_1_8b;
    uint16_t mReserved_2_16b;
    uint32_t mReserved_3_32b;
    uint8_t  mInterruptLine;
    uint8_t  mInterruptPin;
    uint8_t  mMinGrant;
    uint8_t  mMaxLatency;
} tkPCI_DeviceHeaderType0;

typedef struct 
{
    tkPCI_DeviceHeaderCommon mCommonHeader;
    uint32_t mBaseAddress0;
    uint32_t mBaseAddress1;
    uint8_t  mPrimaryBusNumber;
    uint8_t  mSecondaryBusNumber;
    uint8_t  mSubordinateBusNumber;
    uint8_t  mSecondaryLatencyTimer;
    uint8_t  mIOBase;
    uint8_t  mIOLimit;
    uint16_t mSecondaryStatus;
    uint16_t mMemoryBase;
    uint16_t mMemoryLimit;
    uint16_t mPrefetchableMemoryBase;
    uint16_t mPrefetchableMemoryLimit;
    uint32_t mPrefetchableBaseUpper32b;
    uint32_t mPrefetchableLimitUpper32b;
    uint16_t mIOBaseUpper16b;
    uint16_t mIOLimitUpper16b;
    uint8_t  mCapabilitiesPointer;
    uint8_t  mReserved_1_8b;
    uint16_t mReserved_2_16b;
    uint32_t mExpansionRomBaseAddress;
    uint8_t  mInterruptLine;
    uint8_t  mInterruptPin;
    uint16_t mBridgeControl;
} tkPCI_DeviceHeaderType1;



void tkPCI_CheckAllBuses();
void tkPCI_CheckBus(uint8_t bus);
void tkPCI_CheckDevice(uint8_t bus, uint8_t device) ;
void tkPCI_CheckFunction(uint8_t bus, uint8_t device, uint8_t function);

uint16_t tkPCI_ReadConfigWord(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset);
uint8_t  tkPCI_GetHeaderType(uint8_t bus , uint8_t device , uint8_t function);
uint16_t tkPCI_GetVendorID(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  tkPCI_GetBaseClass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  tkPCI_GetSubClass(uint8_t bus, uint8_t device, uint8_t function);
uint8_t  tkPCI_GetSecondaryBus(uint8_t bus, uint8_t device, uint8_t function);
