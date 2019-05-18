#pragma once

#include <stdbool.h>
#include <stdint.h>

struct GraphicsManager
{
    bool Debug;
};

typedef struct GraphicsManager GraphicsManager;

bool GraphicsManager_Constructor(GraphicsManager* self);
void GraphicsManager_Destructor(GraphicsManager* self);

// Drawing
void GraphicsManager_Render(GraphicsManager* self);

