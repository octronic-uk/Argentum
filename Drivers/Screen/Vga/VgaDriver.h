/*
    You absolute beauty:
        https://files.osdev.org/mirrors/geezer/osd/graphics/modes.c
        http://www.osdever.net/FreeVGA/home.htm
*/
#pragma once

#include <stdint.h>
#include <stdbool.h>

struct VgaColor
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
};
typedef struct VgaColor VgaColor;

enum VgaScreenMode
{
	VGA_MODE_TEXT_40_25,
	VGA_MODE_TEXT_90_60,	
	VGA_MODE_PX_320_200_4,	 
	VGA_MODE_PX_320_200_256,	 
	VGA_MODE_PX_320_200_256_X,	 
	VGA_MODE_PX_640_480_16
};
typedef enum VgaScreenMode VgaScreenMode;

struct VgaDriver
{
    void (*WritePixelFunction)(struct VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
    VgaScreenMode ScreenMode;
    uint32_t Width;
    uint32_t Height;
};
typedef struct VgaDriver VgaDriver;

bool VgaDriver_Constructor(VgaDriver* self);

void VgaDriver_Dump(VgaDriver* self, uint8_t *regs, uint32_t count);
void VgaDriver_DumpRegs(VgaDriver* self, uint8_t *regs);
void VgaDriver_ReadRegs(VgaDriver* self, uint8_t *regs);
void VgaDriver_WriteRegs(VgaDriver* self, uint8_t *regs);
void VgaDriver_SetPlane(VgaDriver* self, uint32_t p);
uint32_t VgaDriver_GetFramebufferSegment(VgaDriver* self);
void VgaDriver_vmemwr(VgaDriver* self, uint32_t dst_off, uint8_t *src, uint32_t count);
void VgaDriver_vpokeb(VgaDriver* self, uint32_t off, uint32_t val);
uint32_t VgaDriver_vpeekb(VgaDriver* self, uint32_t off);
void VgaDriver_WriteFont(VgaDriver* self, uint8_t *buf, uint32_t font_height);
void VgaDriver_WritePixel1(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
void VgaDriver_WritePixel2(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
void VgaDriver_WritePixel4p(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
void VgaDriver_WritePixel8(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
void VgaDriver_WritePixel8x(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c);
void VgaDriver_DrawX(VgaDriver* self);
void VgaDriver_DumpState(VgaDriver* self);
void VgaDriver_SetTextMode(VgaDriver* self, bool hi_res);
void VgaDriver_DemoGraphics(VgaDriver* self);
uint8_t VgaDriver_ReverseBits(VgaDriver* self, uint8_t arg);
void VgaDriver_Font512(VgaDriver* self);
void VgaDriver_Main(VgaDriver* self);
void VgaDriver_SetScreenMode(VgaDriver* self, VgaScreenMode mode);
void VgaDriver_WriteDacColor(VgaDriver* self, uint8_t address, VgaColor color);
VgaColor VgaDriver_ReadDacColor(VgaDriver* self, uint8_t address);
void VgaDriver_WriteColorPalette(VgaDriver* self, VgaColor* colors, uint8_t size);