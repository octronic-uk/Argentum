#include "VgaDriver.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <Objects/Kernel/Kernel.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Interrupt/InterruptDriver.h>
#include "VgaConstants.h"

#include <Boot/multiboot.h>

extern Kernel _Kernel;

bool VgaDriver_Constructor(VgaDriver* self)
{
	printf("VgaDriver: Constructing\n");
	memset(self,0,sizeof(VgaDriver));
	self->Debug = false;
	if (_Kernel.MultibootInfo)
	{
		// VBE Info is present
		if ((_Kernel.MultibootInfo->flags & MULTIBOOT_INFO_VBE_INFO) != 0)
		{
            self->VbeControlInfo      = _Kernel.MultibootInfo->vbe_control_info;
            self->VbeModeInfo         = _Kernel.MultibootInfo->vbe_mode_info;
            self->VbeMode             = _Kernel.MultibootInfo->vbe_mode;
            self->VbeInterfaceSegment = _Kernel.MultibootInfo->vbe_interface_seg;
            self->VbeInterfaceOffset  = _Kernel.MultibootInfo->vbe_interface_off;
            self->VbeInterfaceLength  = _Kernel.MultibootInfo->vbe_interface_len;
			self->FramebufferAddress  = _Kernel.MultibootInfo->framebuffer_addr;
			self->FramebufferPitch    = _Kernel.MultibootInfo->framebuffer_pitch;
			self->FramebufferWidth    = _Kernel.MultibootInfo->framebuffer_width;
			self->FramebufferHeight   = _Kernel.MultibootInfo->framebuffer_height;
			self->FramebufferBpp      = _Kernel.MultibootInfo->framebuffer_bpp;

			if (self->Debug)
			{
				VgaDriver_DebugVbeInfo(self);
			}
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

void VgaDriver_Destructor(VgaDriver* self)
{

}

void VgaDriver_DebugVbeInfo(VgaDriver* self)
{
	printf("VgaDriver: Passed VBE Info from Multiboot\n");
	printf("\tVBE Control Info  0x%x\n", self->VbeControlInfo);
	printf("\tVBE Mode Info     0x%x\n", self->VbeModeInfo);
	printf("\tVBE Mode          0x%x\n", self->VbeMode);
	printf("\tVBE Interface Seg 0x%x\n", self->VbeInterfaceSegment);
	printf("\tVBE Interface Off 0x%x\n", self->VbeInterfaceOffset);
	printf("\tVBE Interface Len 0x%x\n", self->VbeInterfaceLength); 
	printf("\tFB Address        0x%x\n", self->FramebufferAddress);
	printf("\tFB Pitch          %d\n",   self->FramebufferPitch);
	printf("\tFB Width          %d\n",   self->FramebufferWidth);
	printf("\tFB Height         %d\n",   self->FramebufferHeight);
	printf("\tFB Bpp            %d\n",   self->FramebufferBpp);
}

void VgaDriver_WritePixel(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c)
{
	if(self->Debug) printf("VgaDriver: Writing x:%d y:%d c:0x%x\n",x,y,c);
	uint32_t addr = self->FramebufferAddress;
	uint32_t px = self->FramebufferBpp / 8;
	addr += ((y*self->FramebufferWidth)+x)*px;
	*((uint32_t*)addr) = c;
}

uint32_t VgaDriver_ReadPixel(VgaDriver* self, uint32_t x, uint32_t y)
{
	if(self->Debug) printf("VgaDriver: Reading x:%d y:%d\n",x,y);
	uint32_t addr = self->FramebufferAddress;
	uint32_t px = self->FramebufferBpp / 8;
	addr += ((y*self->FramebufferWidth)+x)*px;
	return *((uint32_t*)addr);
}

void VgaDriver_ClearScreen(VgaDriver* self)
{
	uint32_t x, y;
	/* clear screen */
	for(y = 0; y < self->FramebufferHeight; y++)
	{
		for(x = 0; x < self->FramebufferWidth; x++)
		{
			VgaDriver_WritePixel(self, x, y, 0);
		}
	}
}

void VgaDriver_WaitForVBlankEnd(VgaDriver* self)
{
	uint8_t result = 0;
	do
	{
		result = IO_ReadPort8b(0x03DA);
	}
	while((result & 0x08) == 0);
}

void VgaDriver_WaitForVBlankStart(VgaDriver* self)
{
	uint8_t result = 0;
	do
	{
		result = IO_ReadPort8b(0x03DA);
	}
	while((result & 0x08) != 0);
}