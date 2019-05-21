#include "GuiElement.h"

#include <string.h>

bool GuiElement_Constructor(GuiElement* self)
{
    memset(self, 0, sizeof(GuiElement));
}

void GuiElement_Destructor(GuiElement* self)
{

}

bool GuiElement_IsMouseOver(GuiElement* self, uint32_t pointer_x, uint32_t pointer_y)
{
    return Area_IsMouseOver(&self->Area, pointer_x, pointer_y);
}