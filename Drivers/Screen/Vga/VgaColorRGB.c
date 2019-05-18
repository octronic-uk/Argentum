#include "VgaColorRGB.h"
#include <stdio.h>


bool VgaColorRGB_EqualTo(VgaColorRGB a, VgaColorRGB b)
{
    return a.Red == b.Red && a.Green == b.Green && a.Blue == b.Blue;
}


void VgaColorRGB_Debug(VgaColorRGB color)
{
    printf("VgaColorRGB: R=0x%x, G=0x%x B=0x%x\n", color.Red, color.Green, color.Blue);
}