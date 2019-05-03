#pragma once

/*
    https://wiki.osdev.org/PS/2_Keyboard

    Scancode Set 2
*/

#define SCANCODE_NONE     0x00
#define SCANCODE_EXTENDED 0xE0
#define SCANCODE_RELEASED 0xF0

// F Keys
#define SCANCODE_F1  0x05 // F1 released
#define SCANCODE_F2  0x06 // F2 pressed
#define SCANCODE_F3  0x04 // F3 pressed
#define SCANCODE_F4  0x0C // F4 pressed
#define SCANCODE_F5  0x03 // F5 pressed
#define SCANCODE_F6  0x0B // F6 pressed
#define SCANCODE_F7  0x83 // F7 pressed
#define SCANCODE_F8  0x0A // F8 pressed
#define SCANCODE_F9  0x01 // F9 pressed
#define SCANCODE_F10 0x09 // F10 pressed
#define SCANCODE_F11 0x78 // F11 pressed
#define SCANCODE_F12 0x07 // F12 pressed
// Alpha Keys
#define SCANCODE_A 0x1C // A pressed
#define SCANCODE_B 0x32 // B pressed
#define SCANCODE_C 0x21 // C pressed
#define SCANCODE_D 0x23 // D pressed
#define SCANCODE_E 0x24 // E pressed
#define SCANCODE_F 0x2B // F pressed
#define SCANCODE_G 0x34 // G pressed
#define SCANCODE_H 0x33 // H pressed
#define SCANCODE_I 0x43 // I pressed
#define SCANCODE_J 0x3B // J pressed
#define SCANCODE_K 0x42 // K pressed
#define SCANCODE_L 0x4B // L pressed
#define SCANCODE_M 0x3A // M pressed
#define SCANCODE_N 0x31 // N pressed
#define SCANCODE_O 0x44 // O pressed
#define SCANCODE_P 0x4D // P pressed
#define SCANCODE_Q 0x15 // Q pressed
#define SCANCODE_R 0x2D // R pressed
#define SCANCODE_S 0x1B // S pressed
#define SCANCODE_T 0x2C // T pressed
#define SCANCODE_U 0x3C // U pressed
#define SCANCODE_V 0x2A // V pressed
#define SCANCODE_W 0x1D // W pressed
#define SCANCODE_X 0x22 // X pressed
#define SCANCODE_Y 0x35 // Y pressed
#define SCANCODE_Z 0x1A // Z pressed
// Number Keys
#define SCANCODE_1 0x16 // 1 pressed
#define SCANCODE_2 0x1E // 2 pressed
#define SCANCODE_3 0x26 // 3 pressed
#define SCANCODE_4 0x25 // 4 pressed
#define SCANCODE_5 0x2E // 5 pressed
#define SCANCODE_6 0x36 // 6 pressed
#define SCANCODE_7 0x3D // 7 pressed
#define SCANCODE_8 0x3E // 8 pressed
#define SCANCODE_9 0x46 // 9 pressed
#define SCANCODE_0 0x45 // 0 (zero) pressed
// Keypad
#define SCANCODE_KP_1         0x69 // (keypad) 1 pressed
#define SCANCODE_KP_2         0x72 // (keypad) 2 pressed
#define SCANCODE_KP_3         0x7A // (keypad) 3 pressed
#define SCANCODE_KP_4         0x6B // (keypad) 4 pressed
#define SCANCODE_KP_5         0x73 // (keypad) 5 pressed
#define SCANCODE_KP_6         0x74 // (keypad) 6 pressed
#define SCANCODE_KP_7         0x6C // (keypad) 7 pressed
#define SCANCODE_KP_8         0x75 // (keypad) 8 pressed
#define SCANCODE_KP_9         0x7D // (keypad) 9 pressed
#define SCANCODE_KP_0         0x70 // (keypad) 0 pressed
#define SCANCODE_KP_FULLSTOP  0x71 // (keypad) . pressed
#define SCANCODE_KP_ASTERISK  0x7C // (keypad) * pressed
#define SCANCODE_KP_PLUS      0x79 // (keypad) + pressed
#define SCANCODE_KP_MINUS     0x7B // (keypad) - pressed
// Punctuation
#define SCANCODE_SPACE        0x29 // space pressed
#define SCANCODE_COMMA        0x41 // , pressed
#define SCANCODE_FULLSTOP     0x49 // . pressed
#define SCANCODE_FWDSLASH     0x4A // / pressed
#define SCANCODE_SEMICOLON    0x4C //  ; pressed
#define SCANCODE_MINUS        0x4E // - pressed
#define SCANCODE_SINGLE_QUOTE 0x52 // ' pressed
#define SCANCODE_SQ_BRACKET_L 0x54 // [ pressed
#define SCANCODE_EQUALS       0x55 // equals pressed
#define SCANCODE_ENTER        0x5A // enter pressed 
#define SCANCODE_BACKSLASH    0x5D // \ pressed
#define SCANCODE_SQ_BRACKET_R 0x5B // ] pressed
#define SCANCODE_BACKTICK     0x0E // ` (back tick) pressed
#define SCANCODE_TAB          0x0D // tab pressed
// Control Keys
#define SCANCODE_CAPSLOCK     0x58 // CapsLock pressed
#define SCANCODE_BACKSPACE    0x66 // backspace pressed
#define SCANCODE_ESCAPE       0x76 // escape pressed
#define SCANCODE_NUMLOCK      0x77 // NumberLock pressed
#define SCANCODE_SCROLL_LOCK  0x7E // ScrollLock pressed
#define SCANCODE_LEFT_ALT     0x11 // left alt pressed
#define SCANCODE_LEFT_CTRL    0x14 // left control pressed
#define SCANCODE_LEFT_SHIFT   0x12 // left shift pressed
#define SCANCODE_RIGHT_SHIFT  0x59 // right shift pressed

// Extended E0 set 
// _EXT means that the scancode is preceeded by 0xE0
#define SCANCODE_KP_FWD_SLASH_EXT /*0xE0*/0x4A // (keypad) / pressed
#define SCANCODE_KP_ENTER_EXT     /*0xE0*/0x5A // (keypad) enter pressed
#define SCANCODE_RIGHT_ALT_EXT    /*0xE0*/0x11 // right alt pressed
#define SCANCODE_RIGHT_CTRL_EXT   /*0xE0*/0x14 // right control pressed
#define SCANCODE_LEFT_GUI_EXT     /*0xE0*/0x1F // left GUI pressed
#define SCANCODE_RIGHT_GUI_EXT    /*0xE0*/0x27 // right GUI pressed
#define SCANCODE_APPS_EXT         /*0xE0*/0x2F // apps pressed
#define SCANCODE_END_EXT          /*0xE0*/0x69 // end pressed
#define SCANCODE_HOME_EXT         /*0xE0*/0x6C // home pressed
#define SCANCODE_INSERT_EXT       /*0xE0*/0x70 // insert pressed
#define SCANCODE_DELETE_EXT       /*0xE0*/0x71 // delete pressed
#define SCANCODE_PG_DOWN_EXT      /*0xE0*/0x7A // page down pressed
#define SCANCODE_PG_UP_EXT        /*0xE0*/0x7D // page up pressed
#define SCANCODE_UP_EXT           /*0xE0*/0x75 // cursor up pressed
#define SCANCODE_DOWN_EXT         /*0xE0*/0x72 // cursor down pressed
#define SCANCODE_LEFT_EXT         /*0xE0*/0x6B // cursor left pressed
#define SCANCODE_RIGHT_EXT        /*0xE0*/0x74 // cursor right pressed