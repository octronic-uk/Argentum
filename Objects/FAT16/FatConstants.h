#pragma once

/* 
    Sources

	https://wiki.osdev.org/FAT 
	https://infogalactic.com/info/Design_of_the_FAT_file_system
 */

#define FAT_LFN_NAME_SIZE 256
#define FAT_CACHED_SECTORS_SIZE 128
#define FAT_SECTOR_SIZE 512
#define FAT_CLUSTER_SIZE_B 32
#define FAT_DIR_LISTING_MAX_DIRS 128 

#define FAT_DIR_ATTR_READ_ONLY 0x01 
#define FAT_DIR_ATTR_HIDDEN    0x02 
#define FAT_DIR_ATTR_SYSTEM    0x04 
#define FAT_DIR_ATTR_VOLUME_ID 0x08 
#define FAT_DIR_ATTR_DIRECTORY 0x10 
#define FAT_DIR_ATTR_ARCHIVE   0x20 
#define FAT_DIR_ATTR_LFN FAT_DIR_ATTR_READ_ONLY|FAT_DIR_ATTR_HIDDEN|FAT_DIR_ATTR_SYSTEM|FAT_DIR_ATTR_VOLUME_ID

// Cluster Constants
#define FAT_END_OF_CHAIN_CLUSTER 0xFFFF
#define FAT_BAD_CLUSTER          0xF7FF

/*
    Time Encoding.
        Hour    5 bits
        Minutes 6 bits
        Secods  5 bits

    Date Encoding.
        Year  7 bits
        Month 4 bits
        Day	  5 bits
*/

/*
	8-inch (200 mm) Single sided, 77 tracks per side, 26 sectors per track, 128 bytes per sector 
	(250.25 KB) (DR-DOS only)
*/
#define BPB_MEDIA_TYPE_8INCH 0xE5 
/*
	5.25-inch (130 mm) Double sided, 80 tracks per side, 9 sector, 720 KB (Tandy 2000 only)
*/
#define BPB_MEDIA_TYPE_5QTR_INCH 0xED 
/*
	Designated for non-standard custom partitions (utilizing non-standard BPB formats or requiring 
	special media access such as 48-/64-bit addressing); corresponds with 0xF8, but not recognized 
	by unaware systems by design; value not required to be identical to FAT ID, never used as 
	cluster end-of-chain marker (Reserved for DR-DOS)
*/
#define BPB_MEDIA_TYPE_CUSTOM 0xEE 
/*
	Designated for non-standard custom superfloppy formats; corresponds with 0xF0, but not 
	recognized by unaware systems by design; value not required to be identical to FAT ID, never 
	used as cluster end-of-chain marker (Reserved for DR-DOS)
*/
#define BPB_MEDIA_TYPE_CUSTOM_SUPERFLOPPY 0xEF 

/*
	3.5-inch (90 mm) Double Sided, 80 tracks per side, 18 or 36 sectors per track (1440 KB, known as 
	“1.44 MB”; or 2880 KB, known as “2.88 MB”). Designated for use with custom floppy and superfloppy
	formats where the geometry is defined in the BPB. Used also for other media types such as tapes.
	
*/
#define BPB_MEDIA_TYPE_3HLF_INCH 0xF0

/*
	Double density (Altos MS-DOS 2.11 only)
*/
#define BPB_MEDIA_TYPE_3HLF_INCH_ALTOS 0xF4

/*
	Fixed disk, 4-sided, 12 sectors per track (1.95? MB) (Altos MS-DOS 2.11 only)[30]
*/
#define BPB_MEDIA_TYPE_FIXED_DISK_4_12 0xF5

/*
	Fixed disk (i.e., typically a partition on a hard disk). (since DOS 2.0)[31][32]
	Designated to be used for any partitioned fixed or removable media, where the geometry is defined in the BPB.
	3.5-inch Single sided, 80 tracks per side, 9 sectors per track (360 KB) (MS-DOS 3.1[12] and MSX-DOS)
	5.25-inch Double sided, 80 tracks per side, 9 sectors per track (720 KB) (Sanyo 55x DS-DOS 2.11 only)[18]
	Single sided (Altos MS-DOS 2.11 only)[30]
*/
#define BPB_MEDIA_TYPE_FIXED 0xF8

/*
	3.5-inch Double sided, 80 tracks per side, 9 sectors per track (720 KB) (since DOS 3.2)[31]
	3.5-inch Double sided, 80 tracks per side, 18 sectors per track (1440 KB) (DOS 3.2 only)[31]
	5.25-inch Double sided, 80 tracks per side, 15 sectors per track (1200 KB, known as “1.2 MB”) (since DOS 3.0)[31]
	Single sided (Altos MS-DOS 2.11 only)[30]
*/

#define BPB_MEDIA_TYPE_3HLF_INCH_DS 0xF9

/*
	3.5-inch and 5.25-inch Single sided, 80 tracks per side, 8 sectors per track (320 KB)
	Used also for RAM disks and ROM disks (e.g., on Columbia Data Products[33] and on HP 200LX)
	Hard disk (Tandy MS-DOS only)
*/
#define BPB_MEDIA_TYPE_FLOPPY_SS 0xFA

/*
	3.5-inch and 5.25-inch Double sided, 80 tracks per side, 8 sectors per track (640 KB)
*/
#define BPB_MEDIA_TYPE_FLOPPY_DS 0xFB

/*
	5.25-inch Single sided, 40 tracks per side, 9 sectors per track (180 KB) (since DOS 2.0)[31]
*/
#define BPB_MEDIA_TYPE_5QTR_SS 0xFC

/*
	5.25-inch Double sided, 40 tracks per side, 9 sectors per track (360 KB) (since DOS 2.0)[31]
	8-inch Double sided, 77 tracks per side, 26 sectors per track, 128 bytes per sector (500.5 KB)
	(8-inch Double sided, (single and) double density (DOS 1)[31])
*/
#define BPB_MEDIA_TYPE_5QTR_DS 0xFD

/*
	5.25-inch Single sided, 40 tracks per side, 8 sectors per track (160 KB) (since DOS 1.0)[31][34]
	8-inch Single sided, 77 tracks per side, 26 sectors per track, 128 bytes per sector (250.25 KB)[30][34]
	8-inch Double sided, 77 tracks per side, 8 sectors per track, 1024 bytes per sector (1232 KB)[34]
	(8-inch Single sided, (single and) double density (DOS 1)[31])
*/
#define BPB_MEDIA_TYPE_5QTR_SS_40 0xFE

/*
	5.25-inch Double sided, 40 tracks per side, 8 sectors per track (320 KB) (since DOS 1.1)[31][34]
	Hard disk (Sanyo 55x DS-DOS 2.11 only)[18]

	This value must reflect the media descriptor stored (in the entry for cluster 0) in the first 
	byte of each copy of the FAT. Certain operating systems before DOS 3.2 (86-DOS, MS-DOS/PC DOS 
	1.x and MSX-DOS version 1.0) ignore the boot sector parameters altogether and use the media 
	descriptor value from the first byte of the FAT to choose among internally pre-defined parameter
	templates. Must be greater or equal to 0xF0 since DOS 4.0.[11]

	On removable drives, DR-DOS will assume the presence of a BPB if this value is greater or equal 
	to 0xF0,[11] whereas for fixed disks, it must be 0xF8 to assume the presence of a BPB.

	Initially, these values were meant to be used as bit flags; for any removable media without a 
	recognized BPB format and a media descriptor of either 0xF8 or 0xFA to 0xFF MS-DOS/PC DOS treats 
	bit 1 as a flag to choose a 9-sectors per track format rather than an 8-sectors format, and bit 
	0 as a flag to indicate double-sided media.[12] Values 0x00 to 0xEF and 0xF1 to 0xF7 are 
	reserved and must not be used.
*/
#define BPB_MEDIA_TYPE_DS_40_8 0xFF

#define FAT_LFN_NAME_1_SIZE  10
#define FAT_LFN_NAME_2_SIZE  12
#define FAT_LFN_NAME_3_SIZE  4
#define FAT_LFN_NAME_FULL_SIZE (FAT_LFN_NAME_1_SIZE+FAT_LFN_NAME_2_SIZE+FAT_LFN_NAME_3_SIZE+1)

#define FAT_LFN_ATTR_IS_LFN  0x0F
#define FAT_LFN_ENTRY_NUMBER 0x0F
#define FAT_LFN_LAST_ENTRY   0x40

#define FAT_VOLUME_CLUSTER_UNUSED 0xE5