#include "HardwareManagerTest.h"

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <Objects/Kernel/Kernel.h>

#include "../GraphicsManager/Test/GraphicsManagerTest.h"
#include "../StorageManager/Test/StorageManagerTest.h"
#include "../InputManager/Test/InputManagerTest.h"

extern Kernel _Kernel;

void HardwareManagerTest_RunSuite(HardwareManager* hm)
{
    //StorageManagerTest_RunSuite(&hm->StorageManager);
	GraphicsManagerTest_RunSuite(&hm->GraphicsManager);
	//InputManagerTest_RunSuite(&hm->InputManager);
}