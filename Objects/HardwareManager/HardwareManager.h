#pragma once

#include <stdbool.h>
#include "StorageManager/StorageManager.h"
#include "GraphicsManager/GraphicsManager.h"
#include "InputManager/InputManager.h"

struct HardwareManager
{
    StorageManager  StorageManager;
    GraphicsManager GraphicsManager;
    InputManager    InputManager;
};

typedef struct HardwareManager HardwareManager;

bool HardwareManager_Constructor(HardwareManager* self);
void HardwareManager_Destructor(HardwareManager* self);