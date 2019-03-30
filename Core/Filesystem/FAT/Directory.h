#pragma once

#include <stdint.h>

struct FatVolume;

#define FAT_DIR_ATTR_READ_ONLY 0x01 
#define FAT_DIR_ATTR_HIDDEN 0x02 
#define FAT_DIR_ATTR_SYSTEM 0x04 
#define FAT_DIR_ATTR_VOLUME_ID 0x08 
#define FAT_DIR_ATTR_DIRECTORY 0x10 
#define FAT_DIR_ATTR_ARCHIVE 0x20 
#define FAT_DIR_ATTR_LFN (   \
    FAT_DIR_ATTR_READ_ONLY | \
    FAT_DIR_ATTR_HIDDEN    | \
    FAT_DIR_ATTR_SYSTEM    | \
    FAT_DIR_ATTR_VOLUME_ID)

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

typedef struct
{
   /* Offset */
   /*00*/ uint8_t  Name[8]; 
   /*08*/ uint8_t  Extension[3];
   /*11*/ uint8_t  Attributes; 
   /*12*/ uint8_t  Reserved1;
   /*13*/ uint8_t  CreationTimeMilliSeconds;
   /*14*/ uint16_t CreationTime;
   /*16*/ uint16_t CreationDate;
   /*18*/ uint16_t LastAccessedDate;
   /*20*/ uint16_t Reserved2;
   /*22*/ uint16_t LastModificationTime;
   /*24*/ uint16_t LastModificationDate;
   /*26*/ uint16_t FirstClusterNumber;
   /*28*/ uint32_t FileSize;
} __attribute__((packed))
FatDirectory;

typedef struct
{
    uint8_t Index;
    uint8_t Characters[10];
    uint8_t Attributes;
    uint8_t EntryType;
    uint8_t Checksum;
    uint8_t Characters2[12];
    uint16_t Zero;
    uint8_t Characters3[4];
} 
__attribute__((packed))
FatLongFileName;

void FatDirectory_Debug(FatDirectory* fd);
void FatDirectory_DebugAttributes(uint8_t attributes);
uint8_t FatDirectory_HasAttribute(FatDirectory* dir, uint8_t attr);