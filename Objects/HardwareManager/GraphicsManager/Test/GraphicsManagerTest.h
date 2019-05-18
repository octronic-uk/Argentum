#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef struct GraphicsManager GraphicsManager;
typedef struct LinkedList LinkedList;
typedef struct VgaColorRGB VgaColorRGB;
typedef struct Image Image;

void GraphicsManagerTest_RunSuite(GraphicsManager* gm);

void GraphicsManagerTest_LowLevelTest(GraphicsManager* gm);
void GraphicsManagerTest_LoadBMP(GraphicsManager* gm);

uint32_t GraphicsManagerTest_WriteImagePalette(Image* img);
void GraphicsManagerTest_DebugData(uint8_t* data, uint32_t size);
void GraphicsManagerTest_DebugDataUnformatted(uint8_t* data, uint32_t size);