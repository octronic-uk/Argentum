#pragma once

#include <stdint.h>

struct ACPI_RsdpDescriptorV1
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    void* RsdtAddress;
} __attribute__ ((packed)) 
;

struct ACPI_RsdpDescriptorV2
{
    struct ACPI_RsdpDescriptorV1 V1Descriptor;
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} __attribute__ ((packed));

struct ACPI_SDTHeader
{
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__((packed));

struct ACPI_GenericAddressStructure
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
} __attribute__((packed));

struct ACPI_FADT
{
    struct ACPI_SDTHeader h;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
 
    // field used in ACPI 1.0; no longer in use, for compatibility only
    uint8_t  Reserved;
 
    uint8_t  PreferredPowerManagementProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t  AcpiEnable;
    uint8_t  AcpiDisable;
    uint8_t  S4BIOS_REQ;
    uint8_t  PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t  PM1EventLength;
    uint8_t  PM1ControlLength;
    uint8_t  PM2ControlLength;
    uint8_t  PMTimerLength;
    uint8_t  GPE0Length;
    uint8_t  GPE1Length;
    uint8_t  GPE1Base;
    uint8_t  CStateControl;
    uint16_t WorstC2Latency;
    uint16_t WorstC3Latency;
    uint16_t FlushSize;
    uint16_t FlushStride;
    uint8_t  DutyOffset;
    uint8_t  DutyWidth;
    uint8_t  DayAlarm;
    uint8_t  MonthAlarm;
    uint8_t  Century;
 
    // reserved in ACPI 1.0; used since ACPI 2.0+
    uint16_t BootArchitectureFlags;
 
    uint8_t  Reserved2;
    uint32_t Flags;
 
    // 12 byte structure; see below for details
    struct ACPI_GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
 
    struct ACPI_GenericAddressStructure X_PM1aEventBlock;
    struct ACPI_GenericAddressStructure X_PM1bEventBlock;
    struct ACPI_GenericAddressStructure X_PM1aControlBlock;
    struct ACPI_GenericAddressStructure X_PM1bControlBlock;
    struct ACPI_GenericAddressStructure X_PM2ControlBlock;
    struct ACPI_GenericAddressStructure X_PMTimerBlock;
    struct ACPI_GenericAddressStructure X_GPE0Block;
    struct ACPI_GenericAddressStructure X_GPE1Block;
}  __attribute__((packed));

struct ACPI_RSDT 
{
    struct ACPI_SDTHeader h;
    uint32_t PointerToOtherSDT; // (h.Length - sizeof(h)) / 4;
};

struct ACPI_XSDT
{
    struct ACPI_SDTHeader h;
    uint64_t PointerToOtherSDT; // (h.Length - sizeof(h)) / 8;
};

struct ACPI_FACS
{
    char     Signature[4];           /* ASCII table signature */
    uint32_t Length;                 /* Length of structure, in bytes */
    uint32_t HardwareSignature;      /* Hardware configuration signature */
    uint32_t FirmwareWakingVector;   /* 32-bit physical address of the Firmware Waking Vector */
    uint32_t GlobalLock;             /* Global Lock for shared hardware resources */
    uint32_t Flags;
    uint64_t XFirmwareWakingVector;  /* 64-bit version of the Firmware Waking Vector (ACPI 2.0+) */
    uint8_t  Version;                /* Version of this table (ACPI 2.0+) */
    uint8_t  Reserved[3];            /* Reserved, must be zero */
    uint32_t OspmFlags;              /* Flags to be set by OSPM (ACPI 4.0) */
    uint8_t  Reserved1[24];          /* Reserved, must be zero */

}__attribute__((packed));

struct  ACPI_MADT
{
    struct ACPI_SDTHeader h;
    uint32_t LocalAPICAddress;
    uint32_t Flags;
    uint8_t RecordsStart;
};

struct ACPI_MADTRecordBase
{
    uint8_t RecordType;
    uint8_t Length;
}__attribute__((packed));

struct ACPI_MADTRecordApic
{
    struct ACPI_MADTRecordBase Base;
    uint8_t AcpiProcessorId;
    uint8_t AcpiId;
    uint32_t Flags;
} __attribute__((packed));

struct ACPI_MADTRecordIoApic
{
    struct ACPI_MADTRecordBase Base;
    uint8_t IoApicId;
    uint8_t Reserved;
    uint32_t IoApicAddress;
    uint32_t GlobalSystemInterruptBase; 
} __attribute__((packed));

struct ACPI_MADTRecordInputSourceOverride
{
    struct ACPI_MADTRecordBase Base;
    uint8_t BusSource;
    uint8_t IrqSource;
    uint32_t GlobalSystemInterrupt;
    uint16_t Flags;
} __attribute__((packed));

struct ACPI_MADTRecordNonMaskableInterrupt
{
    struct ACPI_MADTRecordBase Base;
    uint8_t ApicProcessorId;
    uint16_t Flags;
    uint8_t LintNumber;
} __attribute__((packed));

struct ACPI_MADTRecordLocalApicAddressOverride
{
    struct ACPI_MADTRecordBase Base;
    uint8_t Reserved;
    uint64_t LocalApic;
} __attribute__((packed));

union ACPI_IoApicRedirectionEntry
{
    struct
    {
        uint64_t vector       : 8;
        uint64_t delvMode     : 3;
        uint64_t destMode     : 1;
        uint64_t delvStatus   : 1;
        uint64_t pinPolarity  : 1;
        uint64_t remoteIRR    : 1;
        uint64_t triggerMode  : 1;
        uint64_t mask         : 1;
        uint64_t reserved     : 39;
        uint64_t destination  : 8;
    };
    struct
    {
        uint32_t lowerDword;
        uint32_t upperDword;
    };
};

struct ACPI_IoApic
{
    uint32_t IoApicAddress;
    uint32_t Id;
    uint32_t Version;
    uint32_t Arbitration;
    // Redirects
    uint8_t IrqRedirectEntries;
    union ACPI_IoApicRedirectionEntry IrqRedirections[24];
};