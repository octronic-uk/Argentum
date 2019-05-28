#pragma once

#include <stdint.h>
#include <stdbool.h>

struct RGB
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
} __attribute__((packed));

typedef struct RGB RGB;

bool     RGB_EqualTo(RGB a, RGB b);
void     RGB_Debug(RGB color);
uint32_t RGB_ToUnit32(RGB rgb);