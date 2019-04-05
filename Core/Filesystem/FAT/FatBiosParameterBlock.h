#pragma once

#include <stdint.h>
#include <Filesystem/FAT/FatType.h>

/* 
 *	https://wiki.osdev.org/FAT 
 */

#define FAT_JMP_BYTES    {0xEB, 0x3C, 0x90 }
#define FAT_OEM_ID_BYTES {'m','k','d','o','s','f','s',0}
#define FAT_NUM_FATS     2
#define FAT_READ_ONLY    0x01
#define FAT_HIDDEN       0x02
#define FAT_SYSTEM       0x04
#define FAT_VOLUME_ID    0x08
#define FAT_DIRECTORY    0x10
#define FAT_ARCHIVE      0x20
#define FAT_LFN          FAT_READ_ONLY | FAT_HIDDEN | FAT_SYSTEM | FAT_VOLUME_ID

/*
	Offset   Size(b)  Meaning

	0/0x00	 3      The first three bytes EB 3C 90 disassemble to JMP SHORT 3C NOP. (The 3C value 
					may be different.) The reason for this is to jump over the disk format information 
					(the BPB and EBPB). Since the first sector of the disk is loaded into ram at 
					location 0x0000:0x7c00 and executed, without this jump, the processor would 
					attempt to execute data that isn't code. Even for non-bootable volumes, 
					code matching this pattern (or using the E9 jump opcode) is required to be 
					present by both Windows and OS X. To fulfil this requirement, an infinite 
					loop can be placed here with the bytes EB FE 90.

	3/0x03	 8      OEM identifier. The first 8 Bytes (3 - 10) is the version of DOS being used. 
					The next eight Bytes 29 3A 63 7E 2D 49 48 and 43 read out the name of the 
					version. The official FAT Specification from Microsoft says that this field is 
					really meaningless and is ignored by MS FAT Drivers, however it does recommend 
					the value "MSWIN4.1" as some 3rd party drivers supposedly check it and expect it
					to have that value. Older versions of dos also report MSDOS5.1, linux-formatted
					floppy will likely to carry "mkdosfs" here, and FreeDOS formatted disks have 
					been observed to have "FRDOS5.1" here. If the string is less than 8 bytes, it is
					padded with spaces.

	11/0x0B  2      The number of Bytes per sector (remember, all numbers are in the little-endian 
					format).

	13/0x0D  1      Number of sectors per cluster.

	14/0x0E  2      Number of reserved sectors. The boot record sectors are included in this value.

	16/0x10  1      Number of File Allocation Tables (FAT's) on the storage media. Often this value is 2.

	17/0x11  2      Number of directory entries (must be set so that the root directory occupies 
					entire sectors).

	19/0x13  2      The total sectors in the logical volume. If this value is 0, it means there are 
					more than 65535 sectors in the volume, and the actual count is stored in the 
					Large Sector Count entry at 0x20.

	21/0x15  1      This Byte indicates the media descriptor type.

	22/0x16  2      Number of sectors per FAT. FAT12/FAT16 only.

	24/0x18  2      Number of sectors per track.

	26/0x1A  2      Number of heads or sides on the storage media.

	28/0x1C  4      Number of hidden sectors. (i.e. the LBA of the beginning of the partition.)

	32/0x20  4      Large sector count. This field is set if there are more than 65535 sectors in 

					the volume, resulting in a value which does not fit in the Number of Sectors 
					entry at 0x16.
*/

struct FatBiosParameterBlock
{
	uint8_t  Jmp[3];
	uint8_t  OEMIdentifier[8];
	uint16_t BytesPerSector;
	uint8_t  SectorsPerCluster;
	uint16_t ReservedSectorCount;
	uint8_t  TableCount;
	uint16_t RootEntryCount;
	uint16_t TotalSectors16;
	uint8_t  MediaType;
	uint16_t TableSize16;
	uint16_t SectorsPerTrack;
	uint16_t HeadSideCount;
	uint32_t HiddenSectorCount;
	uint32_t TotalSectors32;
	uint8_t  ExtendedBootRecord[54];
} __attribute__((packed));

/*
	Offset      Length(b)	Meaning
	36/0x024	1	        Drive number. The value here should be identical to the value returned 
							by BIOS interrupt 0x13, or passed in the DL register; i.e. 0x00 for a 
							floppy disk and 0x80 for hard disks. This number is useless because 
							the media is likely to be moved to another machine and inserted in a
							drive with a different drive number.
	
	37/0x025	1	        Flags in Windows NT. Reserved otherwise.
	
	38/0x026	1	        Signature (must be 0x28 or 0x29).
	
	39/0x027	4	        VolumeID 'Serial' number. Used for tracking volumes between computers. 
							You can ignore this if you want.
	
	43/0x02B	11	        Volume label string. This field is padded with spaces.
	
	54/0x036	8	        System identifier string. This field is a string representation of the 
							FAT file system type. It is padded with spaces. The spec says never to 
							trust the contents of this string for any use.
	
	62/0x03E	448     	Boot code.
	
	510/0x1FE	2	        Bootable partition signature 0xAA55.
*/
struct FatExtendedBootRecord16
{
	uint8_t  DriveNumber;
	uint8_t  Flags;
	uint8_t  Signature;
	uint32_t VolumeSerialNumber;
	uint8_t  VolumeLabelString[11];
	uint8_t  SystemIdentifierString[8];
	uint8_t  BootCode[448]; 
	uint16_t BootPartitionSignature;
} __attribute__((packed));

/*
	Offset      Length(b)   Meaning
	36/0x024	4           Sectors per FAT. The size of the FAT in sectors.
	
	40/0x028	2           Flags.
	
	42/0x02A	2           FAT version number. The high byte is the major version and the low byte 
							is the minor version. FAT drivers should respect this field.
	
	44/0x02C	4           The cluster number of the root directory. Often this field is set to 2.
	
	48/0x030	2           The sector number of the FSInfo structure.
	
	50/0x032	2           The sector number of the backup boot sector.
	
	52/0x034	12          Reserved. When the volume is formated these bytes should be zero.
	
	64/0x040	1           Drive number. The values here are identical to the values returned by 
							the BIOS interrupt 0x13. 0x00 for a floppy disk and 0x80 for hard disks.
	
	65/0x041	1           Flags in Windows NT. Reserved otherwise.
	
	66/0x042	1           Signature (must be 0x28 or 0x29).
	
	67/0x043	4           Volume ID 'Serial' number. Used for tracking volumes between computers. 
							You can ignore this if you want.
	
	71/0x047	11          Volume label string. This field is padded with spaces.
	
	82/0x052	8           System identifier string. Always "FAT32   ". The spec says never to 
							trust the contents of this string for any use.
	
	90/0x05A	420         Boot code.
	
	510/0x1FE	2           Bootable partition signature 0xAA55.
*/
struct FatExtendedBootRecord32
{
	uint32_t SectorsPerFAT;
	uint16_t Flags;
	uint16_t FATVersionNumber;
	uint32_t RootDirectoryClusterNumber;
	uint16_t FSInfoSectorNumber;
	uint16_t BackupBootSectorNumber;
	uint8_t  reserved[12];
	uint8_t  DriveNumber;
	uint8_t  NTFlags;
	uint8_t  Signature;
	uint32_t SerialNumber;
	uint8_t  VolumeLabel[11];
	uint8_t  SystemIdentifier[8];
	uint8_t  BootCode[420]; 
	uint16_t BootPartitionSignature;
} __attribute__((packed));

void FatBPB_Debug(struct FatBiosParameterBlock* self);
void FatBPB_DebugEBR16(struct FatExtendedBootRecord16* ebr16);
void FatBPB_DebugEBR32(struct FatExtendedBootRecord32* ebr32);

uint32_t FatBPB_GetTotalSectors(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetFatSizeInSectors(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetRootDirectorySizeInSectors(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetFirstDataSector(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetFirstFATSector(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetTotalDataSectors(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetTotalClusters(struct FatBiosParameterBlock* self);
uint32_t FatBPB_GetFirstRootDirectorySector(struct FatBiosParameterBlock* self);
enum FatType FatBPB_GetFATType(struct FatBiosParameterBlock* self);