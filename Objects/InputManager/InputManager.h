#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <Drivers/PS2/Event.h>
#include <Objects/Common/LinkedList/LinkedList.h>

struct InputManager
{
    bool Debug;
    LinkedList KeyboardEvents;
    LinkedList MouseEvents;
};

typedef struct InputManager InputManager;

bool InputManager_Constructor(InputManager* self);
void InputManager_Destructor(InputManager* self);
void InputManager_ProcessEvents(InputManager* self); 

// Mouse
uint32_t InputManager_GetMouseX(InputManager* self);
uint32_t InputManager_GetMouseY(InputManager* self);
bool     InputManager_GetMouseButton(InputManager* self, uint8_t button);
void     InputManager_OnMouseEvent(MouseEvent event);
void     InputManager_ProcessMouseEvents(InputManager* self);

// Keyboard
void InputManager_OnKeyboardEvent(KeyboardEvent event);
void InputManager_ProcessKeyboardEvents(InputManager* self);

