#pragma once

#include <stdint.h>
#include <Drivers/Screen/Vga/VgaColorRGB.h>

#define PALETTE_INDEX_BLACK 0
#define PALETTE_INDEX_WHITE 1
#define PALETTE_INDEX_BACKGROUND_COLOR 2
#define PALETTE_INDEX_UI_BACKGROUND 3
#define PALETTE_INDEX_BORDER_COLOR 4
#define PALETTE_INDEX_BORDER_HOVER_COLOR 5 

static VgaColorRGB DefaultPalette[] = 
{
    // 0
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, // Black
    // 1 
    { .Red = 0XFF, .Green = 0XFF, .Blue = 0xFF }, // White
    // 2 
    { .Red = 0x20, .Green = 0x20, .Blue = 0x20 }, // Background
    // 3 
    { .Red = 0x1B, .Green = 0x5E, .Blue = 0x95 }, // Ui Background
    // 4 
    { .Red = 0x60, .Green = 0x60, .Blue = 0x60 }, // Ui Border
    // 5 
    { .Red = 0xA0, .Green = 0xA0, .Blue = 0xA0 }, // Ui Border Hover 
    // 6 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 7 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 8 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 9 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 10 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 11 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 12 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 13 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 14 
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }, 
    // 15
    { .Red = 0x00, .Green = 0x00, .Blue = 0x00 }
};


const static uint8_t PointerData[] = 
{
    0b10000000,
    0b11000000,
    0b11100000,
    0b11110000,
    0b11111000,
    0b11111100,
    0b11111110,
    0b11111111,
    0b11111100,
    0b11111000,
    0b11011100,
    0b10001100,
    0b00001110,
    0b00000110,
    0b00000111,
    0b00000011
};