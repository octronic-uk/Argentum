#include "RGB.h"
#include <stdio.h>


bool RGB_EqualTo(RGB a, RGB b)
{
    return a.Red == b.Red && a.Green == b.Green && a.Blue == b.Blue;
}

uint32_t RGB_ToUnit32(RGB rgb)
{
    return (rgb.Red << 16) | (rgb.Green << 8) | rgb.Blue;
}

void RGB_Debug(RGB color)
{
    printf("RGB: R=0x%x, G=0x%x B=0x%x\n", color.Red, color.Green, color.Blue);
}