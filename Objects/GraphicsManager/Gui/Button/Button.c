#include "Button.h"

#include <stdio.h>
#include <string.h>
#include <Objects/GraphicsManager/GraphicsManager.h>
#include <Objects/GraphicsManager/GraphicsManagerConstants.h>

bool Button_Constructor(Button* self)
{
    printf("Button: Constructor\n");
    memset(self,0,sizeof(Button));
    GuiElement_Constructor(&self->GuiElement);
    self->Debug = false;

    // Default Dimensions
    self->GuiElement.Area.W = BUTTON_DEFAULT_WIDTH;
    self->GuiElement.Area.H = BUTTON_DEFAULT_HEIGHT;

    // Set callback functions
    self->GuiElement.DrawFunction = Button_OnDraw;
    self->GuiElement.UpdateFunction = Button_OnUpdate;
    self->GuiElement.OnMouseClickFunction = Button_OnMouseClick;
    self->GuiElement.OnMouseReleaseFunction = Button_OnMouseRelease;
    self->GuiElement.OnMouseEnterFunction = Button_OnMouseEnter;
    self->GuiElement.OnMouseExitFunction = Button_OnMouseExit;

    self->Color = DEFAULT_PALETTE_LIGHT_GREY;
    self->HoverColor = DEFAULT_PALETTE_WHITE;
    self->GuiElement.NeedsRedraw = true;
    return true;
}

void Button_Destructor(Button* self)
{
    printf("Button: Destructor\n");
}

void Button_SetText(Button* self, char* text)
{
    strncpy(&self->Text[0],text,BUTTON_TEXT_BUFFER_SIZE);
}

void Button_OnDraw(GuiElement* self, GraphicsManager* gm)
{
    Button* self_button = (Button*)self;
    if(self_button->Debug) printf("Button: Drawing button with text %s\n",self_button->Text);

    // Borders
    int x, y, border_max_x, border_max_y;

    border_max_x = self->Area.X + self->Area.W;
    border_max_y = self->Area.Y + self->Area.H;

    uint32_t color = self->MouseInside ? self_button->HoverColor : self_button->Color;

    // Horizontal Border
    for (x=self->Area.X; x<=border_max_x; x++)
    {
        GraphicsManager_WritePixelToFramebuffer(gm, x, self->Area.Y, color);
        GraphicsManager_WritePixelToFramebuffer(gm, x, border_max_y, color);
    }
    // Vertical Border
    for (y=self->Area.Y; y<=border_max_y; y++)
    {
        GraphicsManager_WritePixelToFramebuffer(gm, self->Area.X, y, color);
        GraphicsManager_WritePixelToFramebuffer(gm, border_max_x, y, color);
    }
    
    uint32_t text_length = strlen(self_button->Text) * 9;
    uint32_t x_pad = (self->Area.W - text_length) / 2;
    GraphicsManager_DrawText(gm, self->Area.X+x_pad, self->Area.Y+4, color, self_button->Text);
    self->NeedsRedraw = false;
}

void Button_OnUpdate(struct GuiElement* self, GraphicsManager* gm)
{
    Button* self_button = (Button*)self;
    if (self->MouseInsideLast != self->MouseInside)
    {
        self->NeedsRedraw = true;
    }
    self->MouseInsideLast = self->MouseInside;
}

void Button_OnMouseEnter(GuiElement* self, GraphicsManager* gm) 
{
    Button* self_button = (Button*)self;
    if (self_button->Debug) printf("Button: %s mouse entered\n",self_button->Text);
    self->MouseInsideLast = self->MouseInside;
    self->MouseInside = true;
}

void Button_OnMouseExit(GuiElement* self, GraphicsManager* gm) 
{
    Button* self_button = (Button*)self;
    if (self_button->Debug) printf("Button: %s mouse exited\n",self_button->Text);
    self->MouseInsideLast = self->MouseInside;
    self->MouseInside = false;
}

void Button_OnMouseClick(GuiElement* self, GraphicsManager* gm)
{
    Button* self_button = (Button*)self;
    if (self_button->Debug) printf("Button: %s clicked\n",self_button->Text);
}

void Button_OnMouseRelease(GuiElement* self, GraphicsManager* gm)
{
    Button* self_button = (Button*)self;
    if (self_button->Debug) printf("Button: %s released\n",self_button->Text);
}
