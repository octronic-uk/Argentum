#include "Text.h"

#include <stdio.h>
#include <string.h>
#include <Objects/GraphicsManager/GraphicsManager.h>
#include <Objects/GraphicsManager/GraphicsManagerConstants.h>
#include <Drivers/Screen/Vga/VgaFonts.h>

bool Text_Constructor(Text* self)
{
    printf("Text: Constructor\n");
    memset(self,0,sizeof(Text));
    GuiElement_Constructor(&self->GuiElement);
    self->Debug = false;
    // Default Dimensions
    self->GuiElement.Area.W = TEXT_DEFAULT_WIDTH;
    self->GuiElement.Area.H = TEXT_DEFAULT_HEIGHT;
    // Set callback functions
    self->GuiElement.DrawFunction = Text_OnDraw;
    self->GuiElement.UpdateFunction = Text_OnUpdate;
    self->GuiElement.OnMouseClickFunction = Text_OnMouseClick;
    self->GuiElement.OnMouseReleaseFunction = Text_OnMouseRelease;
    self->GuiElement.OnMouseEnterFunction = Text_OnMouseEnter;
    self->GuiElement.OnMouseExitFunction = Text_OnMouseExit;

    self->Color = DEFAULT_PALETTE_LIGHT_GREY;
    self->GuiElement.NeedsRedraw = true;
    return true;
}

void Text_Destructor(Text* self)
{
    printf("Text: Destructor\n");
}

void Text_SetText(Text* self, char* text)
{
    strncpy(&self->Text[0],text,TEXT_BUFFER_SIZE);
}

void Text_OnDraw(GuiElement* self, GraphicsManager* gm)
{
    Text* self_text = (Text*)self;
    //if (self_text->Debug) printf("Text: Drawing text %s\n",self_text->Text);
    uint32_t text_length = strlen(self_text->Text) * (VGA_FONT_8x8_W+1);
    GraphicsManager_DrawText(gm, self->Area.X, self->Area.Y, self_text->Color, self_text->Text);
    self->NeedsRedraw = false;
}

void Text_OnUpdate(struct GuiElement* self, GraphicsManager* gm)
{
    Text* self_text = (Text*)self;
    if (self->MouseInsideLast != self->MouseInside)
    {
        self->NeedsRedraw = true;
    }
    self->MouseInsideLast = self->MouseInside;
}

void Text_OnMouseEnter(GuiElement* self, GraphicsManager* gm) 
{
    Text* self_text = (Text*)self;
    if (self_text->Debug) printf("Text: %s mouse entered\n",self_text->Text);
    self->MouseInsideLast = self->MouseInside;
    self->MouseInside = true;
}

void Text_OnMouseExit(GuiElement* self, GraphicsManager* gm) 
{
    Text* self_text = (Text*)self;
    if (self_text->Debug) printf("Text: %s mouse exited\n",self_text->Text);
    self->MouseInsideLast = self->MouseInside;
    self->MouseInside = false;
}

void Text_OnMouseClick(GuiElement* self, GraphicsManager* gm)
{
    Text* self_text = (Text*)self;
    if (self_text->Debug) printf("Text: %s clicked\n",self_text->Text);
}

void Text_OnMouseRelease(GuiElement* self, GraphicsManager* gm)
{
    Text* self_text = (Text*)self;
    if (self_text->Debug) printf("Text: %s released\n",self_text->Text);
}
