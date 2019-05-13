#include "VgaDriver.h"

#include <stdio.h>
#include <string.h>
#include <Objects/Kernel/Kernel.h>
#include <Drivers/IO/IODriver.h>
#include <Drivers/Interrupt/InterruptDriver.h>
#include "VgaConstants.h"

#define	peekb(S,O)		*(uint8_t *)(16uL * (S) + (O))
#define	pokeb(S,O,V)		*(uint8_t *)(16uL * (S) + (O)) = (V)
#define	pokew(S,O,V)		*(uint16_t *)(16uL * (S) + (O)) = (V)
#define	_vmemwr(DS,DO,S,N)	memcpy((char *)((DS) * 16 + (DO)), S, N)

extern Kernel _Kernel;

bool VgaDriver_Constructor(VgaDriver* self)
{
	printf("VgaDriver: Constructing\n");
	memset(self,0,sizeof(VgaDriver));
	return true;
}

void VgaDriver_Dump(VgaDriver* self, uint8_t *regs, uint32_t count)
{
	uint32_t i;

	i = 0;
	printf("\t");
	for(; count != 0; count--)
	{
		printf("0x%02X,", *regs);
		i++;
		if(i >= 8)
		{
			i = 0;
			printf("\n\t");
		}
		else
			printf(" ");
		regs++;
	}
	printf("\n");
}

void VgaDriver_DumpRegs(VgaDriver* self, uint8_t *regs)
{
	printf("uint8_t g_mode[] =\n");
	printf("{\n");
    /* dump MISCELLANEOUS reg */
	printf("/* MISC */\n");
	printf("\t0x%02X,\n", *regs);
	regs++;
    /* dump SEQUENCER regs */
	printf("/* SEQ */\n");
	VgaDriver_Dump(self, regs, VGA_NUM_SEQ_REGS);
	regs += VGA_NUM_SEQ_REGS;
    /* dump CRTC regs */
	printf("/* CRTC */\n");
	VgaDriver_Dump(self, regs, VGA_NUM_CRTC_REGS);
	regs += VGA_NUM_CRTC_REGS;
    /* dump GRAPHICS CONTROLLER regs */
	printf("/* GC */\n");
	VgaDriver_Dump(self, regs, VGA_NUM_GFX_CTRLR_REGS);
	regs += VGA_NUM_GFX_CTRLR_REGS;
    /* dump ATTRIBUTE CONTROLLER regs */
	printf("/* AC */\n");
	VgaDriver_Dump(self, regs, VGA_NUM_ATTRIB_CTRL_REGS);
	regs += VGA_NUM_ATTRIB_CTRL_REGS;
	printf("};\n");
}

void VgaDriver_ReadRegs(VgaDriver* self, uint8_t *regs)
{
	uint32_t i;

    /* read MISCELLANEOUS reg */
	*regs = IO_ReadPort8b(VGA_MISC_READ);
	regs++;
    /* read SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		IO_WritePort8b(VGA_SEQ_INDEX, i);
		*regs = IO_ReadPort8b(VGA_SEQ_DATA);
		regs++;
	}
    /* read CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		IO_WritePort8b(VGA_CRTC_INDEX, i);
		*regs = IO_ReadPort8b(VGA_CRTC_DATA);
		regs++;
	}
    /* read GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GFX_CTRLR_REGS; i++)
	{
		IO_WritePort8b(VGA_GFX_CTRLR_INDEX, i);
		*regs = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);
		regs++;
	}
    /* read ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_ATTRIB_CTRL_REGS; i++)
	{
		IO_ReadPort8b(VGA_INSTAT_READ);
		IO_WritePort8b(VGA_ATTRIB_CTRL_INDEX, i);
		*regs = IO_ReadPort8b(VGA_ATTRIB_CTRL_READ);
		regs++;
	}
    /* lock 16-color palette and unblank display */
	IO_ReadPort8b(VGA_INSTAT_READ);
	IO_WritePort8b(VGA_ATTRIB_CTRL_INDEX, 0x20);
}

void VgaDriver_WriteDacColor(VgaDriver* self, uint8_t address, VgaColor color)
{
	// Disable Interrupts
	InterruptDriver_Disable_CLI(&_Kernel.Interrupt);
	// Write Address
	IO_WritePort8b(VGA_DAC_WRITE_INDEX, address);
	// Write Color
	IO_WritePort8b(VGA_DAC_DATA, color.Red);
	IO_WritePort8b(VGA_DAC_DATA, color.Green);
	IO_WritePort8b(VGA_DAC_DATA, color.Blue);
	// Enable Interrupts
	InterruptDriver_Enable_STI(&_Kernel.Interrupt);
}

VgaColor VgaDriver_ReadDacColor(VgaDriver* self, uint8_t address)
{
	VgaColor color;
	// Disable Interrupts
	InterruptDriver_Disable_CLI(&_Kernel.Interrupt);
	// Write Address
	IO_WritePort8b(VGA_DAC_READ_INDEX, address);
	// Write Color
	color.Red = IO_ReadPort8b(VGA_DAC_DATA);
	color.Green = IO_ReadPort8b(VGA_DAC_DATA);
	color.Blue = IO_ReadPort8b(VGA_DAC_DATA);
	// Enable Interrupts
	InterruptDriver_Enable_STI(&_Kernel.Interrupt);
	return color;
}

void VgaDriver_WriteColorPalette(VgaDriver* self, VgaColor* colors, uint8_t size)
{
	uint8_t base_address = 0;
	uint8_t i;
	for (i=0;i<size;i++)
	{
		VgaDriver_WriteDacColor(self,base_address+i,colors[i]);
	}
}

void VgaDriver_WriteRegs(VgaDriver* self, uint8_t *regs)
{
	uint32_t i;

    /* write MISCELLANEOUS reg */
	IO_WritePort8b(VGA_MISC_WRITE, *regs);
	regs++;

    /* write SEQUENCER regs */
	for(i = 0; i < VGA_NUM_SEQ_REGS; i++)
	{
		IO_WritePort8b(VGA_SEQ_INDEX, i);
		IO_WritePort8b(VGA_SEQ_DATA, *regs);
		regs++;
	}

    /* unlock CRTC registers */
	IO_WritePort8b(VGA_CRTC_INDEX, 0x03);
	IO_WritePort8b(VGA_CRTC_DATA, IO_ReadPort8b(VGA_CRTC_DATA) | 0x80);
	IO_WritePort8b(VGA_CRTC_INDEX, 0x11);
	IO_WritePort8b(VGA_CRTC_DATA, IO_ReadPort8b(VGA_CRTC_DATA) & ~0x80);

    /* make sure they remain unlocked */
	regs[0x03] |= 0x80;
	regs[0x11] &= ~0x80;

    /* write CRTC regs */
	for(i = 0; i < VGA_NUM_CRTC_REGS; i++)
	{
		IO_WritePort8b(VGA_CRTC_INDEX, i);
		IO_WritePort8b(VGA_CRTC_DATA, *regs);
		regs++;
	}

    /* write GRAPHICS CONTROLLER regs */
	for(i = 0; i < VGA_NUM_GFX_CTRLR_REGS; i++)
	{
		IO_WritePort8b(VGA_GFX_CTRLR_INDEX, i);
		IO_WritePort8b(VGA_GFX_CTRLR_DATA, *regs);
		regs++;
	}

    /* write ATTRIBUTE CONTROLLER regs */
	for(i = 0; i < VGA_NUM_ATTRIB_CTRL_REGS; i++)
	{
		IO_ReadPort8b(VGA_INSTAT_READ);
		IO_WritePort8b(VGA_ATTRIB_CTRL_INDEX, i);
		IO_WritePort8b(VGA_ATTRIB_CTRL_WRITE, *regs);
		regs++;
	}

    /* lock 16-color palette and unblank display */
	IO_ReadPort8b(VGA_INSTAT_READ);
	IO_WritePort8b(VGA_ATTRIB_CTRL_INDEX, 0x20);
}

void VgaDriver_SetPlane(VgaDriver* self, uint32_t p)
{
	uint8_t pmask;

	p &= 3;
	pmask = 1 << p;
    /* set read plane */
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 4);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, p);
    /* set write plane */
	IO_WritePort8b(VGA_SEQ_INDEX, 2);
	IO_WritePort8b(VGA_SEQ_DATA, pmask);
}

/*
    VGA framebuffer is at A000:0000, B000:0000, or B800:0000
    depending on bits in GC 6
*/
uint32_t VgaDriver_GetFramebufferSegment(VgaDriver* self)
{
	uint8_t seg;

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 0x06);
	seg = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);
	seg >>= 2;
	seg &= 0x03;
	switch(seg)
	{
		case 0x00:
		case 0x01: return 0xA000;
		case 0x02: return 0xB000;
		case 0x03: return 0xB800;
	}
	return seg;
}

void VgaDriver_vmemwr(VgaDriver* self, uint32_t dst_off, uint8_t *src, uint32_t count)
{
	_vmemwr(VgaDriver_GetFramebufferSegment(self), dst_off, src, count);
}

void VgaDriver_vpokeb(VgaDriver* self, uint32_t off, uint32_t val)
{
	pokeb(VgaDriver_GetFramebufferSegment(self), off, val);
}

uint32_t VgaDriver_vpeekb(VgaDriver* self, uint32_t off)
{
	return peekb(VgaDriver_GetFramebufferSegment(self), off);
}

/*
    write font to plane P4 (assuming planes are named P1, P2, P4, P8)
*/
void VgaDriver_WriteFont(VgaDriver* self, uint8_t *buf, uint32_t font_height)
{
	uint8_t seq2, seq4, gc4, gc5, gc6;
	uint32_t i;

    /* save registers
        VgaDriver_SetPlane() modifies GC 4 and SEQ 2, so save them as well */
	IO_WritePort8b(VGA_SEQ_INDEX, 2);
	seq2 = IO_ReadPort8b(VGA_SEQ_DATA);

	IO_WritePort8b(VGA_SEQ_INDEX, 4);
	seq4 = IO_ReadPort8b(VGA_SEQ_DATA);
    /* turn off even-odd addressing (set flat addressing)
        assume: chain-4 addressing already off */
	IO_WritePort8b(VGA_SEQ_DATA, seq4 | 0x04);

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 4);
	gc4 = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 5);
	gc5 = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);
    /* turn off even-odd addressing */
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc5 & ~0x10);

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 6);
	gc6 = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);
    /* turn off even-odd addressing */
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc6 & ~0x02);
    /* write font to plane P4 */
	VgaDriver_SetPlane(self,2);
    /* write font 0 */
	for(i = 0; i < 256; i++)
	{
		VgaDriver_vmemwr(self, 16384u * 0 + i * 32, buf, font_height);
		buf += font_height;
	}
	#if 0
    /* write font 1 */
	for(i = 0; i < 256; i++)
	{
		vmemwr(16384u * 1 + i * 32, buf, font_height);
		buf += font_height;
	}
	#endif

    /* restore registers */
	IO_WritePort8b(VGA_SEQ_INDEX, 2);
	IO_WritePort8b(VGA_SEQ_DATA, seq2);
	IO_WritePort8b(VGA_SEQ_INDEX, 4);
	IO_WritePort8b(VGA_SEQ_DATA, seq4);
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 4);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc4);
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 5);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc5);
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 6);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc6);
}

void VgaDriver_WritePixel1(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c)
{
	uint32_t wd_in_bytes;
	uint32_t off, mask;

	c = (c & 1) * 0xFF;
	wd_in_bytes = self->Width / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	VgaDriver_vpokeb(self,off, (VgaDriver_vpeekb(self, off) & ~mask) | (c & mask));
}

void VgaDriver_WritePixel2(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c)
{
	uint32_t wd_in_bytes, off, mask;

	c = (c & 3) * 0x55;
	wd_in_bytes = self->Width / 4;
	off = wd_in_bytes * y + x / 4;
	x = (x & 3) * 2;
	mask = 0xC0 >> x;
	VgaDriver_vpokeb(self,off, (VgaDriver_vpeekb(self,off) & ~mask) | (c & mask));
}

void VgaDriver_WritePixel4p(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c)
{
	uint32_t wd_in_bytes, off, mask, p, pmask;

	wd_in_bytes = self->Width / 8;
	off = wd_in_bytes * y + x / 8;
	x = (x & 7) * 1;
	mask = 0x80 >> x;
	pmask = 1;
	for(p = 0; p < 4; p++)
	{
		VgaDriver_SetPlane(self, p);
		if(pmask & c)
			VgaDriver_vpokeb(self,off, VgaDriver_vpeekb(self, off) | mask);
		else
			VgaDriver_vpokeb(self,off, VgaDriver_vpeekb(self, off) & ~mask);
		pmask <<= 1;
	}
}

void VgaDriver_WritePixel8(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c)
{
	uint32_t wd_in_bytes;
	uint32_t off;

	wd_in_bytes = self->Width;
	off = wd_in_bytes * y + x;
	VgaDriver_vpokeb(self,off, c);
}

void VgaDriver_WritePixel8x(VgaDriver* self, uint32_t x, uint32_t y, uint32_t c)
{
	uint32_t wd_in_bytes;
	uint32_t off;

	wd_in_bytes = self->Width / 4;
	off = wd_in_bytes * y + x / 4;
	VgaDriver_SetPlane(self, x & 3);
	VgaDriver_vpokeb(self, off, c);
}

void VgaDriver_ClearScreen(VgaDriver* self)
{
	uint32_t x, y;
	/* clear screen */
	for(y = 0; y < self->Height; y++)
	{
		for(x = 0; x < self->Width; x++)
		{
			self->WritePixelFunction(self, x, y, 0);
		}
	}
}

void VgaDriver_DrawX(VgaDriver* self)
{

	uint32_t x, y;
	VgaDriver_ClearScreen(self);

    /* draw 2-color X */
	for(y = 0; y < self->Height; y++)
	{
		self->WritePixelFunction(self, (self->Width - self->Height) / 2 + y, y, 1);
		self->WritePixelFunction(self, (self->Height + self->Width) / 2 - y, y, 2);
	}
	getchar();
}

/*
    READ AND DUMP VGA REGISTER VALUES FOR CURRENT VIDEO MODE
    This is where g_40x25_text[], g_80x50_text[], etc. came from :)
*/
void VgaDriver_DumpState(VgaDriver* self)
{
	uint8_t state[VGA_NUM_REGS];

	VgaDriver_ReadRegs(self,state);
	VgaDriver_DumpRegs(self,state);
}

/*
    SET TEXT MODES
*/
void VgaDriver_SetTextMode(VgaDriver* self, bool hi_res)
{
	uint32_t rows, cols, char_height, i;

	if(hi_res)
	{
		VgaDriver_WriteRegs(self, g_90x60_text);
		cols = 90;
		rows = 60;
		char_height = 8;
	}
	else
	{
		VgaDriver_WriteRegs(self, g_80x25_text);
		cols = 80;
		rows = 25;
		char_height = 16;
	}

    /* set font */
	if(char_height >= 16)
	{
		VgaDriver_WriteFont(self,g_8x16_font, 16);
	}
	else
	{
		VgaDriver_WriteFont(self, g_8x8_font, 8);
	}

    /* tell the BIOS what we've done, so BIOS text output works OK */
	pokew(0x40, 0x4A, cols);            /* columns on screen */
	pokew(0x40, 0x4C, cols * rows * 2); /* framebuffer size */
	pokew(0x40, 0x50, 0);               /* cursor pos'n */
	pokeb(0x40, 0x60, char_height - 1);	/* cursor shape */
	pokeb(0x40, 0x61, char_height - 2);
	pokeb(0x40, 0x84, rows - 1);        /* rows on screen - 1 */
	pokeb(0x40, 0x85, char_height);     /* char height */

    /* set white-on-black attributes for all text */
	for(i = 0; i < cols * rows; i++)
	{
		pokeb(0xB800, i * 2 + 1, 7);
	}
}

void VgaDriver_SetScreenMode(VgaDriver* self, VgaScreenMode mode)
{
	self->ScreenMode = mode;
	switch(self->ScreenMode)
	{
		case VGA_MODE_TEXT_40_25:
			VgaDriver_SetTextMode(self,0);
			break;
		case VGA_MODE_TEXT_90_60:
			VgaDriver_SetTextMode(self,1);
			break;
		case VGA_MODE_PX_320_200_4:
			VgaDriver_WriteRegs(self, g_320x200x4);
			self->Width  = 320;
			self->Height = 200;
			self->WritePixelFunction = VgaDriver_WritePixel2;
			break;
		case VGA_MODE_PX_320_200_256:
			VgaDriver_WriteRegs(self, g_320x200x256);
			self->Width  = 320;
			self->Height = 200;
			self->WritePixelFunction = VgaDriver_WritePixel8;
			break;
		case VGA_MODE_PX_320_200_256_X:
			VgaDriver_WriteRegs(self, g_320x200x256_modex);
			self->Width  = 320;
			self->Height = 200;
			self->WritePixelFunction = VgaDriver_WritePixel8x;
			break;
		case VGA_MODE_PX_640_480_16:
			VgaDriver_WriteRegs(self, g_640x480x16);
			self->Width  = 640;
			self->Height = 480;
			self->WritePixelFunction = VgaDriver_WritePixel4p;
			break;
	}
}

/*
    DEMO GRAPHICS MODES
*/
void VgaDriver_DemoGraphics(VgaDriver* self)
{
	printf("Screen-clear in 16-color mode will be VERY SLOW\nPress a key to continue\n");
	getchar();

    /* 4-color * /
	VgaDriver_SetScreenMode(self, VGA_MODE_PX_320_200_4);
	VgaDriver_DrawX(self);
	*/

    /* 16-color */
	VgaDriver_SetScreenMode(self,VGA_MODE_PX_640_480_16);
	VgaDriver_DrawX(self);

    /* 256-color */
	VgaDriver_SetScreenMode(self,VGA_MODE_PX_320_200_256);
	VgaDriver_DrawX(self);

    /* 256-color Mode-X */
	VgaDriver_SetScreenMode(self,VGA_MODE_PX_320_200_256_X);
	VgaDriver_DrawX(self);

    /* go back to 80x25 text mode */
	VgaDriver_SetScreenMode(self,VGA_MODE_TEXT_40_25);
}

uint8_t VgaDriver_ReverseBits(VgaDriver* self, uint8_t arg)
{
	uint8_t ret_val = 0;
	if(arg & 0x01) ret_val |= 0x80;
	if(arg & 0x02) ret_val |= 0x40;
	if(arg & 0x04) ret_val |= 0x20;
	if(arg & 0x08) ret_val |= 0x10;
	if(arg & 0x10) ret_val |= 0x08;
	if(arg & 0x20) ret_val |= 0x04;
	if(arg & 0x40) ret_val |= 0x02;
	if(arg & 0x80) ret_val |= 0x01;
	return ret_val;
}

/*
    512-CHARACTER FONT
*/
void VgaDriver_Font512(VgaDriver* self)
{
	const char msg1[] = "!txet sdrawkcaB";
	const char msg2[] = "?rorrim a toG";

	uint8_t seq2, seq4, gc4, gc5, gc6;
	uint32_t font_height, i, j;

    /* start in 80x25 text mode */
	VgaDriver_SetTextMode(self, 1);
    /* 
		code pasted in from WriteFont():
			save registers
			SetPlane() modifies GC 4 and SEQ 2, so save them as well 
	*/
	IO_WritePort8b(VGA_SEQ_INDEX, 2);
	seq2 = IO_ReadPort8b(VGA_SEQ_DATA);

	IO_WritePort8b(VGA_SEQ_INDEX, 4);
	seq4 = IO_ReadPort8b(VGA_SEQ_DATA);

    /* turn off even-odd addressing (set flat addressing) assume: chain-4 addressing already off */
	IO_WritePort8b(VGA_SEQ_DATA, seq4 | 0x04);

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 4);
	gc4 = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 5);
	gc5 = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);

    /* turn off even-odd addressing */
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc5 & ~0x10);

	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 6);
	gc6 = IO_ReadPort8b(VGA_GFX_CTRLR_DATA);

    /* turn off even-odd addressing */
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc6 & ~0x02);

    /* write font to plane P4 */
	VgaDriver_SetPlane(self, 2);

    /* 
		this is different from WriteFont():
        	use font 1 instead of font 0, and use it for BACKWARD text 
	*/
	font_height = 16;
	for(i = 0; i < 256; i++)
	{
		for(j = 0; j < font_height; j++)
		{
			VgaDriver_vpokeb(self,16384u * 1 + 32 * i + j,VgaDriver_ReverseBits(self,g_8x16_font[font_height * i + j]));
		}
	}

    /* restore registers */
	IO_WritePort8b(VGA_SEQ_INDEX, 2);
	IO_WritePort8b(VGA_SEQ_DATA, seq2);
	IO_WritePort8b(VGA_SEQ_INDEX, 4);
	IO_WritePort8b(VGA_SEQ_DATA, seq4);
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 4);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc4);
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 5);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc5);
	IO_WritePort8b(VGA_GFX_CTRLR_INDEX, 6);
	IO_WritePort8b(VGA_GFX_CTRLR_DATA, gc6);

    /* 
		now: sacrifice attribute bit b3 (foreground intense color)
        use it to select characters 256-511 in the second font 
	*/
	IO_WritePort8b(VGA_SEQ_INDEX, 3);
	IO_WritePort8b(VGA_SEQ_DATA, 4);

    /* 
		xxx - maybe re-program 16-color palette here
        so attribute bit b3 is no longer used for 'intense' 
	*/
	for(i = 0; i < sizeof(msg1); i++)
	{
		VgaDriver_vpokeb(self,(80 * 8  + 40 + i) * 2 + 0, msg1[i]);
    	/* set attribute bit b3 for backward font */
		VgaDriver_vpokeb(self,(80 * 8  + 40 + i) * 2 + 1, 0x0F);
	}

	for(i = 0; i < sizeof(msg2); i++)
	{
		VgaDriver_vpokeb(self,(80 * 16 + 40 + i) * 2 + 0, msg2[i]);
		VgaDriver_vpokeb(self,(80 * 16 + 40 + i) * 2 + 1, 0x0F);
	}
}

void VgaDriver_Main(VgaDriver* self)
{
	VgaDriver_DumpState(self);
	//VgaDriver_SetTextMode(self, arg_c > 1);
	VgaDriver_DemoGraphics(self);
	//VgaDriver_Font512(self);
}