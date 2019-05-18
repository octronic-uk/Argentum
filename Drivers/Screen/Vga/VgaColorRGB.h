#pragma once

#include <stdint.h>
#include <stdbool.h>

struct VgaColorRGB
{
    uint8_t Red;
    uint8_t Green;
    uint8_t Blue;
} __attribute__((packed));

typedef struct VgaColorRGB VgaColorRGB;

bool VgaColorRGB_EqualTo(VgaColorRGB a, VgaColorRGB b);
void VgaColorRGB_Debug(VgaColorRGB color);