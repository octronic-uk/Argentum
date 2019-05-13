#pragma once

#include <stdbool.h>

struct GraphicsManager
{
    bool Debug;
};

typedef struct GraphicsManager GraphicsManager;

bool GraphicsManager_Constructor(GraphicsManager* self);
void GraphicsManager_Destructor(GraphicsManager* self);