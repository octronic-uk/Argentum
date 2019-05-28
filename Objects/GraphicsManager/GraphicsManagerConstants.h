#pragma once

#include <stdint.h>
#include "RGB.h"

#define CURSOR_W 8
#define CURSOR_H 16

#define DEFAULT_PALETTE_BLACK      0x00000000
#define DEFAULT_PALETTE_WHITE      0x00FFFFFF
#define DEFAULT_PALETTE_LIGHT_GREY 0x00CCCCCC
#define DEFAULT_PALETTE_DARK_GREY  0x00333333

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