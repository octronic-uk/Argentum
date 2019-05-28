#include "TextBox.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Drivers/PS2/Scancode.h>
#include <Objects/GraphicsManager/GraphicsManager.h>
#include <Objects/GraphicsManager/GraphicsManagerConstants.h>

bool TextBox_Constructor(TextBox* self)
{
    printf("TextBox: Constructor\n");
    memset(self,0,sizeof(TextBox));
    GuiElement_Constructor(&self->GuiElement);
    self->Debug = false;
    // Default Dimensions
    self->GuiElement.Area.W = TEXTBOX_DEFAULT_WIDTH;
    self->GuiElement.Area.H = TEXTBOX_DEFAULT_HEIGHT;
    self->TextPaddingX = 4;
    self->TextPaddingY = 4;
    self->CharacterWidth = 9;
    self->MaxVisibleChars = self->GuiElement.Area.W / self->CharacterWidth;
    self->CursorPadding = 3; 
    self->GuiElement.NeedsRedraw = true;

    // Colors
    self->Color = DEFAULT_PALETTE_LIGHT_GREY;
    self->HoverColor = DEFAULT_PALETTE_WHITE;

    // Set callback functions
    self->GuiElement.DrawFunction = TextBox_OnDraw;
    self->GuiElement.UpdateFunction = TextBox_OnUpdate;
    self->GuiElement.OnMouseClickFunction = TextBox_OnMouseClick;
    self->GuiElement.OnMouseReleaseFunction = TextBox_OnMouseRelease;
    self->GuiElement.OnMouseEnterFunction = TextBox_OnMouseEnter;
    self->GuiElement.OnMouseExitFunction = TextBox_OnMouseExit;
    self->GuiElement.OnKeyboardEventFunction = TextBox_OnKeyboardEventFunction;

    
    return true;
}

void TextBox_Destructor(TextBox* self)
{
    printf("TextBox: Destructor\n");
}

void TextBox_SetText(TextBox* self, char* text)
{
    strncpy(&self->Text[0],text,TEXTBOX_BUFFER_SIZE);
}

void TextBox_OnDraw(GuiElement* self, GraphicsManager* gm)
{
    TextBox* self_textbox = (TextBox*)self;
    if (self_textbox->Debug) printf("TextBox: Drawing button with text %s\n",self_textbox->Text);

    // Borders
    int x, y, border_max_x, border_max_y;

    border_max_x = self->Area.X + self->Area.W;
    border_max_y = self->Area.Y + self->Area.H;

    uint32_t color = self->MouseInside ? self_textbox->HoverColor : self_textbox->Color;
    // Clear box contents with background
    GraphicsManager_ReplaceBackgroundArea(gm, self->Area);
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
    
    GraphicsManager_DrawText(gm, 
        self->Area.X + self_textbox->TextPaddingX, 
        self->Area.Y + self_textbox->TextPaddingY, color, self_textbox->Text);

    uint32_t cursor_x = self->Area.X + (strlen(self_textbox->Text) * self_textbox->CharacterWidth) + 1;
    uint32_t cursor_max_y = self->Area.Y + self->Area.H - self_textbox->CursorPadding;

    // Cursor line 
    for (y=self->Area.Y+self_textbox->CursorPadding; y<=cursor_max_y; y++)
    {
        GraphicsManager_WritePixelToFramebuffer(gm, cursor_x, y, color);
    }

    self->NeedsRedraw = false;
}

void TextBox_OnUpdate(struct GuiElement* self, GraphicsManager* gm)
{
    TextBox* self_textbox = (TextBox*)self;
    if (self->MouseInsideLast != self->MouseInside)
    {
        self->NeedsRedraw = true;
    }
    self->MouseInsideLast = self->MouseInside;
}

void TextBox_OnMouseEnter(GuiElement* self, GraphicsManager* gm) 
{
    TextBox* self_textbox = (TextBox*)self;
    if (self_textbox->Debug) printf("TextBox: %s mouse entered\n",self_textbox->Text);
    self->MouseInsideLast = self->MouseInside;
    self->MouseInside = true;
}

void TextBox_OnMouseExit(GuiElement* self, GraphicsManager* gm) 
{
    TextBox* self_textbox = (TextBox*)self;
    if (self_textbox->Debug) printf("TextBox: %s mouse exited\n",self_textbox->Text);
    self->MouseInsideLast = self->MouseInside;
    self->MouseInside = false;
}

void TextBox_OnMouseClick(GuiElement* self, GraphicsManager* gm)
{
    TextBox* self_textbox = (TextBox*)self;
    if (self_textbox->Debug) printf("TextBox: %s clicked\n",self_textbox->Text);
}

void TextBox_OnMouseRelease(GuiElement* self, GraphicsManager* gm)
{
    TextBox* self_textbox = (TextBox*)self;
    if (self_textbox->Debug) printf("TextBox: %s released\n",self_textbox->Text);
}

void TextBox_OnKeyboardEventFunction(struct GuiElement* self, GraphicsManager* gm, KeyboardEvent e)
{
    TextBox* self_textbox = (TextBox*)self;
    if (self_textbox->Debug) printf("TextBox: KeyboardEvent\n");

    if (e.Pressed) 
    { 
        if (e.Scancode == SCANCODE_BACKSPACE)
        {
            uint32_t len = strlen(self_textbox->Text);
            if (len-1 >= 0) 
            {
                self_textbox->Text[len-1] = 0;
                self->NeedsRedraw = true;
            }
        }
        else
        {
            if (e.Character)
            {
                uint32_t len = strlen(self_textbox->Text);
                if (len < TEXTBOX_BUFFER_SIZE-1);
                {
                    self_textbox->Text[len] = e.Character;
                    self_textbox->Text[len+1] = 0;
                }
                // always cap
                self_textbox->Text[TEXTBOX_BUFFER_SIZE-1] = 0;
            }
        }
    }
}