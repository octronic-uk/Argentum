#pragma once

#include <stdint.h>

struct MBRChsAddress3b
{
    uint8_t CHS0;
    uint8_t CHS1;
    uint8_t CHS2;
}__attribute__((packed));
typedef struct MBRChsAddress3b MBRChsAddress3b;

struct MBRChsAddress
{
    uint16_t Cylinder;
    uint8_t Head;
    uint8_t Sector;
}__attribute__((packed));
typedef struct MBRChsAddress MBRChsAddress;

void MBRChsAddress_From3b(MBRChsAddress* self, MBRChsAddress3b* data);
void MBRChsAddress_Debug(const char* name, MBRChsAddress addr);