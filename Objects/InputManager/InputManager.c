#include "InputManager.h"

#include <Objects/Kernel/Kernel.h>
#include <Drivers/PS2/PS2Driver.h>

#include <stdio.h>
#include <string.h>

extern Kernel _Kernel;

bool InputManager_Constructor(InputManager* self)
{
    printf("InputManager: Constructing\n");
    memset(self,0,sizeof(InputManager));
    self->Debug = false;

    LinkedList_Constructor(&self->KeyboardEvents);
    LinkedList_Constructor(&self->MouseEvents);

    PS2Driver_SetKeyboardEventCallback(&_Kernel.PS2, InputManager_OnKeyboardEvent);
    PS2Driver_SetMouseEventCallback(&_Kernel.PS2, InputManager_OnMouseEvent);
    return true;
}

void InputManager_Destructor(InputManager* self)
{
    LinkedList_Destructor(&self->KeyboardEvents);
    LinkedList_Destructor(&self->MouseEvents);
    printf("InputManager: Destructing\n");
}

uint32_t InputManager_GetMouseX(InputManager* self)
{
    return _Kernel.PS2.MouseState.PositionX;
}

uint32_t InputManager_GetMouseY(InputManager* self)
{
    return _Kernel.PS2.MouseState.PositionY;
}

bool InputManager_GetMouseButton(InputManager* self, uint8_t button)
{
    if (button > _Kernel.PS2.MouseState.NumButtons) return false;
    return _Kernel.PS2.MouseState.Button[button];
}

void InputManager_OnMouseEvent(MouseEvent event)
{
    InputManager* self = &_Kernel.InputManager;

    if(self->Debug) printf("InputManager: Got Mouse Event\n");

    MouseEvent *e = (MouseEvent*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(MouseEvent));
    memcpy(e,&event,sizeof(MouseEvent));
    LinkedList_PushBack(&self->MouseEvents, e);
}

void InputManager_OnKeyboardEvent(KeyboardEvent event)
{
    InputManager* self = &_Kernel.InputManager;

    if(self->Debug) printf("InputManager: Got Keyboard Event\n");

    KeyboardEvent *e = (KeyboardEvent*)MemoryDriver_Allocate(&_Kernel.Memory, sizeof(KeyboardEvent));
    memcpy(e,&event,sizeof(KeyboardEvent));
    LinkedList_PushBack(&self->KeyboardEvents, e);
}

void InputManager_ProcessEvents(InputManager* self)
{
    InputManager_ProcessKeyboardEvents(self);
    InputManager_ProcessMouseEvents(self);
}

void InputManager_ProcessMouseEvents(InputManager* self)
{
    if (self->Debug) printf("InputManager: Processing Mouse Events");
    LinkedList* mouse_event_list = &self->MouseEvents;
    while (LinkedList_Size(mouse_event_list) > 0)
    {
        InterruptDriver_Disable_CLI(&_Kernel.Interrupt);
        MouseEvent* e = LinkedList_PopFront(mouse_event_list);
        _Kernel.GraphicsManager.CurrentMouseEvent = *e;
        GraphicsManager_MouseEventToActiveGuiElement(&_Kernel.GraphicsManager, *e);
        MemoryDriver_Free(&_Kernel.Memory,e);
        InterruptDriver_Enable_STI(&_Kernel.Interrupt);
    }
}

void InputManager_ProcessKeyboardEvents(InputManager* self)
{
    if (self->Debug) printf("InputManager: Processing Keyboard Events");
    LinkedList* event_list = &self->KeyboardEvents;
    while (LinkedList_Size(event_list) > 0)
    {
        InterruptDriver_Disable_CLI(&_Kernel.Interrupt);
        KeyboardEvent* e = LinkedList_PopFront(event_list);
        _Kernel.GraphicsManager.CurrentKeyboardEvent = *e;
        GraphicsManager_KeyboardEventToActiveGuiElement(&_Kernel.GraphicsManager, *e);
        MemoryDriver_Free(&_Kernel.Memory,e);
        InterruptDriver_Enable_STI(&_Kernel.Interrupt);
    }
}