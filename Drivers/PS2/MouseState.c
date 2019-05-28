#include "MouseState.h"

#include <string.h>
#include <stdio.h>

#include <Objects/Kernel/Kernel.h>

extern Kernel _Kernel;

bool MouseState_Constructor(MouseState* self)
{
    printf("MouseState: Constructor\n");
    memset(self,0,sizeof(MouseState));
    self->Debug        = false;
    self->NumButtons   = 3;
    self->PositionXMax = _Kernel.Vga.FramebufferWidth;
    self->PositionYMax = _Kernel.Vga.FramebufferHeight;
    return true;
}

void MouseState_OnData(MouseState* self, uint8_t data)
{
    if (self->Debug) printf("MouseState: OnData 0x%x -> %d\n",data, self->DataBufferIndex);

    if (self->DataBufferIndex == 0 && (data & 0x8) == 0)
    {
        if (self->Debug) printf("MouseState: Waiting for state byte\n");
        return;
    }

    self->DataBuffer[self->DataBufferIndex++] = data;
    if (self->DataBufferIndex >= MOUSE_STATE_DATA_BUFFER_MAX)
    {
        MouseState_ParseDataBuffer(self);
        self->DataBufferIndex = 0;
    }
}

void MouseState_ParseDataBuffer(MouseState* self)
{
    if(self->Debug) printf("MouseState: ParseDataBuffer\n");

    uint8_t state   = self->DataBuffer[0];
	uint8_t delta_x = self->DataBuffer[1];
	uint8_t delta_y = self->DataBuffer[2];

    // Discard X/Y Overflow packets
    if ((state & 0x80) != 0 || (state & 0x40) != 0) 
    {
        if (self->Debug) printf("MouseState: X or Y Overflow\n");
        return;
    }

	self->PositionX += delta_x - ((state << 4) & 0x100);
	self->PositionY -= delta_y - ((state << 3) & 0x100);

    // X Clamping
    if (self->PositionX < 0) self->PositionX = 0;
    else if (self->PositionX > self->PositionXMax) self->PositionX = self->PositionXMax;

    // Y Clamping
    if (self->PositionY < 0) self->PositionY = 0;
    else if (self->PositionY > self->PositionYMax) self->PositionY = self->PositionYMax;

    // Buttons
    self->Button[0] = (state & 0x01) != 0;
    self->Button[1] = (state & 0x02) != 0;
    self->Button[2] = (state & 0x04) != 0;

    if (self->Debug) MouseState_Debug(self);
    if (self->MouseEventCallback)
    {
        MouseEvent me;
        me.X = self->PositionX;
        me.Y = self->PositionY;
        me.Button[0] = self->Button[0];
        me.Button[1] = self->Button[1];
        me.Button[2] = self->Button[2];
        self->MouseEventCallback(me);
    }
}

void MouseState_Debug(MouseState* self)
{
    printf("MouseState: X:%03d Y:%03d 1:%d 2:%d 3:%d\n", 
        self->PositionX, 
        self->PositionY, 
        self->Button[0], 
        self->Button[1], 
        self->Button[2]
    );
}


void MouseState_SetEventCallback(MouseState* self, void(*callback)(MouseEvent))
{
    self->MouseEventCallback = callback;
}