#include "ScancodeParser.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "Scancode.h"

bool ScancodeParser_Constructor(struct ScancodeParser* self)
{
    printf("ScancodeParser: Constructing");
    memset(self,0,sizeof(struct ScancodeParser));
    self->Debug = false;
    self->State = STATE_START;
    self->Shift = false;
    self->Ctrl = false;
    self->Alt = false;
    ScancodeParser_ClearCurrentAction(self);
    return true;
}

void ScancodeParser_ParseScancode(struct ScancodeParser* self, uint8_t next_byte)
{
    if (self->Debug) printf("ScancodeParser: Parsing 0x%x\n",next_byte);

    if (next_byte)
    {
        switch (self->State)
        {
            case STATE_START:
                ScancodeParser_ClearCurrentAction(self);
                if (self->Debug) printf("ScancodeParser: START\n");
                if (next_byte == SCANCODE_EXTENDED)
                {
                    self->State = STATE_GOT_EXTENDED;
                }
                else if (next_byte == SCANCODE_RELEASED)
                {
                    self->CurrentAction.KeyState = KEY_STATE_RELEASED;
                    self->State = STATE_GOT_RELEASED;
                }
                else
                {
                    self->CurrentAction.KeyState = KEY_STATE_PRESSED;
                    self->CurrentAction.Scancode = next_byte;
                    self->CurrentAction.Valid = true;
                    self->State = STATE_START;
                }
                break;
            case STATE_GOT_EXTENDED:
                if (self->Debug) printf("ScancodeParser: GOT_EXTENDED\n");
                if (next_byte == SCANCODE_RELEASED)
                {
                    self->CurrentAction.KeyState = KEY_STATE_RELEASED;
                    self->State = STATE_GOT_RELEASED;
                }
                else
                {
                    self->CurrentAction.KeyState = KEY_STATE_PRESSED;
                    self->CurrentAction.Scancode = next_byte;
                    self->CurrentAction.Valid = true;
                    self->State = STATE_START;
                }
                break;
            case STATE_GOT_RELEASED:
                if (self->Debug) printf("ScancodeParser: GOT_RELEASED\n");
                self->State = STATE_START;
                self->CurrentAction.Scancode = next_byte;
                self->CurrentAction.Valid = true;
                break;
        }
    }

    // Handle modifier keys immediately
    if (self->CurrentAction.Valid)
    {
        if (self->CurrentAction.Scancode == SCANCODE_LEFT_SHIFT || 
            self->CurrentAction.Scancode == SCANCODE_RIGHT_SHIFT)
        {
            self->Shift = self->CurrentAction.KeyState == KEY_STATE_PRESSED;
            ScancodeParser_ClearCurrentAction(self);
        }
        else if (self->CurrentAction.Scancode == SCANCODE_LEFT_ALT || 
                 self->CurrentAction.Scancode == SCANCODE_RIGHT_ALT_EXT)
        {
            self->Alt = self->CurrentAction.KeyState == KEY_STATE_PRESSED;
            ScancodeParser_ClearCurrentAction(self);
        }
        else if (self->CurrentAction.Scancode == SCANCODE_LEFT_CTRL || 
                 self->CurrentAction.Scancode == SCANCODE_RIGHT_CTRL_EXT)
        {
            self->Ctrl = self->CurrentAction.KeyState == KEY_STATE_PRESSED;
            ScancodeParser_ClearCurrentAction(self);
        }
    }
}

void ScancodeParser_ClearCurrentAction(struct ScancodeParser* self)
{
    if (self->Debug) printf("ScancodeParser: Clearing Current Action\n");
    self->CurrentAction.KeyState = KEY_STATE_NONE;
    self->CurrentAction.Scancode = SCANCODE_NONE;
    self->CurrentAction.Valid = false;
}

int32_t ScancodeParser_GetChar(struct ScancodeParser* self)
{
    if (!self->CurrentAction.Valid) return -1;

    if (self->Shift)
    {
        switch (self->CurrentAction.Scancode)
        {
            // Alpha
            case SCANCODE_A: return 'A'; 
            case SCANCODE_B: return 'B'; 
            case SCANCODE_C: return 'C'; 
            case SCANCODE_D: return 'D'; 
            case SCANCODE_E: return 'E'; 
            case SCANCODE_F: return 'F'; 
            case SCANCODE_G: return 'G'; 
            case SCANCODE_H: return 'H'; 
            case SCANCODE_I: return 'I'; 
            case SCANCODE_J: return 'J'; 
            case SCANCODE_K: return 'K'; 
            case SCANCODE_L: return 'L'; 
            case SCANCODE_M: return 'M'; 
            case SCANCODE_N: return 'N'; 
            case SCANCODE_O: return 'O'; 
            case SCANCODE_P: return 'P'; 
            case SCANCODE_Q: return 'Q'; 
            case SCANCODE_R: return 'R'; 
            case SCANCODE_S: return 'S'; 
            case SCANCODE_T: return 'T'; 
            case SCANCODE_U: return 'U'; 
            case SCANCODE_V: return 'V'; 
            case SCANCODE_W: return 'W'; 
            case SCANCODE_X: return 'X'; 
            case SCANCODE_Y: return 'Y'; 
            case SCANCODE_Z: return 'Z'; 
            // Numbers
            case SCANCODE_1: return '!'; 
            case SCANCODE_2: return '@'; 
            case SCANCODE_3: return '#'; 
            case SCANCODE_4: return '$'; 
            case SCANCODE_5: return '%'; 
            case SCANCODE_6: return '^'; 
            case SCANCODE_7: return '&'; 
            case SCANCODE_8: return '*'; 
            case SCANCODE_9: return '('; 
            case SCANCODE_0: return ')';
            // Punctuation
            case SCANCODE_SPACE:        return ' ';
            case SCANCODE_COMMA:        return '<';
            case SCANCODE_FULLSTOP:     return '>';
            case SCANCODE_FWDSLASH:     return '?';
            case SCANCODE_SEMICOLON:    return ':';
            case SCANCODE_MINUS:        return '_';
            case SCANCODE_SINGLE_QUOTE: return '"';
            case SCANCODE_SQ_BRACKET_L: return '{';
            case SCANCODE_EQUALS:       return '+';
            case SCANCODE_ENTER:        return '\n'; 
            case SCANCODE_BACKSLASH:    return '|';
            case SCANCODE_SQ_BRACKET_R: return '}';
            case SCANCODE_BACKTICK:     return '~';
            case SCANCODE_TAB:          return '\t';
        }
    }
    else
    {
        switch (self->CurrentAction.Scancode)
        {
            // Alpha
            case SCANCODE_A: return 'a'; 
            case SCANCODE_B: return 'b'; 
            case SCANCODE_C: return 'c'; 
            case SCANCODE_D: return 'd'; 
            case SCANCODE_E: return 'e'; 
            case SCANCODE_F: return 'f'; 
            case SCANCODE_G: return 'g'; 
            case SCANCODE_H: return 'h'; 
            case SCANCODE_I: return 'i'; 
            case SCANCODE_J: return 'j'; 
            case SCANCODE_K: return 'k'; 
            case SCANCODE_L: return 'l'; 
            case SCANCODE_M: return 'm'; 
            case SCANCODE_N: return 'n'; 
            case SCANCODE_O: return 'o'; 
            case SCANCODE_P: return 'p'; 
            case SCANCODE_Q: return 'q'; 
            case SCANCODE_R: return 'r'; 
            case SCANCODE_S: return 's'; 
            case SCANCODE_T: return 't'; 
            case SCANCODE_U: return 'u'; 
            case SCANCODE_V: return 'v'; 
            case SCANCODE_W: return 'w'; 
            case SCANCODE_X: return 'x'; 
            case SCANCODE_Y: return 'y'; 
            case SCANCODE_Z: return 'z'; 
            // Numbers
            case SCANCODE_1: return '1'; 
            case SCANCODE_2: return '2'; 
            case SCANCODE_3: return '3'; 
            case SCANCODE_4: return '4'; 
            case SCANCODE_5: return '5'; 
            case SCANCODE_6: return '6'; 
            case SCANCODE_7: return '7'; 
            case SCANCODE_8: return '8'; 
            case SCANCODE_9: return '9'; 
            case SCANCODE_0: return '0';
            // Punctuation
            case SCANCODE_SPACE:        return ' ';
            case SCANCODE_COMMA:        return ',';
            case SCANCODE_FULLSTOP:     return '.';
            case SCANCODE_FWDSLASH:     return '/';
            case SCANCODE_SEMICOLON:    return ';';
            case SCANCODE_MINUS:        return '-';
            case SCANCODE_SINGLE_QUOTE: return '\'';
            case SCANCODE_SQ_BRACKET_L: return '[';
            case SCANCODE_EQUALS:       return '=';
            case SCANCODE_ENTER:        return '\n'; 
            case SCANCODE_BACKSLASH:    return '\\';
            case SCANCODE_SQ_BRACKET_R: return ']';
            case SCANCODE_BACKTICK:     return '`';
            case SCANCODE_TAB:          return '\t';
        }
    }
    return -1;
}

void ScancodeParser_WaitForKeyPressAction(struct ScancodeParser* self)
{
    while (self->CurrentAction.KeyState != KEY_STATE_PRESSED) {}
}