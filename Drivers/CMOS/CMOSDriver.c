#include "CMOSDriver.h"

#include <string.h>
#include <unistd.h>
#include <Drivers/IO/IODriver.h>

uint8_t CMOSDriver_ReadFloppyConfiguration(bool nmi_enable)
{
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_FLOPPY_CONF | (nmi_enable << 7));
    usleep(10);
    return IO_ReadPort8b(CMOS_IO_PORT_DATA);
}

struct RTCState CMOSDriver_ReadRTC(bool nmi_enable)
{
   struct RTCState state; 

    // Seconds    
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_SECONDS | (nmi_enable << 7));
    state.Seconds = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Minutes
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_MINUTES | (nmi_enable << 7));
    state.Minutes = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Hours
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_HOURS | (nmi_enable << 7));
    state.Hours = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Weekday
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_WEEKDAY | (nmi_enable << 7));
    state.Weekday = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Day of the Month
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_DAY_OF_MONTH | (nmi_enable << 7));
    state.DayOfTheMonth = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Month
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_MONTH | (nmi_enable << 7));
    state.Month = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Year
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_YEAR | (nmi_enable << 7));
    state.Year = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // Century
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_CENTURY | (nmi_enable << 7));
    state.Century = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // StatusA
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_STATUS_A | (nmi_enable << 7));
    state.StatusA = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    // StatusB
    IO_WritePort8b(CMOS_IO_PORT_ADDRESS, CMOS_REGISTER_RTC_STATUS_B | (nmi_enable << 7));
    state.StatusB = IO_ReadPort8b(CMOS_IO_PORT_DATA);

    return state;
}