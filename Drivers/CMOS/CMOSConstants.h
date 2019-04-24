#pragma once

#define CMOS_IO_PORT_ADDRESS 0x70
#define CMOS_IO_PORT_DATA    0x71

/*
    Value   Drive Type
        00h	no drive
        01h	360 KB 5.25 Drive
        02h	1.2 MB 5.25 Drive
        03h	720 KB 3.5 Drive
        04h	1.44 MB 3.5 Drive
        05h	2.88 MB 3.5 drive
*/
#define CMOS_FLOPPY_CONF_NO_DRIVE 0x00
#define CMOS_FLOPPY_CONF_525_360  0x01
#define CMOS_FLOPPY_CONF_525_1200 0x02
#define CMOS_FLOPPY_CONF_35_720   0x03
#define CMOS_FLOPPY_CONF_35_1440  0x04
#define CMOS_FLOPPY_CONF_35_2880  0x05
#define CMOS_REGISTER_FLOPPY_CONF 0x10

/*
    Realtime Clock Registers
    Register  Contents
        0x00      Seconds
        0x02      Minutes
        0x04      Hours
        0x06      Weekday
        0x07      Day of Month
        0x08      Month
        0x09      Year
        0x32      Century (maybe)
        0x0A      Status Register A
        0x0B      Status Register B
*/
#define CMOS_REGISTER_RTC_SECONDS      0x00
#define CMOS_REGISTER_RTC_MINUTES      0x02
#define CMOS_REGISTER_RTC_HOURS        0x04
#define CMOS_REGISTER_RTC_WEEKDAY      0x06
#define CMOS_REGISTER_RTC_DAY_OF_MONTH 0x07
#define CMOS_REGISTER_RTC_MONTH        0x08
#define CMOS_REGISTER_RTC_YEAR         0x09
#define CMOS_REGISTER_RTC_CENTURY      0x32
#define CMOS_REGISTER_RTC_STATUS_A     0x0A
#define CMOS_REGISTER_RTC_STATUS_B     0x0B