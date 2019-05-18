#include "Font.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool Font_Constructor(Font* self)
{
    printf("Font: Constructing\n");
    memset(self, 0, sizeof(Font));
    return true;
}

void Font_Destructor(Font* self)
{
    printf("Font: Destructing\n");
}