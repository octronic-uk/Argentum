#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "../GuiElement.h"

#define TEXTBOX_BUFFER_SIZE    64
#define TEXTBOX_DEFAULT_WIDTH  200 
#define TEXTBOX_DEFAULT_HEIGHT 24

struct TextBox
{
    GuiElement GuiElement;
    bool Debug;
    char Text[TEXTBOX_BUFFER_SIZE];
    uint32_t Color;
    uint32_t HoverColor;
    uint8_t TextPaddingX;
    uint8_t TextPaddingY;
    uint32_t MaxVisibleChars;
    uint8_t CharacterWidth;
    uint8_t CursorPadding;
    uint32_t CursorIndex;
};
typedef struct TextBox TextBox;

bool TextBox_Constructor(TextBox* self);
void TextBox_Destructor(TextBox* self);

void TextBox_SetText(TextBox* self, char* text);
void TextBox_OnDraw(GuiElement* self, GraphicsManager* gm);
void TextBox_OnUpdate(GuiElement* self, GraphicsManager* gm);
void TextBox_OnMouseEnter(GuiElement* self, GraphicsManager* gm);
void TextBox_OnMouseExit(GuiElement* self, GraphicsManager* gm);
void TextBox_OnMouseClick(GuiElement* self, GraphicsManager* gm);
void TextBox_OnMouseRelease(GuiElement* self, GraphicsManager* gm);
void TextBox_OnKeyboardEventFunction(struct GuiElement* self, GraphicsManager* gm, KeyboardEvent e);