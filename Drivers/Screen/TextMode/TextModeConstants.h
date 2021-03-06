#pragma once

// Text Mode

#define TEXTMODE_VBP 0x000B8000

#define TEXTMODE_TAB_SIZE 4
#define TEXTMODE_COLUMNS_80 80
#define TEXTMODE_ROWS_25 25

#define TEXTMODE_COLUMNS_90 90
#define TEXTMODE_ROWS_60 60 

#define TEXTMODE_BYTES_PER_ELEMENT 2

#define TEXTMODE_CHAR_SIZE(x,y) (x * y)
#define TEXTMODE_ROW_SIZE_BYTES(x) (x * TEXTMODE_BYTES_PER_ELEMENT)
#define TEXTMODE_SIZE_BYTES(x,y) (TEXTMODE_CHAR_SIZE(x,y) * TEXTMODE_BYTES_PER_ELEMENT)

#define TEXTMODE_FB_IO_CMD     0x03D4
#define TEXTMODE_FB_IO_DATA    0x03D5
#define TEXTMODE_FB_CURSOR_HI  14
#define TEXTMODE_FB_CURSOR_LOW 15