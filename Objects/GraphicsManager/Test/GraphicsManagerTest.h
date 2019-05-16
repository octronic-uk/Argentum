#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct GraphicsManager GraphicsManager;
typedef struct LinkedList LinkedList;
typedef struct VgaColorRGB VgaColorRGB;

void GraphicsManagerTest_RunSuite(GraphicsManager* gm);

void GraphicsManagerTest_LowLevelTest(GraphicsManager* gm);
void GraphicsManagerTest_LoadBMP(GraphicsManager* gm);

uint32_t GraphicsManagerTest_CreatePalette(LinkedList* palette, VgaColorRGB* data, uint32_t size);
uint8_t GraphicsManagerTest_PaletteIndex(LinkedList* palette, VgaColorRGB* rgb);
bool GraphicsManagerTest_InPalette(LinkedList* palette, VgaColorRGB *rgb);
void GraphicsManagerTest_DebugData(uint8_t* data, uint32_t size);
void GraphicsManagerTest_DebugDataUnformatted(uint8_t* data, uint32_t size);