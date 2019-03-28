#pragma once

#include <stdint.h>

#define ACPI_EBDA_POINTER_ADDR 0x040E
#define ACPI_BIOS_SEARCH_BEGIN 0x000E0000
#define ACPI_BIOS_SEARCH_END   0x000FFFFF
#define ACPI_BIOS_SEARCH_STEP  0x10

#define ACPI_RSDP_HEADER_STR_LENGTH  8

#define ACPI_GAS_ADDRESS_SPACE_SYSTEM_MEMORY 0x00
#define ACPI_GAS_ADDRESS_SPACE_SYSTEM_IO     0x01
#define ACPI_GAS_ADDRESS_SPACE_PCI_CFG       0x02
#define ACPI_GAS_ADDRESS_SPACE_EMB_CTRLR     0x03
#define ACPI_GAS_ADDRESS_SPACE_SMBUS         0x04
#define ACPI_GAS_ADDRESS_SPACE_FF_HW         0x7F
#define ACPI_GAS_ADDRESS_SPACE_SYSTEM_MEMORY 0x00
#define ACPI_GAS_ADDRESS_SPACE_OEM           0xC0

#define ACPI_GAS_ACCESS_SIZE_1B  0x01
#define ACPI_GAS_ACCESS_SIZE_16B 0x02
#define ACPI_GAS_ACCESS_SIZE_32B 0x03
#define ACPI_GAS_ACCESS_SIZE_64B 0x04

/* Masks for FADT ARM Boot Architecture Flags (arm_boot_flags) ACPI 5.1 */
#define ACPI_FADT_PSCI_COMPLIANT    (1)         /* 00: [V5+] PSCI 0.2+ is implemented */
#define ACPI_FADT_PSCI_USE_HVC      (1<<1)      /* 01: [V5+] HVC must be used instead of SMC as the PSCI conduit */

/* Masks for FADT flags */
#define ACPI_FADT_WBINVD            (1)         /* 00: [V1] The WBINVD instruction works properly */
#define ACPI_FADT_WBINVD_FLUSH      (1<<1)      /* 01: [V1] WBINVD flushes but does not invalidate caches */
#define ACPI_FADT_C1_SUPPORTED      (1<<2)      /* 02: [V1] All processors support C1 state */
#define ACPI_FADT_C2_MP_SUPPORTED   (1<<3)      /* 03: [V1] C2 state works on MP system */
#define ACPI_FADT_POWER_BUTTON      (1<<4)      /* 04: [V1] Power button is handled as a control method device */
#define ACPI_FADT_SLEEP_BUTTON      (1<<5)      /* 05: [V1] Sleep button is handled as a control method device */
#define ACPI_FADT_FIXED_RTC         (1<<6)      /* 06: [V1] RTC wakeup status is not in fixed register space */
#define ACPI_FADT_S4_RTC_WAKE       (1<<7)      /* 07: [V1] RTC alarm can wake system from S4 */
#define ACPI_FADT_32BIT_TIMER       (1<<8)      /* 08: [V1] ACPI timer width is 32-bit (0=24-bit) */
#define ACPI_FADT_DOCKING_SUPPORTED (1<<9)      /* 09: [V1] Docking supported */
#define ACPI_FADT_RESET_REGISTER    (1<<10)     /* 10: [V2] System reset via the FADT RESET_REG supported */
#define ACPI_FADT_SEALED_CASE       (1<<11)     /* 11: [V3] No internal expansion capabilities and case is sealed */
#define ACPI_FADT_HEADLESS          (1<<12)     /* 12: [V3] No local video capabilities or local input devices */
#define ACPI_FADT_SLEEP_TYPE        (1<<13)     /* 13: [V3] Must execute native instruction after writing  SLP_TYPx register */
#define ACPI_FADT_PCI_EXPRESS_WAKE  (1<<14)     /* 14: [V4] System supports PCIEXP_WAKE (STS/EN) bits (ACPI 3.0) */
#define ACPI_FADT_PLATFORM_CLOCK    (1<<15)     /* 15: [V4] OSPM should use platform-provided timer (ACPI 3.0) */
#define ACPI_FADT_S4_RTC_VALID      (1<<16)     /* 16: [V4] Contents of RTC_STS valid after S4 wake (ACPI 3.0) */
#define ACPI_FADT_REMOTE_POWER_ON   (1<<17)     /* 17: [V4] System is compatible with remote power on (ACPI 3.0) */
#define ACPI_FADT_APIC_CLUSTER      (1<<18)     /* 18: [V4] All local APICs must use cluster model (ACPI 3.0) */
#define ACPI_FADT_APIC_PHYSICAL     (1<<19)     /* 19: [V4] All local xAPICs must use physical dest mode (ACPI 3.0) */
#define ACPI_FADT_HW_REDUCED        (1<<20)     /* 20: [V5] ACPI hardware is not implemented (ACPI 5.0) */
#define ACPI_FADT_LOW_POWER_S0      (1<<21)     /* 21: [V5] S0 power savings are equal or better than S3 (ACPI 5.0) */

const static char* ACPI_SIG_APIC = "APIC"; // Multiple APIC Description Table (MADT)
const static char* ACPI_SIG_BERT = "BERT"; // Boot Error Record Table (BERT)
const static char* ACPI_SIG_CPEP = "CPEP"; // Corrected Platform Error Polling Table (CPEP)
const static char* ACPI_SIG_DSDT = "DSDT"; // Differentiated System Description Table (DSDT)
const static char* ACPI_SIG_ECDT = "ECDT"; // Embedded Controller Boot Resources Table (ECDT)
const static char* ACPI_SIG_EINJ = "EINJ"; // Error Injection Table (EINJ)
const static char* ACPI_SIG_ERST = "ERST"; // Error Record Serialization Table (ERST)
const static char* ACPI_SIG_FACP = "FACP"; // Fixed ACPI Description Table (FADT)
const static char* ACPI_SIG_FACS = "FACS"; // Firmware ACPPI Control Structure (FACS)
const static char* ACPI_SIG_HEST = "HEST"; // Hardware Error Source Table (HEST)
const static char* ACPI_SIG_MSCT = "MSCT"; // Maximum System Characteristics Table (MSCT)
const static char* ACPI_SIG_MPST = "MPST"; // Memory Power State Table (MPST)
const static char* ACPI_SIG_OEMx = "OEMx"; // OEM Specific Information Tables (Any table with a signature beginning with "OEM" falls into this definition)
const static char* ACPI_SIG_PMTT = "PMTT"; // Platform Memory Topology Table (PMTT)
const static char* ACPI_SIG_PSDT = "PSDT"; // Persistent System Description Table (PSDT)
const static char* ACPI_SIG_RASF = "RASF"; // ACPI RAS FeatureTable (RASF)
const static char* ACPI_SIG_RSDT = "RSDT"; // Root System Description Table (This wiki page; included for completeness)
const static char* ACPI_SIG_SBST = "SBST"; // Smart Battery Specification Table (SBST)
const static char* ACPI_SIG_SLIT = "SLIT"; // System Locality System Information Table (SLIT)
const static char* ACPI_SIG_SRAT = "SRAT"; // System Resource Affinity Table (SRAT)
const static char* ACPI_SIG_SSDT = "SSDT"; // Secondary System Description Table (SSDT)
const static char* ACPI_SIG_XSDT = "XSDT"; // Extended System Description Table (XSDT; 64-bit version of the RSDT)

const static char* ACPI_RSDP_HEADER_STR  = "RSD PTR ";

/* Masks for FADT IA-PC Boot Architecture Flags (boot_flags) [Vx]=Introduced in this FADT revision */
#define ACPI_FADT_LEGACY_DEVICES    (1)         /* 00: [V2] System has LPC or ISA bus devices */
#define ACPI_FADT_8042              (1<<1)      /* 01: [V3] System has an 8042 controller on port 60/64 */
#define ACPI_FADT_NO_VGA            (1<<2)      /* 02: [V4] It is not safe to probe for VGA hardware */
#define ACPI_FADT_NO_MSI            (1<<3)      /* 03: [V4] Message Signaled Interrupts (MSI) must not be enabled */
#define ACPI_FADT_NO_ASPM           (1<<4)      /* 04: [V4] PCIe ASPM control must not be enabled */
#define ACPI_FADT_NO_CMOS_RTC       (1<<5)      /* 05: [V5] No CMOS real-time clock present */

/* MADT Record Flags */
#define ACPI_MADT_RECORD_TYPE_PROC_LOCAL_APIC 0x00
#define ACPI_MADT_RECORD_TYPE_IOAPIC          0x01
#define ACPI_MADT_RECORD_TYPE_ISO             0x02
#define ACPI_MADT_RECORD_TYPE_NMI             0x04
#define ACPI_MADT_RECORD_TYPE_LOCAL_APIC_OVR  0x05

#define ACPI_MADT_FLAGS_ACTIVE_LOW      0x02
#define ACPI_MADT_FLAGS_LEVEL_TRIGGERED 0x08

typedef struct  
{
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    void* RsdtAddress;
} __attribute__ ((packed)) 
ACPI_RsdpDescriptorV1;

typedef struct  
{
    ACPI_RsdpDescriptorV1 V1Descriptor;
    uint32_t Length;
    uint64_t XsdtAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
} __attribute__ ((packed))
ACPI_RsdpDescriptorV2;

typedef struct  
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
} __attribute__((packed)) 
ACPI_SDTHeader;

typedef struct 
{
  uint8_t AddressSpace;
  uint8_t BitWidth;
  uint8_t BitOffset;
  uint8_t AccessSize;
  uint64_t Address;
} __attribute__((packed))
ACPI_GenericAddressStructure;

typedef struct
{
    ACPI_SDTHeader h;
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
    ACPI_GenericAddressStructure ResetReg;
 
    uint8_t  ResetValue;
    uint8_t  Reserved3[3];
 
    // 64bit pointers - Available on ACPI 2.0+
    uint64_t                X_FirmwareControl;
    uint64_t                X_Dsdt;
 
    ACPI_GenericAddressStructure X_PM1aEventBlock;
    ACPI_GenericAddressStructure X_PM1bEventBlock;
    ACPI_GenericAddressStructure X_PM1aControlBlock;
    ACPI_GenericAddressStructure X_PM1bControlBlock;
    ACPI_GenericAddressStructure X_PM2ControlBlock;
    ACPI_GenericAddressStructure X_PMTimerBlock;
    ACPI_GenericAddressStructure X_GPE0Block;
    ACPI_GenericAddressStructure X_GPE1Block;
}  __attribute__((packed)) ACPI_FADT;

typedef struct  
{
    ACPI_SDTHeader h;
    uint32_t PointerToOtherSDT; // (h.Length - sizeof(h)) / 4;
} ACPI_RSDT;

typedef struct  
{
    ACPI_SDTHeader h;
    uint64_t PointerToOtherSDT; // (h.Length - sizeof(h)) / 8;
} ACPI_XSDT;

typedef struct
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

} ACPI_FACS;

typedef struct 
{
    ACPI_SDTHeader h;
    uint32_t LocalAPICAddress;
    uint32_t Flags;
    uint8_t RecordsStart;
} ACPI_MADT;

typedef struct
{
    uint8_t RecordType;
    uint8_t Length;
}__attribute__((packed))
ACPI_MADTRecordBase;

typedef struct
{
    ACPI_MADTRecordBase Base;
    uint8_t AcpiProcessorId;
    uint8_t AcpiId;
    uint32_t Flags;
} __attribute__((packed))
ACPI_MADTRecordApic;

typedef struct
{
    ACPI_MADTRecordBase Base;
    uint8_t IoApicId;
    uint8_t Reserved;
    uint32_t IoApicAddress;
    uint32_t GlobalSystemInterruptBase; 
} __attribute__((packed))
ACPI_MADTRecordIoApic;

typedef struct
{
    ACPI_MADTRecordBase Base;
    uint8_t BusSource;
    uint8_t IrqSource;
    uint32_t GlobalSystemInterrupt;
    uint16_t Flags;
} __attribute__((packed))
ACPI_MADTRecordInputSourceOverride;

typedef struct
{
    ACPI_MADTRecordBase Base;
    uint8_t ApicProcessorId;
    uint16_t Flags;
    uint8_t LintNumber;
} __attribute__((packed))
ACPI_MADTRecordNonMaskableInterrupt;

typedef struct
{
    ACPI_MADTRecordBase Base;
    uint8_t Reserved;
    uint64_t LocalApic;
} __attribute__((packed))
ACPI_MADTRecordLocalApicAddressOverride;

typedef union
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
} ACPI_IoApicRedirectionEntry;


typedef struct
{
    uint32_t IoApicAddress;
    uint32_t Id;
    uint32_t Version;
    uint32_t Arbitration;
    // Redirects
    uint8_t IrqRedirectEntries;
    ACPI_IoApicRedirectionEntry IrqRedirections[24];
} ACPI_IoApic;


void ACPI_Constructor();
void ACPI_Destructor();

void* ACPI_GetEbdaPointer();
void* ACPI_FindRsdpPointer();

uint8_t ACPI_CheckRsdpChecksum(ACPI_RsdpDescriptorV1* rsdp, uint8_t version);
uint8_t ACPI_CheckSDTChecksum(ACPI_SDTHeader *tableHeader);

void* ACPI_FindSDTBySignature(const char* sig, void* RootSDT, uint8_t version);

void ACPI_SetFacsPointer(uint32_t ptr);
void ACPI_SetSciInterrupt(uint16_t sci);
void ACPI_SetDsdtPointer(uint32_t dsdt);
void APCI_SetDebug(uint8_t debug);

void ACPI_ProcessMADT(ACPI_MADT* madt);

void ACPI_DebugIoApic(ACPI_MADTRecordIoApic* record);
void ACPI_DebugInterruptSourceOverride(ACPI_MADTRecordInputSourceOverride* record);
void ACPI_DebugMADT();