/*
    You absolute beauty:
        https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
        http://www.osdever.net/FreeVGA/home.htm
*/
#pragma once

#include <stdint.h>
#include <stdbool.h>

struct VgaDriver
{
    bool Debug;
    uint32_t VbeControlInfo;
    uint32_t VbeModeInfo;
    uint16_t VbeMode;
    uint16_t VbeInterfaceSegment;
    uint16_t VbeInterfaceOffset;
    uint16_t VbeInterfaceLength;

    uint32_t FramebufferAddress;
    uint32_t FramebufferPitch;
    uint32_t FramebufferWidth;
    uint32_t FramebufferHeight;
    uint8_t  FramebufferBpp;
};
typedef struct VgaDriver VgaDriver;

bool VgaDriver_Constructor(VgaDriver* self);
void VgaDriver_Destructor(VgaDriver* self);
void VgaDriver_ClearScreen(VgaDriver* self);
void VgaDriver_WaitForVBlankStart(VgaDriver* self);
void VgaDriver_WaitForVBlankEnd(VgaDriver* self);
void VgaDriver_DebugVbeInfo(VgaDriver* self);
void VgaDriver_WritePixel(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
uint32_t VgaDriver_ReadPixel(VgaDriver* self, uint32_t x, uint32_t y);