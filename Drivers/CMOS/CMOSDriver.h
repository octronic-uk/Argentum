#pragma once

#include "CMOSConstants.h"
#include <stdint.h>
#include <stdbool.h>

struct RTCState
{
    uint8_t Seconds;    
    uint8_t Minutes;
    uint8_t Hours;
    uint8_t Weekday;
    uint8_t DayOfTheMonth;
    uint8_t Month;
    uint8_t Year;
    uint8_t Century;
    uint8_t StatusA;
    uint8_t StatusB;
};

uint8_t CMOSDriver_ReadFloppyConfiguration(bool nmi_enable);
struct RTCState CMOSDriver_ReadRTCState(bool nmi_enable);

