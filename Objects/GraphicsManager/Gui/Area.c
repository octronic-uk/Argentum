#include "Area.h"

bool Area_IsMouseOver(Area* self, uint32_t pointer_x, uint32_t pointer_y)
{
    return pointer_x > self->X && // past left edge |<- 
           pointer_x < self->X + self->W && // past before right edge  ->|
           pointer_y > self->Y && // below top edge 
           pointer_y < self->Y + self->H;   // Above bottom edge
}