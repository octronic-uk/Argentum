#pragma once

#include <stdint.h>

struct MBRChsAddress3b
{
    uint8_t CHS0;
    uint8_t CHS1;
    uint8_t CHS2;
}__attribute__((packed));

struct MBRChsAddress
{
    uint16_t Cylinder;
    uint8_t Head;
    uint8_t Sector;
}__attribute__((packed));

void MBRChsAddress_From3b(struct MBRChsAddress* self, struct MBRChsAddress3b* data);
void MBRChsAddress_Debug(const char* name, struct MBRChsAddress addr);