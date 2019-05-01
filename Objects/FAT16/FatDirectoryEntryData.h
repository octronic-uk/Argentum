#pragma once

#include <stdint.h>
#include <stdbool.h>

struct FatVolume;

struct FatDirectoryEntryData
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
} __attribute__((packed));

void FatDirectoryEntryData_Debug(struct FatDirectoryEntryData* self);
void FatDirectoryEntryData_DebugAttributes(struct FatDirectoryEntryData* self);
bool FatDirectoryEntryData_HasAttribute(struct FatDirectoryEntryData* self, uint8_t attr);

char* FatDirectoryEntryData_GetDirectoryTypeString(uint8_t attributes);