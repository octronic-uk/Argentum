#pragma once

#include <stdint.h>
#include <stdbool.h>

struct FatVolume;

struct FatDirectoryCluster
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

void FatDirectoryCluster_Debug(struct FatDirectoryCluster* self);
void FatDirectoryCluster_DebugAttributes(struct FatDirectoryCluster* self);
bool FatDirectoryCluster_HasAttribute(struct FatDirectoryCluster* self, uint8_t attr);

char* FatDirectoryCluster_GetDirectoryTypeString(struct FatDirectoryCluster* self);